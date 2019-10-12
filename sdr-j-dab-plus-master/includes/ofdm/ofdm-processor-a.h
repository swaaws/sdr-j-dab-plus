#
/*
 *
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
 */
#
#ifndef	__OFDM_DECODER_A
#define	__OFDM_DECODER_A
/*
 *
 */
#include	"dab-constants.h"
#include	"stdint.h"
#include	"phasereference.h"
#include	"ofdm-decoder.h"
#include	"virtual_input.h"
#include	<QThread>
#include	<QObject>
#include	"ringbuffer.h"
#include	"ofdm-processor.h"

class	RadioInterface;
class	common_fft;

class ofdm_processor_a: public ofdm_processor {
Q_OBJECT
public:
		ofdm_processor_a	(virtualInput *,
	                                 DabParams	*,
	                                 RadioInterface *,
	                                 mscHandler *,
	                                 ficHandler *,
	                                 int16_t,		// fillpoint
	                                 RingBuffer<DSPCOMPLEX> *,
	                                 RingBuffer<DSPCOMPLEX> *,	// passing
	                                 bool);
		~ofdm_processor_a	(void);
private:
	int32_t		sampleCnt;
	int32_t		inputSize;
	int32_t		inputPointer;
	void		getNextSampleReady	(void);
	DSPCOMPLEX	currentSample;
virtual	void		run		(void);
};
#endif

