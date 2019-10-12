#
/*
 *    Copyright (C) 2014, 2015
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J.
 *    Many of the ideas as implemented in SDR-J are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are recognized.
 *
 *    SDR-J is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    SDR-J is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with SDR-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	We made two implementations of the ofdmProcessor,
 *	one with an explicit "state", the other one with
 *	the state of the computation implicit
 *	This happens to be the explicit one
 */
#include	"ofdm-processor-b.h"
#include	"gui.h"
#include	"Xtan2.h"
//
//	States for our little state machine
//
#define	OFDM_INIT		0100
#define	SYNC_ON_NULL		0101
#define	CONTINUE_ON_NULL	0102
#define	SYNC_ON_END_OF_NULL	0103
#define	OFDM_SYNC_ON_PHASE	0104
#define	OFDM_PRS		0105
#define	OFDM_SYMBOLS		0106
#define	PRE_OFDM_INIT		0107

static inline
DSPFLOAT decayingAverage (DSPFLOAT old, DSPFLOAT input, DSPFLOAT weight) {
	if (weight <= 1)
	   return input;
	return input * (1.0 / weight) + old * (1.0 - (1.0 / weight));
}

	ofdm_processor_b::ofdm_processor_b	(virtualInput *theRig,
	                                         DabParams	*p,
	                                         RadioInterface *mr,
	                                         mscHandler 	*msc,
	                                         ficHandler 	*fic,
	                                         int16_t	fillSize,
	                                         RingBuffer<DSPCOMPLEX> *scopeBuffer,
	                                         RingBuffer<DSPCOMPLEX> *iqBuffer,
	                                         bool	   lightRun):
	         ofdm_processor (theRig, p, mr,
	                         msc, fic, fillSize, scopeBuffer,
	                         iqBuffer, lightRun) {
	decoderState	= PRE_OFDM_INIT;
	resetRequest	= false;
	start (QThread::TimeCriticalPriority);
}

	ofdm_processor_b::~ofdm_processor_b	(void) {
	running	= false;
	fprintf (stderr, "running set to false\n");
	while (!isFinished ())
	   usleep (100);
}

void	ofdm_processor_b::reset	(void) {
	resetRequest	= true;
}
//
void	ofdm_processor_b::run	 (void) {
int32_t	i, j;
double		V_Tnull;
DSPCOMPLEX	currentValue;
int32_t		amount;
int32_t		sampleCnt		= 0;
DSPFLOAT	local_freqDelta		= 0;
double		localLevel		= 0;
DSPFLOAT	avg_freqDelta		= 0;
DSPFLOAT	frame_freqDelta		= 0;
DSPFLOAT	small_frequencyCorrector	= 0;
DSPFLOAT	frequencyCorrector	= 0;
int32_t		localPhase		= 0;
int32_t		ofdmBufferIndex		= 0;
int32_t		initCount		= 0;
compAtan	myAtan;
double		syncFilterSum		= 0;

	while (running) {
	   int32_t x = theRig -> Samples ();
	   if (x < 10000) {		// pretty arbirary
	      if (!running)
	        fprintf (stderr, "not running\n");
	      usleep (10);
	      continue;
	   }
//
//	at maximum 20480 samples each time
	   amount = theRig	-> Samples ();
	   if (amount > 20480)
	      amount = 20480;
	   amount = theRig	-> getSamples (dataBuffer, amount);
//
//	First, handle the dumping flag
	   if (dumping) {
	      int32_t i;
	      float *dumpBuffer = (float *)alloca (2 * amount * sizeof (float));
	      for (i = 0; i < amount; i ++) {
	         dumpBuffer [2 * i] = real (dataBuffer [i]);
	         dumpBuffer [2 * i + 1] = imag (dataBuffer [i]);
	      }
	      sf_writef_float (dumpFile, dumpBuffer, amount);
	   }
//
//	Process a buffer, sample by sample in a single loop
	   for (i = 0; i < amount; i ++) {
	      if (resetRequest) {
	         decoderState	= PRE_OFDM_INIT;
	         resetRequest	= false;
	      }
	      currentValue = cmul (dataBuffer [i], gain);
//
//	Frequency correction is pretty expensive, therefore
//	it is coded inline
	      localPhase	-= frequencyCorrector;
//	            FreqOffset + KHz (coarseCorrector) +
//	                          small_frequencyCorrector;
	      if (localPhase < 0)
	         localPhase += INPUT_RATE;
	      else
	      if (localPhase >= INPUT_RATE)
	         localPhase -= INPUT_RATE;
	      currentValue *= oscillatorTable [localPhase];
//
//	Data is shifted, now for the averages:
//	syncFilterSum contains the sum of  (absolute values) of
//	the last T_null / 10 samples, localLevel is then the
//	average over the last T_null / 10 samples
	      syncBuffer [syncBufferIndex] = currentValue;
	      envBuffer [syncBufferIndex] = abs (currentValue);
	      V_Tnull        = 
	               envBuffer [(syncBufferIndex - T_null / 10) & syncBufferMask];
	      syncFilterSum	+= abs (currentValue) - V_Tnull;
	      localLevel 	= syncFilterSum / (T_null / 10);
	      tokenLength ++;
//
//	for the average signal level, we take about 10 lengths of
//	the null symbol. Note that in ofdm-processor-a we do this
//	by an averaging filter, which works as well
	      signalLevel	+= abs (currentValue) -
	                     abs (syncBuffer [(syncBufferIndex - 10 * T_null) & syncBufferMask]);
//
//	N times a second we show the spectrum
#define	N	8
	      if (++ sampleCnt > INPUT_RATE / N) {
	         showSpectrum	(fillSize);
	         show_fineCorrector (small_frequencyCorrector);
	         show_coarseCorrector (coarseCorrector);
	         show_avgTokenLength (avgTokenLength);
	         sampleCnt	= 0;
	      }
//
//	State dispatcher:
	      switch (decoderState) {
//
//	On a reset - either hard or soft - we should indeed
//	reset all relevant variables
	         case PRE_OFDM_INIT:
	            lowCount			= 0;
	            highCount			= 0;
	            syncFilterSum		= 0;
	            initCount			= 0;
	            tokenLength			= 0;
	            localPhase			= 0;
	            localLevel			= 0;
	            signalLevel			= 0;
	            local_freqDelta		= 0;
	            avg_freqDelta		= 0;
	            frame_freqDelta		= 0;
	            small_frequencyCorrector	= 0;
	            frequencyCorrector		= 0;
	            ofdmBufferIndex		= 0;
	            decoderState		= OFDM_INIT;
	            syncMode			= UNSYNCED;
	            setSynced	(UNSYNCED);
//
//	fall through
	         case OFDM_INIT:
//	To avoid starting up problems, we just let the machine
//	run and fill the buffer to get a more or less reliable
//	signalLevel value. 
	            if (++ initCount >= 10 * T_null) {
	               decoderState	= SYNC_ON_NULL;
	               initCount	= 0;
	            }
	            break;
//
//	It is assumed that as soon as the signal level of the buffer
//	of T_null/ 10 symbols is below a threshold (here 0.25), we are in
//	a null period. It means that the null period detection
//	happens no later than 0.1 * T_null samples
//
//	Two cases: we restart after an error:
	         case SYNC_ON_NULL:
	            if (syncMode == SYNCED) {
	               syncMode	= UNSYNCED;
	               setSynced (syncMode);
	            }
//	fall through
	         case CONTINUE_ON_NULL:
	            if (localLevel < 0.25 * signalLevel / (10 * T_null)) {
	               decoderState	= SYNC_ON_END_OF_NULL;
	               highCount	= 0;
	            }
	            if (++ highCount > 4 * T_null) {
	               decoderState	= PRE_OFDM_INIT;
	               highCount	= 0;
	            }
	            break;
//
//	As soon as the average signal level of the buffer rises above a second
//	threshold, we are out of the null period. Note that
//	we are about half a buffer length late,
//	so, the most recent sample belongs to the CP of the zero-th
//	symbol
//	we compensate that by setting the value of the pointer
//	syncPhaseIndex back into the buffer.
	         case SYNC_ON_END_OF_NULL:
	            if (localLevel > 0.75 * signalLevel / (10 * T_null)) {
	               decoderState	= OFDM_SYNC_ON_PHASE;
	               syncPhaseIndex	= 0.75 * T_null / 10;
	               syncTries	= 0;
	            }
	            else 
	            if (++ lowCount > 2 * T_null) { // ?????
	               decoderState	= SYNC_ON_NULL;
	               lowCount		= 0;
	            }
	            break;

//	basic synchronization through the phase synchronizer
	         case OFDM_SYNC_ON_PHASE:
	            if (++ syncPhaseIndex >= params -> T_u) {	// full buffer
	               syncTries += 1;
//
//	the search starts at T_u samples back
	               int32_t a = (syncBufferIndex - params -> T_u) & syncBufferMask;
//
//	phaseSynchronizer returns the index of the match, i.e. the
//	first entry of the match. Iff we would have been precise with
//	the rough estimate of the start of the zero-th symbol, the
//	index should be exactly T_s - T_u, i.e. the
//	length of the cyclic prefix (for Mode I 504 samples).
//	In practice it is somewhat longer.
	               int16_t t =
	                     phaseSynchronizer -> findIndex (a, params -> T_u);
	               if (t == -1) {	// not found
	                  if (syncTries > 3) {
//	                     fprintf (stderr, "Failing\n");
	                     decoderState = SYNC_ON_NULL;
	                     lowCount		= 0;
	                  }
	                  else 
//	otherwise, we shift the index a little and try again
	                      syncPhaseIndex = params -> T_u / 3;

	                  break;
	               }
//
//	When here we reset syncTries
	               syncTries	= 0;
//	length of the previous token is to be adjusted:
	               tokenLength	= tokenLength - (params -> T_u - t);
//
//	the tokens for the next token (the PRS symbol) are stored
//	in the ofdmBuffer
	               ofdmBufferIndex = 0;
	               for (j = t; j <= params -> T_u; j ++)
	                  ofdmBuffer [ofdmBufferIndex ++] =
	                              syncBuffer [(a + j) & syncBufferMask];
//
	               if (0.9 * params -> T_F <= tokenLength &&
	                             tokenLength <= 1.1 * params -> T_F)
	                  avgTokenLength = decayingAverage (avgTokenLength,
	                                                    tokenLength, 3);
//	... and  prepare for the next round:
	               tokenLength		= params -> T_u - t;
//	and set the Mode
	               decoderState = OFDM_PRS;
	               if (syncMode != SYNCED) {
	                  syncMode = SYNCED;
	                  setSynced (SYNCED);
	               }
	            }
	            break;
//
//	The synchronisation takes place on (Ts - Tu) samples
//	from the actual beginning, so here we only continue
//	saving samples until Tu.
	         case OFDM_PRS:
	            ofdmBuffer [ofdmBufferIndex ++] = currentValue;
	            if (ofdmBufferIndex >= T_u) {
	               ofdmDecoder	-> processBlock_0 (ofdmBuffer);
	               ofdmBufferIndex	= 0;
	               ofdmSymbolCount	= 1;
	               local_freqDelta	= 0;
	               avg_freqDelta	= 0;
	               decoderState	= OFDM_SYMBOLS;
	            }
	            break;
// 
//	we collect the samples for the ofdm symbol
//	when having Tu samples, we start fine frequency correction
//    	with the cyclic prefix
	         case OFDM_SYMBOLS:
	            ofdmBuffer [ofdmBufferIndex] = currentValue;
//
//	At the end of a block we compute the (average) small frequency offset
	            if (++ofdmBufferIndex < (int16_t)T_s)  // still collecting
	               break;
//
//	When here, we have at least a full block
	            for (int lp = 0; lp < T_s - T_u; lp ++) {
	               DSPCOMPLEX x =
	                     ofdmBuffer [ofdmBufferIndex - T_s + lp];
	               DSPCOMPLEX y =
	                     ofdmBuffer [ofdmBufferIndex - T_s + T_u + lp];
	               local_freqDelta += myAtan. argX (y * conj (x));
	            }
	            local_freqDelta	/=  (2 * M_PI * (T_s - T_u));
	            avg_freqDelta	+= local_freqDelta ;
	            ++ofdmSymbolCount;
//
//	and we immediately process the block, i.e. make it into (soft) bits
	            ofdmDecoder -> processToken (ofdmBuffer,
	                                         ibits,
	                                         ofdmSymbolCount);
//	We distinguish between FIC blocks and MSC blocks
	            if ((2 <= ofdmSymbolCount) && (ofdmSymbolCount <= 4))
	                  my_ficHandler	-> process_ficBlock (ibits,
	                                                    ofdmSymbolCount);
	            else
	            if (!lightRun)	// just a gadget for slow processors
	               my_mscHandler	-> process_mscBlock (ibits,
	                                                  ofdmSymbolCount);
//	set everything for the next block
	            ofdmBufferIndex = 0;
//
	            if ((int16_t)ofdmSymbolCount < (int16_t)(params -> L)) 
	               break;
//
//	At the end of a frame we first compute the small frequency
//	delta, based on the collected data from the blocks
	            frame_freqDelta += (avg_freqDelta / (params -> L - 1)) *
	                                     params -> carrierDiff;
	            if (frame_freqDelta > params -> carrierDiff / 2) 
	               frame_freqDelta -= params -> carrierDiff / 2;
	            if (frame_freqDelta < - params -> carrierDiff / 2)
	               frame_freqDelta += params -> carrierDiff / 2;
	            small_frequencyCorrector =
	                          decayingAverage (small_frequencyCorrector,
	                                           frame_freqDelta, 10);
//	If the auto correction is on, we look at the coarse correction
//	info from the decoder
	            if (f2Correction) {
	               coarseCorrector += ofdmDecoder -> coarseCorrector () 
	                                * (params -> carrierDiff / 1000);
//	               fprintf (stderr, "%d \n", ofdmDecoder -> getStrength ());
	            }
//
//	The total frequency offset to take into account at the next round
	            frequencyCorrector = Khz (coarseCorrector)  + 
	                                 small_frequencyCorrector;
	            ofdmSymbolCount	= 0;
	            decoderState	= CONTINUE_ON_NULL;
	            lowCount		= 0;
	            break;
   
	         default:;	// This really cannot happen
	      }

	      syncBufferIndex = (syncBufferIndex + 1) & syncBufferMask;
	   }
	}
	fprintf (stderr, "ofdm processor terminated\n");
}
