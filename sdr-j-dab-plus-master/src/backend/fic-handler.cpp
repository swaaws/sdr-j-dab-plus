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
 * 	An ofdm block generator for FIC blocks
 * 	We get in - through get_ficBlock - the FIC data
 * 	in units of 768 bits.
 * 	We follow the standard and apply conv coding and
 * 	puncturing.
 *	The data is sent through to the fic processor
 */

#include	"fic-handler.h"
#include	"gui.h"
#include	"msc-handler.h"
#include	"fib-processor.h"
#include	"protTables.h"
//
//	The 3072 bits of the serial motherword shall be split into
//	24 blocks of 128 bits each.
//	The first 21 blocks shall be subjected to
//	puncturing (per 32 bits) according to PI_16
//	The next three blocks shall be subjected to 
//	puncturing (per 32 bits) according to PI_15
//	The last 24 bits shall be subjected to puncturing
//	according to the table X
static
uint8_t PI_X [24] = {
	1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
	1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0
};

		ficHandler::ficHandler (RadioInterface *mr,
	                                mscHandler *f):viterbi (768) {
int16_t	i, j;
	bitBuffer_out	= new uint8_t [768];
	ofdm_input 	= new int16_t [2304];
	index		= 0;
	BitsperBlock	= 2 * 1536;
	ficno		= 0;
	ficBlocks	= 0;
	ficMissed	= 0;
	ficRatio	= 0;
	convState	= 0;
	PI_15		= get_PCodes (15 - 1);
	PI_16		= get_PCodes (16 - 1);
	fibProcessor	= new fib_processor	(mr, f);
	memset (shiftRegister, 1, 9);

	for (i = 0; i < 768; i ++) {
	   PRBS [i] = shiftRegister [8] ^ shiftRegister [4];
	   for (j = 8; j > 0; j --)
	      shiftRegister [j] = shiftRegister [j - 1];

	   shiftRegister [0] = PRBS [i];
	}

	connect (this, SIGNAL (show_ficRatio (int)),
	         mr, SLOT (show_ficRatio (int)));
}

		ficHandler::~ficHandler (void) {
	        delete	bitBuffer_out;
	        delete	ofdm_input;
	        delete	fibProcessor;
}

	
//
//	The number of bits to be processed per incoming block
//	is 2 * p -> K, which still depends on the Mode.
//	for Mode I it is 2 * 1536, for Mode II, it is 2 * 384,
//	for Mode III it is 192, Mode IV gives 2 * 768.
//	for Mode II we will get the 2304 bits after having read
//	the 3 FIC blocks,
//	for Mode IV we will get 3 * 2 * 768 = 4608, i.e. two resulting blocks
//	Note that Mode III is NOT supported
void	ficHandler::setBitsperBlock	(int16_t b) {
	if ((b == 2 * 384) ||
	    (b == 2 * 768) ||
	    (b == 2 * 1536))
	BitsperBlock	= b;
	index		= 0;
	ficno		= 0;
}

void	ficHandler::process_ficBlock (int16_t *data,
	                              int16_t blkno) {
int32_t	i;

	if (blkno == 2) {
	   index = 0;
	   ficno = 0;
	}
//
//	The number of bits to be processed per incoming block
//	is 2 * p -> K, which still depends on the Mode.
//	for Mode I it is 2 * 1536, for Mode II, it is 2 * 384,
//	for Mode III it is 192, Mode IV gives 2 * 768.
//	for Mode II we will get the 2304 bits after having read
//	the 3 FIC blocks, each with 768 bits.
//	for Mode IV we will get 3 * 2 * 768 = 4608, i.e. two resulting blocks
//	Note that Mode III is NOT supported
	if ((2 <= blkno) && (blkno <= 4)) {
	   for (i = 0; i < BitsperBlock; i ++) {
	      ofdm_input [index ++] = data [i];
	      if (index >= 2304) {
	         process_ficInput (ofdm_input, ficno);
	         index = 0;
	         ficno ++;
	      }
	   }
	}
	else
	   fprintf (stderr, "You should not call ficBlock here\n");
//	we are pretty sure now that after block 4, we end up
//	with index = 0
}
//
//	we have a vector of 2304 (0 .. 2303) soft bits that has
//	to be de-punctured and de-conv-ed into a block of 768 bits
//	In this approach we first create the full 3072 block (i.e.
//	depuncturing, and thenwe apply the deconvolution
//	In the next coding step, we will combine this function with the
//	one above
void	ficHandler::process_ficInput (int16_t *ficblock,
	                              int16_t ficno) {
int16_t	input_counter	= 0;
int16_t	i, k;
int32_t	local		= 0;
int16_t	viterbiBlock [3072 + 24];

//	a block of 2304 bits is considered to be a codeword
//	a. First we have 21 blocks with punctured according to PI_16
//	each 128 bit block contains 4 subblocks of 32 bits
//	on which the given puncturing is applied
	for (i = 0; i < 21; i ++) {
	   for (k = 0; k < 32 * 4; k ++) {
	      if (PI_16 [k % 32] == 1)  
	         viterbiBlock [local ++] = ficblock [input_counter ++];
	      else
	         viterbiBlock [local ++] = 0;	// a real "do not know"
	   }
	}
//	b Second, we have 3 blocks with puncturing according to PI_15
//	each 128 bit block contains 4 subblocks of 32 bits
//	on which the given puncturing is applied
	for (i = 0; i < 3; i ++) {
	   for (k = 0; k < 32 * 4; k ++) {
	      if (PI_15 [k % 32] == 1)  
	         viterbiBlock [local ++] = ficblock [input_counter ++];
	      else
	         viterbiBlock [local ++] = 0;	// a real "do not know"
	      }
	}

//	we had a final block of 24 bits  with puncturing according to PI_X
//	This block constitues the 6 * 4 bits of the register itself.
	for (k = 0; k < 24; k ++) {
	   if (PI_X [k] == 1) {
	      viterbiBlock [local ++] = ficblock [input_counter ++];
	   }
	   else
	      viterbiBlock [local ++] = 0;
	}
//
//	It is all in, apply:
	deconvolve (viterbiBlock, bitBuffer_out);
//
//	if everything worked as planned, we now have a
//	768 bit vector containing three FIB's
//	first step: prbs handling
	for (i = 0; i < 768; i ++)
	   bitBuffer_out [i] ^= PRBS [i];

	for (i = ficno * 3; i < (ficno + 1) * 3; i ++) {
	   if (++ficBlocks >= 100) {
	      ficRatio = 100 - ficMissed;
	      show_ficRatio (100 - ficMissed);
	      ficBlocks = ficMissed = 0;
	   }
	   uint8_t *p = &bitBuffer_out [(i % 3) * 256];
	   if (!check_ficCRC (p, 256)) {
	      ficMissed ++;
	      continue;
	   }
	   fibProcessor	-> process_FIB (p, ficno);
	}
//	fibProcessor	-> printActions (ficno);
}

void	ficHandler::setSelectedService (char *s) {
	fibProcessor -> setSelectedService (s);
}

void	ficHandler::unnamedService (int n) {
	fibProcessor -> setunnamedService (n);
}

void	ficHandler::clearEnsemble (void) {
	fibProcessor	-> clearEnsemble ();
}

int16_t	ficHandler::get_ficRatio (void) {
	return ficRatio;
}

static
const uint8_t crcPolynome [] =
	{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0};	// MSB .. LSB

bool	ficHandler::check_ficCRC (uint8_t *in, int16_t size) {
int16_t	i, f;
uint8_t	b [16];
int16_t	Sum	= 0;

	memset (b, 1, 16);

	for (i = size - 16; i < size; i ++)
	   in [i] ^= 1;

	for (i = 0; i < size; i++) {
	   if ((b [0] ^ in [i]) == 1) {
	      for (f = 0; f < 15; f++) 
	         b [f] = crcPolynome [f] ^ b[f + 1];
	      b [15] = 1;
	   }
	   else {
	      memmove (&b [0], &b[1], sizeof (uint8_t ) * 15); // Shift
	      b [15] = 0;
	   }
	}

	for (i = 0; i < 16; i++)
	   Sum += b [i];

	return Sum == 0;
}

