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
 *
 */
#
#ifndef	MP4PROCESSOR
#define	MP4PROCESSOR
/*
 * 	Handling superframes for DAB+ and delivering
 * 	frames into the ffmpeg or faad decoding library
 */
//
#include	"dab-constants.h"
#include	<stdio.h>
#include	<stdint.h>
#include	"audiosink.h"
#include	"dab-processor.h"
#include	"firecode-checker.h"
#include	"rscodec.h"
#include	<QObject>

class	RadioInterface;
class	faadDecoder;

class	mp4Processor : public QObject, public dabProcessor {
Q_OBJECT
public:
			mp4Processor	(RadioInterface *,
	                                 audioSink *,
	                                 FILE *,
	                                 int16_t,
	                                 bool);
			~mp4Processor	(void);
	void		addtoFrame	(uint8_t *, int16_t);
	void		setFile		(FILE *);
private:
	bool		processSuperframe (uint8_t [], int16_t);

	RadioInterface	*myRadioInterface;
	audioSink	*ourSink;
	FILE		*mp4File;
	int16_t		superFramesize;
	int16_t		blockFillIndex;
	int16_t		blocksInBuffer;
	int16_t		blockCount;
	int16_t		bitRate;
	uint8_t		*frameBytes;
	uint8_t		**RSMatrix;
	int16_t		RSDims;
	int16_t		au_start	[10];
	int32_t		baudRate;

	int16_t		au_count;
	int16_t		au_errors;
	int16_t		errorRate;
	firecode_checker	fc;
	rscodec		rsDecoder;
	int16_t		aacFrameLength;
	uint8_t		*outVector;

	uint8_t		d_header	[10];

	struct adts_fixed_header {
	        unsigned int syncword           :12;
	        unsigned int id                 :1;
	        unsigned int layer              :2;
	        unsigned int protection_absent  :1;
	        unsigned int profile_objecttype :2;
	        unsigned int sampling_freq_idx  :4;
	        unsigned int private_bit        :1;
	        unsigned int channel_conf       :3;
	        unsigned int original_copy      :1;
	        unsigned int home               :1;
	    } d_fh;

	    struct adts_variable_header {
	        unsigned int copyright_id_bit       :1;
	        unsigned int copyright_id_start     :1;
	        unsigned int aac_frame_length       :13;
	        unsigned int adts_buffer_fullness   :11;
	        unsigned int no_raw_data_blocks     :2;
	    }d_vh;

//
//	and for the aac decoder
	faadDecoder	*aacDecoder;
	int16_t		frameCount;
	int16_t		frameErrors;
signals:
	void		show_successRate		(int);
};

#endif


