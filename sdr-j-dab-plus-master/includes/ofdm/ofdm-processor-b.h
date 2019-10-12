#
#ifndef	__OFDM_DECODER_B
#define	__OFDM_DECODER_B
/*
 *	We made two implementations of the ofdmProcessor,
 *	one with an explicit "state", the other one with
 *	the state of the computation implicit
 *	This happens to be the explicit one
 */
#include	"ofdm-processor.h"

class	RadioInterface;
class	common_fft;

class ofdm_processor_b: public ofdm_processor {
Q_OBJECT
public:
		ofdm_processor_b	(virtualInput *,
	                                 DabParams	*,
	                                 RadioInterface *,
	                                 mscHandler *,
	                                 ficHandler *,
	                                 int16_t,		// fillpoint
	                                 RingBuffer<DSPCOMPLEX> *,
	                                 RingBuffer<DSPCOMPLEX> *, // passing
	                                 bool);
		~ofdm_processor_b	(void);
	void	reset			(void);
private:
virtual	void		run		(void);
	int32_t		syncPhaseIndex;
	uint32_t	syncTries;
	uint32_t	syncCounter;
	int8_t		decoderState;
	int32_t		lowCount;
	int32_t		highCount;
	uint8_t		syncMode;
	bool		resetRequest;
};
#endif

