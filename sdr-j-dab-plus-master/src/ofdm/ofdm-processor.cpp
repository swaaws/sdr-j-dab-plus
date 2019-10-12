#
/*
 *    Copyright (C) 2013
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
 *
 * 	the virtual ofdm processor
 * 	It basically provides the control mechanisms,
 * 	the processor itself, the thread, is defined in its
 * 	implementing classes
 */
#include	"ofdm-processor.h"
#include	"gui.h"
//
	ofdm_processor::ofdm_processor (virtualInput	*theRig,
	                                DabParams	*p,
	                                RadioInterface	*mr,
	                                mscHandler 	*msc,
	                                ficHandler 	*fic,
	                                int32_t		fillSize,
	                                RingBuffer<DSPCOMPLEX> *scopeBuffer,
	                                RingBuffer<DSPCOMPLEX> *iqBuffer,
	                                bool	   lightRun) {
uint32_t	i;
	this	-> theRig		= theRig;
	params				= p;
	this	-> T_null		= p	-> T_null;
	this	-> T_s			= p 	-> T_s;
	this	-> T_u			= p	-> T_u;
	this	-> myRadioInterface	= mr;
	this	-> my_mscHandler	= msc;
	this	-> my_ficHandler	= fic;
	this	-> fillSize		= fillSize;
	this	-> scopeBuffer		= scopeBuffer;
	this	-> lightRun		= lightRun;
	dumping				= false;
	Missers				= 0;
	Hitters				= 0;
//
//	syncBufferSize should be somewhat larger than 10 * the
//	Null symbol length (and it should be a power of two)
//	T_null = 2656 samples for Mode I, so 32768 is sufficiently large
	syncBufferMask			= 32768 - 1;
	syncBufferSize			= syncBufferMask + 1;
	syncBuffer			= new DSPCOMPLEX [syncBufferSize];
	envBuffer			= new DSPFLOAT [syncBufferSize];
	memset (syncBuffer, 0, syncBufferSize * sizeof (DSPCOMPLEX));

	syncBufferIndex			= 0;
	syncFilterSum			= 0;
	signalLevel			= 0;
	nullLevel			= 0;
	initCount			= 0;
	ofdmBuffer			= new DSPCOMPLEX [76 * T_s];
	ofdmBufferIndex			= 0;
	ibits				= new int16_t [2 * params -> K];
	ofdmSymbolCount			= 0;
	FreqCorr			= 0;
	tokenCount			= 0;
	tokenLength			= 0;
	avgCorr				= 0;
	avgTokenLength			= params -> T_F;
	FreqOffset			= 0;
	phaseSynchronizer	= new phaseReference (syncBuffer,
	                                              syncBufferMask, params);
	ofdmDecoder		= new ofdm_decoder (params,
	                                            iqBuffer,
	                                            myRadioInterface);
	fineCorrector		= 0;	
	coarseCorrector		= 0;
	f2Corrector		= 0;
	f2Correction		= false;
	frequencyCorrector	= 0;
	oscillatorTable		= new DSPCOMPLEX [INPUT_RATE];
	localPhase		= 0;

	dataBuffer		= new DSPCOMPLEX [BUFFER_SIZE];
	for (i = 0; i < INPUT_RATE; i ++)
	   oscillatorTable [i] = DSPCOMPLEX (cos (2.0 * M_PI * i / INPUT_RATE),
	                                     sin (2.0 * M_PI * i / INPUT_RATE));

	connect (this, SIGNAL (show_fineCorrector (int)),
	         myRadioInterface, SLOT (set_fineCorrectorDisplay (int)));
	connect (this, SIGNAL (show_coarseCorrector (int)),
	         myRadioInterface, SLOT (set_coarseCorrectorDisplay (int)));
	connect (this, SIGNAL (show_avgTokenLength (int)),
	         myRadioInterface, SLOT (set_avgTokenLengthDisplay (int)));
	connect (this, SIGNAL (showScope (int)),
	         myRadioInterface, SLOT (showScope (int)));
	connect (this, SIGNAL (setSynced (char)),
	         myRadioInterface, SLOT (setSynced (char)));

	running		= true;
	gain		= 30;
}

	ofdm_processor::~ofdm_processor	(void) {
	running	= false;	// this will cause an exception to be raised
	delete	syncBuffer;
	delete	ofdmBuffer;
	delete	phaseSynchronizer;
	delete	ofdmDecoder;
	delete	oscillatorTable;
	delete	dataBuffer;
	disconnect (this, SIGNAL (show_fineCorrector (int)),
	            myRadioInterface, SLOT (set_fineCorrectorDisplay (int)));
	disconnect (this, SIGNAL (show_coarseCorrector (int)),
	            myRadioInterface, SLOT (set_coarseCorrectorDisplay (int)));
	disconnect (this, SIGNAL (show_avgTokenLength (int)),
	            myRadioInterface, SLOT (set_avgTokenLengthDisplay (int)));
}

void	ofdm_processor::stop	(void) {
	running	= false;
}

void	ofdm_processor::setOffset	(int32_t k) {
	FreqOffset		= k;
}

void	ofdm_processor::startDumping	(SNDFILE *f) {
	if (dumping)
	   return;
//	do not change the order here.
	dumpFile 	= f;
	dumping		= true;
}

void	ofdm_processor::stopDumping	(void) {
	dumping = false;
}
//
//	amount is in pairs
void	ofdm_processor::setGain	(int16_t gain) {
	this	-> gain	= gain;
}


void	ofdm_processor::coarseCorrectorOn (void) {
	f2Correction 	= true;
	f2Corrector	= 0;
}

void	ofdm_processor::coarseCorrectorOff (void) {
	f2Correction	= false;
}
//
//	it is assumed that spectrumSize < syncBufferSize
//	The "last" amount samples are stored in a buffer
//	that is shared with the gui thread, and at the end a signal
//	is given
void	ofdm_processor::showSpectrum	(int16_t amount) {
int16_t i;

DSPCOMPLEX	*v = (DSPCOMPLEX *)alloca (amount * sizeof (DSPCOMPLEX));
	for (i = 0; i < amount; i++) 
	   v [i] = 
	         syncBuffer [(syncBufferIndex - amount + i) & syncBufferMask];

	scopeBuffer	-> putDataIntoBuffer (v, amount);
	showScope (amount);
}

void	ofdm_processor::reset	(void) {
}

