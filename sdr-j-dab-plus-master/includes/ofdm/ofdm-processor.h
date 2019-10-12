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
 */
#
#ifndef	__OFDM_PROCESSOR
#define	__OFDM_PROCESSOR

#include	<QThread>
#include	<QObject>
#include	<sndfile.h>
#include	"dab-constants.h"
#include	"stdint.h"
#include	"phasereference.h"
#include	"ofdm-decoder.h"
#include	"fic-handler.h"
#include	"msc-handler.h"
#include	"virtual-input.h"
#include	"ringbuffer.h"

class	RadioInterface;
class	common_fft;

#define	BUFFER_SIZE	20480
class ofdm_processor: public QThread {
Q_OBJECT
public:
		ofdm_processor	(virtualInput *,
	                         DabParams	*,
	                         RadioInterface *,
	                         mscHandler *,
	                         ficHandler *,
	                         int32_t,		// fillpoint
	                         RingBuffer<DSPCOMPLEX> *,
	                         RingBuffer<DSPCOMPLEX> *,	// passing
	                         bool);
virtual		~ofdm_processor	(void);
	void	stop		(void);
	void	setOffset	(int32_t);
	void	setGain		(int16_t);
	void	coarseCorrectorOn	(void);
	void	coarseCorrectorOff	(void);
	void	startDumping	(SNDFILE *);
	void	stopDumping	(void);
	void	showSpectrum	(int16_t);
virtual	void	reset		(void);
protected:
	bool		running;
	int16_t		gain;
	bool		dumping;
	SNDFILE		*dumpFile;
	virtualInput	*theRig;
	RingBuffer<DSPCOMPLEX> * scopeBuffer;
	DabParams	*params;
	int32_t		T_null;
	int32_t		T_u;
	int32_t		T_s;
	int32_t		Missers;
	int32_t		Hitters;
	RadioInterface	*myRadioInterface;
	bool		lightRun;
	DSPCOMPLEX	*dataBuffer;
	int32_t		FreqOffset;
	DSPCOMPLEX	*oscillatorTable;
	int32_t		localPhase;
	int16_t		fineCorrector;
	int32_t		coarseCorrector;
	float		f2Corrector;
volatile
	bool		f2Correction;
	int32_t		frequencyCorrector;
	DSPCOMPLEX	*syncBuffer;
	DSPFLOAT	*envBuffer;
	uint32_t	syncBufferSize;
	uint32_t	syncBufferMask;
	uint32_t	syncBufferIndex;
	DSPFLOAT	syncFilterSum;
	DSPFLOAT	signalLevel;
	DSPFLOAT	nullLevel;
	DSPFLOAT	currLevel;
	uint32_t	initCount;
	int32_t		tokenLength;
	int32_t		tokenCount;
	int32_t		avgTokenLength;
	int16_t		*ibits;
	DSPCOMPLEX	*ofdmBuffer;
	uint32_t	ofdmBufferIndex;
	uint32_t	ofdmSymbolCount;
	phaseReference	*phaseSynchronizer;
	ofdm_decoder	*ofdmDecoder;
	DSPFLOAT	FreqCorr;
	DSPFLOAT	avgCorr;
	ficHandler	*my_ficHandler;
	mscHandler	*my_mscHandler;
	int16_t		fillSize;
signals:
	void		show_fineCorrector	(int);
	void		show_coarseCorrector	(int);
	void		show_avgTokenLength	(int);
	void		showScope		(int);
	void		setSynced		(char);
};
#endif

