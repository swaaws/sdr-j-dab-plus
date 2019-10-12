#
/*
 *    Copyright (C) 2013, 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR).
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
 */
#include	"ofdm-processor-a.h"
#include	"gui.h"
//
	ofdm_processor_a::ofdm_processor_a	(virtualInput *theRig,
	                                 DabParams	*p,
	                                 RadioInterface *mr,
	                                 mscHandler 	*msc,
	                                 ficHandler 	*fic,
	                                 int16_t	fillSize,
	                                 RingBuffer<DSPCOMPLEX> *scopeBuffer,
	                                 RingBuffer<DSPCOMPLEX> *iqBuffer,
	                                 bool	   lightRun):
	ofdm_processor (theRig, p, mr, msc, fic,
	                fillSize, scopeBuffer, iqBuffer, lightRun) {
//
//	Most initialization is done in the parent
	inputSize	= 0;
	inputPointer	= inputSize + 1;
	start (QThread::TimeCriticalPriority);
}

	ofdm_processor_a::~ofdm_processor_a	(void) {
	running	= false;	// this will cause an exception to be raised
	                        //through the getNextSampleReady
	while (!isFinished ())
	   usleep (100);
}

//	In this implementation, we implement the "states"
//	of the state machine by using positions in the code
//	We make the "getNextSampleReady" therefore extremely smart
//	Note that in this setup, getNextSampleReady is a real CPU burner,
//	for 2048000 samples per second, it is called once per sample
//
void ofdm_processor_a::getNextSampleReady (void) {
DSPCOMPLEX 	sample;
DSPFLOAT	Z_Tnull;
DSPFLOAT	absSample;
//
	if (!running)
	   throw (20);
	if (inputPointer >= inputSize) {	// samples needed
	   int32_t x = theRig -> Samples ();
//
	   while (running && (x < BUFFER_SIZE)) {
	      usleep (100);
	      x = theRig -> Samples ();
	   }
	   if (!running)	
	      throw 20;
	   inputPointer = 0;
	   inputSize	= theRig -> getSamples (dataBuffer, BUFFER_SIZE);
	   if (dumping) {
	      int32_t i;
	      float dumpBuffer [2 * inputSize];
	      for (i = 0; i < inputSize; i ++) {
	         dumpBuffer [2 * i] = real (dataBuffer [i]);
	         dumpBuffer [2 * i + 1] = imag (dataBuffer [i]);
	      }
	      sf_writef_float (dumpFile, dumpBuffer, inputSize);
	   }
	}
//
//	OK, we have samples!!
	sample	= cmul (dataBuffer [inputPointer], gain);
	inputPointer ++;
//
//	first: adjust frequency. We need Hz accuracy
	localPhase	-=
	       FreqOffset + Khz (f2Corrector) + frequencyCorrector;
	if (localPhase < 0)
	   localPhase += INPUT_RATE;
	if (localPhase >= INPUT_RATE)
	   localPhase -= INPUT_RATE;
	sample		*= oscillatorTable [localPhase];
	absSample	= abs (sample);

	syncBuffer [syncBufferIndex] = sample;
	envBuffer  [syncBufferIndex] = absSample;
	Z_Tnull		= envBuffer [(syncBufferIndex - T_null) &
	                                              syncBufferMask];
	syncFilterSum	+= absSample - Z_Tnull;
//
//	for signal level we take the average
	signalLevel	= 1.0 / (10 * T_null) * absSample +
	                  (1.0 - 1.0 / (10 * T_null)) * signalLevel;
#define	N	7
	if (++ sampleCnt > INPUT_RATE / N) {
	   showSpectrum		(fillSize);
	   show_fineCorrector	(frequencyCorrector);
	   show_coarseCorrector (f2Corrector);
	   sampleCnt = 0;
	}

	syncBufferIndex = (syncBufferIndex + 1) & syncBufferMask;
	tokenLength ++;
	currentSample	= sample;
}
//
//	The main thread, reading sample and trying to identify
//	the ofdm frames
void	ofdm_processor_a::run	(void) {
int32_t		startIndex;
int32_t		i, j;
//	we keep a local index in the syncbuf in case
//	we want to synchronize
int32_t		indexInSyncBuf;
//
	try {
//	first, we initialize to get the buffers filled
	   for (i = 0; i < 10 * T_null; i ++) 
	      getNextSampleReady ();
notSynced:
	setSynced (false);
//	Then we look for a "dip" in the datastream
SyncOnNull:
	   while (syncFilterSum / T_null  > 0.50 * signalLevel ) 
	      getNextSampleReady ();

//	OK, we have a null level
	   nullLevel = syncFilterSum / T_null;

//	Now it is waiting for the end
SyncOnEndNULL:
	   while (syncFilterSum / T_null <=  nullLevel) {
	      nullLevel = syncFilterSum / T_null;
	      getNextSampleReady ();
	   }

SyncOnPhase:
//	syncBufferIndex should point to the assumed first sample of the
//	new frame. We read up to params -> T_u samples in the buffer
	indexInSyncBuf	= (syncBufferIndex - 1) & syncBufferMask;
	   for (i = 1; i < params -> T_u; i ++) 
	      getNextSampleReady ();
//
//	and then call upon the phase synchronizer to verify/identify
//	the real "first" sample of the new frame
	   startIndex = phaseSynchronizer ->
	                        findIndex (indexInSyncBuf, params -> T_u);
	   if (startIndex == -1) { // no sync, try again
	      goto notSynced;
	   }

//	Once here, we are synchronized, we need to copy the data we
//	used for synchronization for block 0
	   ofdmBufferIndex	= 0;
	   for (j = startIndex; j < params -> T_u; j ++)
	      ofdmBuffer [ofdmBufferIndex ++] =
	            syncBuffer [(indexInSyncBuf + j) & syncBufferMask];
//
//	just a sanity check
	if (((indexInSyncBuf + params -> T_u) & syncBufferMask) !=
	                                                 syncBufferIndex)
	   fprintf (stderr, "Illegal buffering scheme\n");

//	we can assert that indexInSyncBuf + params -> T_u == syncBufferIndex,
//	so all delayed tokens are consumed
//	We now have a reference for computing the tokenLength
//	
	   tokenLength = tokenLength - (params -> T_u - startIndex);
	   if (0.9 * params -> T_F <= tokenLength &&
	                tokenLength <= 1.1 * params -> T_F)
	      avgTokenLength = 0.8 * avgTokenLength + 0.2 * tokenLength;

	   if (++tokenCount > 10) {
	      tokenCount = 0;
	      show_avgTokenLength (avgTokenLength);
	   }
//	and for the next round
	   tokenLength		= params -> T_u - startIndex;
//	but we are also ready to  actually process block_0

OFDM_PRS:
	   setSynced (true);
	   for (i = ofdmBufferIndex; i < (int)T_u; i ++) {
	      getNextSampleReady ();
	      ofdmBuffer [i] = currentSample;
	   }
//
//	block0 will set the phase reference for further decoding
	   ofdmDecoder	-> processBlock_0 (ofdmBuffer);
//
//	after block 0, we will just read in the other (params -> L) blocks
OFDM_SYMBOLS:
//	The first ones are the FIC blocks
	   avgCorr			= 0;
	   for (ofdmSymbolCount = 2;
	        ofdmSymbolCount <= 4; ofdmSymbolCount ++) {
	      FreqCorr	= 0;
	      for (i = 0; i < (int)T_u; i ++) {
	         getNextSampleReady ();
	         ofdmBuffer [i] = currentSample;
	      }
//	and for the frequency correction, the last T_s - T_u samples
	      for (i = (int)T_u; i < (int)T_s; i ++) {
	         getNextSampleReady ();
	         ofdmBuffer [i] = currentSample;
	         FreqCorr += arg (ofdmBuffer [i] *
	                             conj (ofdmBuffer [i - T_u]));
	      }
//	OK, at the end of a block
	      FreqCorr /= (2 * M_PI * (T_s - T_u));
	      avgCorr += FreqCorr;
	      ofdmDecoder -> processToken (ofdmBuffer, ibits, ofdmSymbolCount);
	      my_ficHandler -> process_ficBlock (ibits,
	                                         ofdmSymbolCount);
	   }
//
//	and similar for the (params -> L - 4) MSC blocks
	   for (ofdmSymbolCount = 5;
	        ofdmSymbolCount <= params -> L; ofdmSymbolCount ++) {
	      FreqCorr	= 0;
	      for (i = 0; i < (int)T_u; i ++) {
	         getNextSampleReady ();
	         ofdmBuffer [i] = currentSample;
	      }
	      for (i = (int32_t)T_u; i < (int32_t)T_s; i ++) {
	         getNextSampleReady ();
	         ofdmBuffer [i] = currentSample;
	         FreqCorr += arg (ofdmBuffer [i] *
	                             conj (ofdmBuffer [i - T_u]));
	      }
//	OK, at the end of a block
	      FreqCorr /= (2 * M_PI * (T_s - T_u));
	      avgCorr += FreqCorr;
	      ofdmDecoder -> processToken (ofdmBuffer, ibits, ofdmSymbolCount);
	      if (!lightRun)
	         my_mscHandler -> process_mscBlock (ibits,
	                                            ofdmSymbolCount);
	   }
//
//	Frequency correction:
	   fineCorrector += (avgCorr / params -> L) * params -> carrierDiff;
	   if (fineCorrector > params -> carrierDiff / 2)
	      fineCorrector -= params -> carrierDiff / 2;
	   if (fineCorrector < -params -> carrierDiff / 2)
	      fineCorrector += params -> carrierDiff / 2;
	   frequencyCorrector = 0.9 * frequencyCorrector +
	                        0.1 * fineCorrector;
	   if (f2Correction) {
	      float correction = ofdmDecoder -> coarseCorrector ();
	      f2Corrector	+= correction * (params -> carrierDiff / 1000);
	   }
//
//	and off we go, up to the next frame
	   goto SyncOnNull;
	}
	catch (int e) {
	   fprintf (stderr, "ofdmProcessor will halt\n");
	}
}

