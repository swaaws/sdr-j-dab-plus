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
 *	Once the bits are "in", interpretation and manipulation
 *	should reconstruct the data blocks.
 *	Ofdm_decoder is called once every Ts samples, and
 *	its invocation results in 2 * Tu bits
 */
#include	"ofdm-decoder.h"
#include	"gui.h"
#include	"phasetable.h"

	ofdm_decoder::ofdm_decoder	(DabParams	*p,
	                                 RingBuffer<DSPCOMPLEX> *iqBuffer,
	                                 RadioInterface *mr) {
	this	-> params		= p;
	this	-> T_s			= params	-> T_s;
	this	-> T_u			= params	-> T_u;
	this	-> carriers		= params	-> K;
	this	-> iqBuffer		= iqBuffer;
	this	-> myRadioInterface	= mr;

	connect (this, SIGNAL (showIQ (int)),
	         mr, SLOT (showIQ (int)));
	this	-> delta	= T_s - T_u;
	fft_handler		= new common_fft (T_u);
	fft_buffer		= fft_handler -> getVector ();
	phaseReference		= new DSPCOMPLEX [T_u];
	myMapper		= new permVector (params);
	iqCount			= 0;
	coarseOffset		= 0;
	displayToken		= 2;
//
	connect (this, SIGNAL (show_snr (int)),
	         mr, SLOT (show_snr (int)));
	snrCount		= 0;
	snr			= 0;	
	strength		= 0;
}

	ofdm_decoder::~ofdm_decoder	(void) {
	delete	fft_handler;
	delete	phaseReference;
	delete	myMapper;
}
//	in practive, we use the "incoming" block
//	and use its data to generate the prs
void	ofdm_decoder::processBlock_0 (DSPCOMPLEX *vi) {
DSPCOMPLEX	*v = (DSPCOMPLEX *)alloca (T_u * sizeof (DSPCOMPLEX));

	memcpy (fft_buffer, vi, T_u * sizeof (DSPCOMPLEX));
	fft_handler	-> do_FFT ();
//	The + and - frequencies are still to be switched, so:
	memcpy (v, &fft_buffer [T_u / 2], T_u / 2 * sizeof (DSPCOMPLEX));
	memcpy (&v [T_u / 2], fft_buffer, T_u / 2 * sizeof (DSPCOMPLEX));
	coarseOffset 	= getMiddle	(v);
	snr		= 0.7 * snr + 0.3 * get_snr (v);
	strength	= 0.7 * strength + 0.3 * newStrength (v);
	if (++snrCount > 10) {
	   show_snr (snr);
	   snrCount = 0;
	}
//	we are now in the frequency domain, and we keep the carriers
//	from -Tu / 2 .. Tu / 2 \{0} in the phaseReference 
	memcpy (phaseReference, v, T_u * sizeof (DSPCOMPLEX));
//
}

//	for the other blocks of data, the first step is to go from
//	time to frequency domain, to get the carriers.
//	This requires an FFT and zero de-padding and rearrangement
void	ofdm_decoder::processToken (DSPCOMPLEX *inv,
	                            int16_t *ibits, int32_t blkno) {
int16_t		i;
static		int cnt	= 0;
DSPCOMPLEX	*v = (DSPCOMPLEX *)alloca (T_u * sizeof (DSPCOMPLEX));

	memcpy (fft_buffer, &inv [delta], T_u * sizeof (DSPCOMPLEX));
	fft_handler -> do_FFT ();
//	The + and - frequencies are still to be switched, so:
	memcpy (v, &fft_buffer [T_u / 2], T_u / 2 * sizeof (DSPCOMPLEX));
	memcpy (&v [T_u / 2], fft_buffer, T_u / 2 * sizeof (DSPCOMPLEX));
//
//	ok, now we have our K (-K / 2 .. K / 2) channels back, embedded
//	in -T_u / 2 .. T_u / 2 carriers
//	Differential demodulation means multiplying the
//	incoming symbol with the conjunct of the previous (incoming) symbol
//
//	Logically, differential demodulation is a phase
//	preceding the mapping onto bits as follows
//	for (i = T_u / 2 - carriers / 2 ; i <= T_u / 2 + carriers / 2; i ++) {
//	   DSPCOMPLEX x1 = v [i];	// new "previous" symbol
//	   v [i] = v [i] * conj (phaseReference [i]);
//	   phaseReference [i]		= x1;
//	}
//
//	Conversion into bits is simple: the two components of the complex
//	each denote a bit. NOTE: x > 0 means 0, x < 0 means 1.
//	Note that myMapper is a function from 0 .. 1535 -> -768 .. 768 \{0}
//	and we are taking the carriers from the shifted fft buffer
//	for (i = 0; i < carriers; i ++) {
//	   DSPCOMPLEX x1 = v [myMapper -> mapIn (i) + T_u / 2];	
//	   DSPFLOAT ab1 = abs (x1);
//	   ibits [i] = real (x1) / ab1 * 256;	// positive = 0, negative = 1
//	   ibits [carriers + i] = imag (x1) / ab1 * 256;
//	}
//
//	These two loops can be combined into a single one
//
	for (i = 0; i < carriers; i ++) {
	   int16_t	index	= myMapper -> mapIn (i) + T_u / 2;
	   DSPCOMPLEX	x = v [index];
	   v [index] = v [index] * conj (phaseReference [index]);
	   phaseReference [index] = x;
	   DSPFLOAT ab1	= abs (v [index]);
//	Recall:  positive = 0, negative = 1
	   ibits [i]		= (real (v [index])) / ab1 * 255.0;
	   ibits [carriers + i] = (imag (v [index])) / ab1 * 255.0;
	}
//
//	and, from time to time we show some clouds.
//	Since we are in a different thread, directly calling of the
//	iqBuffer is not done. We use an intermediate buffer
	if (blkno == displayToken) {
	   if (++cnt > 7) {
	      iqBuffer	-> putDataIntoBuffer (&v [T_u / 2 - carriers / 2],
	                                      carriers);
	      showIQ	(carriers);
	      cnt = 0;
	   }
	}
}

int16_t	ofdm_decoder::coarseCorrector (void) {
	return coarseOffset;
}

int16_t	ofdm_decoder::getMiddle (DSPCOMPLEX *v) {
int16_t		i;
DSPFLOAT	sum = 0;
int16_t		maxIndex = 0;
DSPFLOAT	oldMax	= 0;
//
//	basic sum over K carriers that are - most likely -
//	in the range
//	The range in which the carrier should be is
//	T_u / 2 - K / 2 .. T_u / 2 + K / 2
//	We first determine an initial sum
	for (i = 10; i < carriers + 10; i ++)
	   sum += abs (v [i]);
//
//	Now a moving sum, look for a maximum within a reasonable
//	range (around (T_u - K) / 2, the start of the useful frequencies)
	for (i = 10; i < T_u - carriers - 10; i ++) {
	   sum -= abs (v [i]);
	   sum += abs (v [i + carriers]);
	   if (sum > oldMax) {
	      sum = oldMax;
	      maxIndex = i;
	   }
	}
	return maxIndex - (T_u - carriers) / 2;
}
//
//
//	for the snr we have a full T_u wide vector, with in the middle
//	K carriers
int16_t	ofdm_decoder::get_snr (DSPCOMPLEX *v) {
int16_t	i;
DSPFLOAT	noise 	= 0;
DSPFLOAT	signal	= 0;
int16_t	low	= T_u / 2 -  carriers / 2;
int16_t	high	= low + carriers;

	for (i = 10; i < low - 20; i ++)
	   noise += abs (v [i]);

	for (i = high + 20; i < T_u - 10; i ++)
	   noise += abs (v [i]);

	noise	/= (low - 30 + T_u - high - 30);
	for (i = T_u / 2 - carriers / 4;  i < T_u / 2 + carriers / 4; i ++)
	   signal += abs (v [i]);

	return get_db (signal / (carriers / 2)) - get_db (noise);
}

int16_t	ofdm_decoder::newStrength (DSPCOMPLEX *v) {
int16_t	i;
DSPFLOAT	signal	= 0;

	for (i = T_u / 2 - carriers / 4; i < T_u / 2 + carriers / 4; i ++)
	   signal += abs (v [i]);
	return get_db (signal / (carriers / 2));
}

int16_t	ofdm_decoder::getStrength (void) {
	return strength;
}

