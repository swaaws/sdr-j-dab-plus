#
//
//	For the different formats for input, we have
//	different readers, with one "mother" reader.
//	Note that the cardreader is quite different
//	and coded elsewhere
//
#include	"common-readers.h"
//
//	The reader for 16 bit int values
//
	reader_16::reader_16 (RingBuffer<DSPCOMPLEX> *p,
	                      int32_t base_16):virtualReader (p) {
	this	-> base = base_16;
}

	reader_16::~reader_16 (void) {
}
//
//	apparently bytes are read in from low byte to high byte
void	reader_16::processData	(float IQoffs, void *data, int cnt) {
int32_t	i;
DSPCOMPLEX temp [blockSize];
uint8_t	*p	= (uint8_t *)data;

	(void)IQoffs;
	for (i = 0; i < blockSize; i ++) {
	   uint8_t r0	= p [4 * i];
	   uint8_t r1	= p [4 * i + 1];
	   uint8_t i0	= p [4 * i + 2];
	   uint8_t i1	= p [4 * i + 3];
	   int16_t re	= (r1 << 8) | r0;
	   int16_t im	= (i1 << 8) | i0;
	   temp [i] = DSPCOMPLEX (float (re) / float (base),
	                          float (im) / float (base));
	}
	theBuffer	-> putDataIntoBuffer (temp, blockSize);
}

int16_t reader_16::bitDepth	(void) {
	return 16;
}
//
//	The reader for 24 bit integer values
//
	reader_24::reader_24 (RingBuffer<DSPCOMPLEX> *p, int32_t base_24):
	                                       virtualReader (p) {
	this	-> base	= base_24;
}

	reader_24::~reader_24 (void) {
}

void	reader_24::processData	(float IQoffs, void *data, int cnt) {
int32_t	i;
DSPCOMPLEX temp [blockSize];
uint8_t	*p	= (uint8_t *)data;
	(void)IQoffs;
	for (i = 0; i < blockSize; i ++) {
	   uint8_t r0	= p [6 * i];
	   uint8_t r1	= p [6 * i + 1];
	   uint8_t r2	= p [6 * i + 2];
	   uint8_t i0	= p [6 * i + 3];
	   uint8_t i1	= p [6 * i + 4];
	   uint8_t i2	= p [6 * i + 5];
	   int32_t re	= int32_t (uint32_t (r2 << 16 | r1 << 8 | r0));
	   int32_t im	= int32_t (uint32_t (i2 << 16 | i1 << 8 | i0));
	   temp [i] = DSPCOMPLEX (float (re) / float (base),
	                          float (im) / float (base));
	}
	theBuffer	-> putDataIntoBuffer (temp, blockSize);
}

int16_t reader_24::bitDepth	(void) {
	return 24;
}
//
//	The reader for 32 bit integer values
//
	reader_32::reader_32 (RingBuffer<DSPCOMPLEX> *p, int32_t base_32):
	                                         virtualReader (p) {
	this	-> base = base_32;
}

	reader_32::~reader_32 (void) {
}

void	reader_32::processData	(float IQoffs, void *data, int cnt) {
int32_t	i;
DSPCOMPLEX temp [blockSize];
uint8_t	*p	= (uint8_t *)data;
	(void)IQoffs;
	for (i = 0; i < blockSize; i ++) {
	   uint8_t r0	= p [8 * i];
	   uint8_t r1	= p [8 * i + 1];
	   uint8_t r2	= p [8 * i + 2];
	   uint8_t r3	= p [8 * i + 3];
	   uint8_t i0	= p [8 * i + 4];
	   uint8_t i1	= p [8 * i + 5];
	   uint8_t i2	= p [8 * i + 6];
	   uint8_t i3	= p [8 * i + 7];
	   int32_t re	= int32_t (uint32_t (r3 << 24 | r2 << 16 |
	                                             r1 << 8 | r0));
	   int32_t im	= int32_t (uint32_t (i3 << 24 | i2 << 16 |
	                                             i1 << 8 | i0));
	   temp [i] = DSPCOMPLEX (float (re) / float (base),
	                          float (im) / float (base));
	}
	theBuffer	-> putDataIntoBuffer (temp, blockSize);
}

int16_t	reader_32::bitDepth	(void) {
	return 32;
}
//
//	The reader for 32 bit float values
//
	reader_float::reader_float (RingBuffer<DSPCOMPLEX> *p):
	                                             virtualReader (p) {
int16_t	i;
//	The sizes of the mapTable and the convTable are
//	predefined and follow from the input and output rate
//	(3072000 / 1000) vs (2048000 / 1000)
	for (i = 0; i < 2048; i ++) {
	   mapTable_int [i] =  int (floor (i * (3072.0 / 2048.0)));
	   mapTable_float [i] = i * (3072.0 / 2048.0) - mapTable_int [i];
	}
	convIndex		= 0;
}

	reader_float::~reader_float (void) {
}
//
//	While reading the data in, we convert from 3072 to 2048,
//	so we do this 1000 times a second.
void	reader_float::processData	(float IQoffs, void *data, int cnt) {
int32_t	i, j;
DSPCOMPLEX temp [2048];
float	*p	= (float *)data;
	(void)IQoffs;
	(void)cnt;

	for (i = 0; i < blockSize; i ++) {
	   convBuffer [convIndex ++] = DSPCOMPLEX (p [2 * i],
	                                           p [2 * i + 1]);
	   if (convIndex > 3072) {
	      for (j = 0; j < 2048; j ++) {
	      int16_t  inpBase	= mapTable_int [j];
	      float    inpRatio	= mapTable_float [j];
	         temp [j]	= inpRatio * convBuffer [inpBase + 1] + 
	                          (1 - inpRatio) * convBuffer [inpBase];
	      }
	      theBuffer	-> putDataIntoBuffer (temp, 512);
//
//	shift the sample at the end to the beginning, it is needed
//	as the starting sample for the next time
	      convBuffer [0] = convBuffer [3072];
	      convIndex = 1;
	   }
	}
}

int16_t reader_float::bitDepth	(void) {
	return 16;
}

