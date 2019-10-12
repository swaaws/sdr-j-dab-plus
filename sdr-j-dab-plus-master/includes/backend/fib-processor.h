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
#ifndef	FIB_PROCESSOR
#define	FIB_PROCESSOR
#
//
#include	<stdint.h>
#include	<stdio.h>
#include	<QObject>
#include	"msc-handler.h"

	struct dablabel {
	   uint8_t	label [17];
	   uint8_t	mask;
	   bool		hasName;
	};

	typedef struct dablabel	dabLabel;

	typedef struct subchannelmap channelMap;
//	from FIG1/2
	struct serviceid {
	   int32_t	serviceId;
	   dabLabel	serviceLabel;
	   bool		inUse;
	   bool		hasPNum;
	   bool		hasLanguage;
	   int16_t	language;
	   int16_t	programType;
	   uint16_t	pNum;
	   
	};
	typedef	struct serviceid serviceId;

	struct servicecomponents {
	   bool		inUse;
	   serviceId	*service;
	   int16_t	subchannelId;
	   int16_t	componentNr;
	   int16_t	flags;
	   int16_t	ASCTy;
	   int16_t	PS_flag;
	   int8_t	TMid;
	};

	typedef struct servicecomponents serviceComponents;

	struct subchannelmap {
	   int32_t	SubChId;
	   int32_t	StartAddr;
	   int32_t	Length;
	   int32_t	uepFlag;
	   int32_t	protLevel;
	   int32_t	BitRate;
	   int16_t	language;
	};

class	RadioInterface;

class	fib_processor: public QObject {
Q_OBJECT
public:
		fib_processor		(RadioInterface *,  mscHandler *);
		~fib_processor		(void);
	void	process_FIB		(uint8_t *, uint16_t);

	void	setupforNewFrame	(void);
	void	clearEnsemble		(void);
	void	printActions		(int16_t);
	void	setCIFparameters	(int16_t);
	void	setSelectedService	(char *);
	void	setunnamedService	(int);
private:
	RadioInterface	*myRadioInterface;
	mscHandler	*myDecoder;
	serviceId	*findServiceId (int32_t);
	channelMap	*findChannelMap (int16_t, int16_t);
	void		bindService (int8_t,
	                             int32_t, int16_t,
	                             int16_t, int16_t, int16_t);
	int32_t		selectedService;
	void		process_FIG0		(uint8_t *);
	void		process_FIG1		(uint8_t *);
	void		FIG0Extension0		(uint8_t *);
	void		FIG0Extension1		(uint8_t *);
	void		FIG0Extension2		(uint8_t *);
	void		FIG0Extension3		(uint8_t *);
	void		FIG0Extension5		(uint8_t *);
	void		FIG0Extension8		(uint8_t *);
	void		FIG0Extension9		(uint8_t *);
	void		FIG0Extension10		(uint8_t *);
	void		FIG0Extension13		(uint8_t *);
	void		FIG0Extension14		(uint8_t *);
	void		FIG0Extension16		(uint8_t *);
	void		FIG0Extension17		(uint8_t *);
	void		FIG0Extension18		(uint8_t *);
	void		FIG0Extension19		(uint8_t *);
	void		FIG0Extension21		(uint8_t *);
	void		FIG0Extension22		(uint8_t *);

	int16_t		HandleFIG0Extension1	(uint8_t *,
	                                         int16_t, uint8_t);
	int16_t		HandleFIG0Extension2	(uint8_t *,
	                                         int16_t, uint8_t, uint8_t);
	int16_t		HandleFIG0Extension3	(uint8_t *, int16_t);
	int16_t		HandleFIG0Extension5	(uint8_t *, int16_t);
	int16_t		HandleFIG0Extension8	(uint8_t *,
	                                         int16_t, uint8_t);
	int16_t		HandleFIG0Extension13	(uint8_t *,
	                                         int16_t, uint8_t);
	int16_t		HandleFIG0Extension22	(uint8_t *, int16_t);
	int32_t		dateTime	[8];
	channelMap	ficList [64];
	serviceComponents	components [64];
	serviceId	listofServices [64];
	bool	dateFlag;
signals:
	void		addEnsembleChar	(char, int);
	void		addtoEnsemble (const QString &);
	void		nameofEnsemble	(int, char *);
	void		technicalData	(int, int, int, int, int, int, int);
};

#endif

