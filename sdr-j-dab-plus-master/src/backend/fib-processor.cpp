#
/*
 *    Copyright (C) 2014
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
 * 	fib and fig processor
 */
#include	"fib-processor.h"
#include	<cstring>
#include	"gui.h"
#include	"charsets.h"
//
//
// Tabelle ETSI EN 300 401 Page 50
// Table is copied from the work of Michael Hoehn
   const int ProtLevel[64][3]   = {{16,5,32},	// Index 0
                                   {21,4,32},
                                   {24,3,32},
                                   {29,2,32},
                                   {35,1,32},	// Index 4
                                   {24,5,48},
                                   {29,4,48},
                                   {35,3,48},
                                   {42,2,48},
                                   {52,1,48},	// Index 9
                                   {29,5,56},
                                   {35,4,56},
                                   {42,3,56},
                                   {52,2,56},
                                   {32,5,64},	// Index 14
                                   {42,4,64},
                                   {48,3,64},
                                   {58,2,64},
                                   {70,1,64},
                                   {40,5,80},	// Index 19
                                   {52,4,80},
                                   {58,3,80},
                                   {70,2,80},
                                   {84,1,80},
                                   {48,5,96},	// Index 24
                                   {58,4,96},
                                   {70,3,96},
                                   {84,2,96},
                                   {104,1,96},
                                   {58,5,112},	// Index 29
                                   {70,4,112},
                                   {84,3,112},
                                   {104,2,112},
                                   {64,5,128},
                                   {84,4,128},	// Index 34
                                   {96,3,128},
                                   {116,2,128},
                                   {140,1,128},
                                   {80,5,160},
                                   {104,4,160},	// Index 39
                                   {116,3,160},
                                   {140,2,160},
                                   {168,1,160},
                                   {96,5,192},
                                   {116,4,192},	// Index 44
                                   {140,3,192},
                                   {168,2,192},
                                   {208,1,192},
                                   {116,5,224},
                                   {140,4,224},	// Index 49
                                   {168,3,224},
                                   {208,2,224},
                                   {232,1,224},
                                   {128,5,256},
                                   {168,4,256},	// Index 54
                                   {192,3,256},
                                   {232,2,256},
                                   {280,1,256},
                                   {160,5,320},
                                   {208,4,320},	// index 59
                                   {280,2,320},
                                   {192,5,384},
                                   {280,3,384},
                                   {416,1,384}};

//
//	just some locals
static inline
uint16_t	getBits_8 (uint8_t *d, int16_t offset) {
int16_t	i;
uint16_t	res	= 0;

	for (i = 0; i < 8; i ++) {
	   res <<= 1;
	   res |= d [offset + i];
	}
	return res;
}

static inline
uint16_t	getBits (uint8_t *d, int16_t offset, int16_t size) {
int16_t	i;
uint16_t	res	= 0;

	for (i = 0; i < size; i ++) {
	   res <<= 1;
	   res |= d [offset + i];
	}
	return res;
}

static inline
uint16_t	getBit (uint8_t *d, int16_t offset) {
	return (d [offset] & 0x01);
}


static inline
uint32_t	getLBits	(uint8_t *d,
	                         int16_t offset, int16_t amount) {
uint32_t	res	= 0;
int16_t		i;

	for (i = 0; i < amount; i ++) {
	   res <<= 1;
	   res |= (d [offset + i] & 01);
	}
	return res;
}

	fib_processor::fib_processor (RadioInterface *mr,
	                              mscHandler *f) {
	memset (dateTime, 0, 8);
	dateFlag	= false;
	myRadioInterface	= mr;
	myDecoder		= f;
	selectedService		= -1;
	clearEnsemble	();
	connect (this, SIGNAL (addtoEnsemble (const QString &)),
	         myRadioInterface, SLOT (addtoEnsemble (const QString &)));
	connect (this, SIGNAL (nameofEnsemble (int, char *)),
	         myRadioInterface, SLOT (nameofEnsemble (int, char *)));
	connect (this, SIGNAL (addEnsembleChar (char, int)),
	         myRadioInterface, SLOT (addEnsembleChar (char, int)));
	connect (this, SIGNAL (technicalData (int, int, int, 
	                                      int, int, int, int)),
	         myRadioInterface, 
	         SLOT (channelData (int, int, int, int, int, int, int)));
//	connect (this, SIGNAL (setTime (int, int, int, int, int, int)),
//	         myRadioInterface,
//	         SLOT (showTime (int, int, int, int, int, int)));
}
	
	fib_processor::~fib_processor (void) {
}
//
//	FIB's are segments of 256 bits. When here, we already
//	passed the crc and we start unpacking into FIGs
//	This is merely a dispatcher
void	fib_processor::process_FIB (uint8_t *p, uint16_t fib) {
uint8_t	FIGtype;
int8_t	processedBytes	= 0;
uint8_t	*d		= p;

	   (void)fib;
	   while (processedBytes  < 30) {
	      FIGtype 		= getBits (d, 0, 3);
	      switch (FIGtype) {
	         case 0:
	            process_FIG0 (d);	
	            break;

	         case 1:
	            process_FIG1 (d);
	            break;

	         case 7:
	            return;

	         default:
//	            fprintf (stderr, "FIG%d aanwezig\n", FIGtype);
	            break;
	      }
//
//	Thanks to Ronny Kunze, who discovered that I used
//	a p rather than a d
	      processedBytes += getBits (d, 3, 5) + 1;
//	      processedBytes += getBits (p, 3, 5) + 1;
	      d = p + processedBytes * 8;
	   }
}
//
//	Handle ensemble is all through FIG0
//
void	fib_processor::process_FIG0 (uint8_t *d) {
uint8_t	Ext	= getBits (d, 8 + 3, 5);
//uint8_t	CN	= getBit (d, 8 + 0);

	switch (Ext) {
	   case 0:
	      FIG0Extension0 (d);
	      break;

	   case 1:
	      FIG0Extension1 (d);
	      break;

	   case 2:
	      FIG0Extension2 (d);
	      break;

	   case 3:
	      FIG0Extension3 (d);
	      break;

	   case 5:
	      FIG0Extension5 (d);
	      break;

	   case 8:
	      FIG0Extension8 (d);
	      break;

	   case 9:
	      FIG0Extension9 (d);
	      break;

	   case 10:
	      FIG0Extension10 (d);
	      break;

	   case 14:
	      FIG0Extension14 (d);
	      break;

	   case 13:
	      FIG0Extension13 (d);
	      break;

	   case 16:
	      FIG0Extension16 (d);
	      break;

	   case 17:
	      FIG0Extension17 (d);
	      break;

	   case 18:
	      FIG0Extension18 (d);
	      break;

	   case 19:
	      FIG0Extension19 (d);
	      break;

	   case 21:
	      FIG0Extension21 (d);
	      break;

	   case 22:
	      FIG0Extension22 (d);
	      break;

	   default:
	      break;
	}
}

void	fib_processor::FIG0Extension0 (uint8_t *d) {
uint16_t	EId;
uint8_t		changeflag;
uint16_t	highpart, lowpart;
int16_t		occurrenceChange;
uint8_t	CN	= getBit (d, 8 + 0);

	(void)CN;
	changeflag	= getBits (d, 16 + 16, 2);
	if (changeflag == 0)
	   return;

	EId			= getBits (d, 16, 16);
	(void)EId;
	highpart		= getBits (d, 16 + 19, 5) % 20;
	(void)highpart;
	lowpart			= getBits (d, 16 + 24, 8) % 250;
	(void)lowpart;
	occurrenceChange	= getBits (d, 16 + 32, 8);
	(void)occurrenceChange;

//	if (changeflag == 1) {
//	   fprintf (stderr, "Changes in sub channel organization\n");
//	   fprintf (stderr, "cifcount = %d\n", highpart * 250 + lowpart);
//	   fprintf (stderr, "Change happening in %d CIFs\n", occurrenceChange);
//	}
//	else if (changeflag == 3) {
//	   fprintf (stderr, "Changes in subchannel and service organization\n");
//	   fprintf (stderr, "cifcount = %d\n", highpart * 250 + lowpart);
//	   fprintf (stderr, "Change happening in %d CIFs\n", occurrenceChange);
//	}
//
}
//
//	FIG0 extension 1 creates a mapping between the
//	sub channel identifications and the positions in the
//	relevant CIF.
void	fib_processor::FIG0Extension1 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
int16_t	Length	= getBits (d, 3, 5);
uint8_t	PD_bit	= getBit (d, 8 + 2);
//uint8_t	CN	= getBit (d, 8 + 0);

	while (used < Length - 1)
	   used = HandleFIG0Extension1 (d, used, PD_bit);
}

int16_t	fib_processor::HandleFIG0Extension1 (uint8_t *d,
	                                     int16_t offset,
	                                     uint8_t pd) {
int16_t	bitOffset	= offset * 8;
int16_t	SubChId		= getBits (d, bitOffset, 6);
int16_t StartAdr	= getBits (d, bitOffset + 6, 10);
int16_t	tabelIndex;
int16_t	option, protLevel, subChanSize;
	(void)pd;		// not used right now, maybe later
	ficList [SubChId]. StartAddr = StartAdr;
	if (getBit (d, bitOffset + 16) == 0) {	// short form
	   tabelIndex = getBits (d, bitOffset + 18, 6);
	   ficList [SubChId]. Length  	= ProtLevel [tabelIndex][0];
	   ficList [SubChId]. uepFlag	= 0;
	   ficList [SubChId]. protLevel	= ProtLevel [tabelIndex][1];
	   ficList [SubChId]. BitRate	= ProtLevel [tabelIndex][2];
	   bitOffset += 24;
	}
	else { 	// EEP long form
	   ficList [SubChId]. uepFlag	= 1;
	   option = getBits (d, bitOffset + 17, 3);
	   if (option == 0) { 		// A Level protection
	      protLevel = getBits (d, bitOffset + 20, 2) + 1;
//
//	we encode the A level protection by adding 0100 to the level
	      ficList [SubChId]. protLevel = protLevel + 0100;
	      subChanSize = getBits (d, bitOffset + 22, 10);
	      ficList [SubChId]. Length	= subChanSize;
	      if (protLevel == 1)
	         ficList [SubChId]. BitRate	= subChanSize / 12 * 8;
	      if (protLevel == 2)
	         ficList [SubChId]. BitRate	= subChanSize / 8 * 8;
	      if (protLevel == 3)
	         ficList [SubChId]. BitRate	= subChanSize / 6 * 8;
	      if (protLevel == 4)
	         ficList [SubChId]. BitRate	= subChanSize / 4 * 8;
	   }
	   else			// option should be 001
	   if (option == 001) {		// B Level protection
	      protLevel = getBits (d, bitOffset + 20, 2) + 1;
//
//	we encode the B protection levels by adding a 0200 to the level
	      ficList [SubChId]. protLevel = protLevel + 0200;
	      subChanSize = getBits (d, bitOffset + 22, 10);
	      ficList [SubChId]. Length = subChanSize;
	      if (protLevel == 1)
	         ficList [SubChId]. BitRate	= subChanSize / 27 * 32;
	      if (protLevel == 2)
	         ficList [SubChId]. BitRate	= subChanSize / 21 * 32;
	      if (protLevel == 3)
	         ficList [SubChId]. BitRate	= subChanSize / 18 * 32;
	      if (protLevel == 4)
	         ficList [SubChId]. BitRate	= subChanSize / 15 * 32;
	   }

	   bitOffset += 32;
	}

	return bitOffset / 8;	// we return bytes
}

void	fib_processor::FIG0Extension2 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
int16_t	Length	= getBits (d, 3, 5);
uint8_t	PD_bit	= getBit (d, 8 + 2);
uint8_t	CN	= getBit (d, 8 + 0);

//	if (CN)
//	   fprintf (stderr, "FIG0/2 CN = %d\n", CN);
	while (used < Length) {
	   used = HandleFIG0Extension2 (d, used, CN, PD_bit);
	}
}
//
//	Note Offset is in bytes
//	With FIG0/2 we bind the channels to Service Ids
//
int16_t	fib_processor::HandleFIG0Extension2 (uint8_t *d,
	                                     int16_t offset,
	                                     uint8_t cn,
	                                     uint8_t pd) {
int16_t		lOffset	= 8 * offset;
int16_t		i;
uint8_t		ecc;
uint8_t		cId;
uint32_t	SId;
int16_t		numberofComponents;

	if (pd == 1) {		// long Sid
	   ecc	= getBits (d, lOffset, 8);	(void)ecc;
	   cId	= getBits (d, lOffset + 1, 4);
	   SId	= getBits (d, lOffset + 5, 32);
	   lOffset	+= 32;
	}
	else {
	   cId	= getBits (d, lOffset, 4);	(void)cId;
	   SId	= getBits (d, lOffset + 4, 12);
	   SId	= getBits (d, lOffset, 16);
	   lOffset	+= 16;
	}

//	fprintf (stderr, "CAId = %d\n", getBits (d, lOffset + 1, 3));
	numberofComponents	= getBits (d, lOffset + 4, 4);
	lOffset	+= 8;

	for (i = 0; i < numberofComponents; i ++) {
	   uint8_t	TMid	= getBits (d, lOffset, 2);
	   uint8_t	ASCTy	= getBits (d, lOffset + 2, 6);
	   uint8_t	SubChId	= getBits (d, lOffset + 8, 6);
	   uint8_t	PS_flag	= getBit (d, lOffset + 14);
	   if ((TMid == 00) || (TMid == 01))  {	// MSC stream Audio
	      bindService (TMid, SId, i, SubChId, PS_flag, ASCTy);
	      lOffset += 16;
	      if (cn) {
	         int16_t j;
	         serviceId *myIndex	= findServiceId (SId);
	         if (myIndex -> serviceLabel. hasName) {
	            fprintf (stderr, "Binding channel %d to SId  %d(TMid = %d)  ",
	                        SubChId, SId, TMid);
	            for (j = 0; j < 16; j ++) 
	               fprintf (stderr, "%c", myIndex ->  serviceLabel. label [j]);
	            fprintf (stderr, "\n");
	         }
	      }
	   }
	   else if (TMid == 3) { // MSC packet data
	      int16_t SCId	= getBits (d, lOffset + 2, 12);
	      uint8_t PSflag	= getBits (d, lOffset + 14, 1);
	      uint8_t CAflag	= getBits (d, lOffset + 15, 1);
//	      fprintf (stderr, "Packet data: SCId = %d, PS = %d, CA = %d\n",
//	                                 SCId, PSflag, CAflag);
	      (void)SCId;
	      (void)PSflag;
	      (void)CAflag;
	      lOffset += 16;
	   }
	   else
	      lOffset += 16;
	}
	return lOffset / 8;		// in Bytes
}

void	fib_processor::FIG0Extension3 (uint8_t *d) {
int16_t	used	= 2;
int16_t	Length	= getBits (d, 3, 5);

	while (used < Length)
	   used = HandleFIG0Extension3 (d, used);
}

int16_t	fib_processor::HandleFIG0Extension3 (uint8_t *d, int16_t used) {
int16_t	SCId		= getBits (d, used * 8, 12);
int16_t	CAOrgflag	= getBits (d, used * 8 + 15, 1);
int16_t	DGflag		= getBits (d, used * 8 + 16, 1);
int16_t	DSCTy		= getBits (d, used * 8 + 18, 6);
int16_t	SubChId		= getBits (d, used * 8 + 24, 6);
int16_t	packetAddress	= getBits (d, used * 8 + 30, 10);
uint16_t	CAOrg	= getBits (d, used * 8 + 40, 16);

	(void)SCId;
	(void)CAOrgflag;
	(void)DGflag;
	(void)DSCTy;
	(void)SubChId;
	(void)packetAddress;
	(void)CAOrg;
//	fprintf (stderr, "SCId = %d, SubChId = %d, CAOrgflag = %d,  DGflag = %d, packetAddress = %d, CAOrg = %d\n",
//	               SCId, SubChId,  CAOrgflag, DGflag, packetAddress, CAOrg);
	used += 56 / 8;
	return used;
}

void	fib_processor::FIG0Extension5 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
int16_t	Length	= getBits (d, 3, 5);

	while (used < Length) {
	   used = HandleFIG0Extension5 (d, used);
	}
}

int16_t	fib_processor::HandleFIG0Extension5 (uint8_t* d, int16_t offset) {
int16_t	loffset	= offset * 8;
uint8_t	lsFlag	= getBit (d, loffset);
int16_t	subChId, serviceComp, language;

	if (lsFlag == 0) {	// short form
	   if (getBit (d, loffset + 1) == 0) {
	      subChId = getBits (d, loffset + 2, 6);
	      language	= getBits (d, loffset + 8, 8);
	      ficList [subChId]. language = language;
	   }
	   loffset += 16;
	}
	else {			// long form
	   serviceComp = getBits (d, loffset + 4, 12);
	   language	= getBits (d, loffset + 16, 8);
//	   fprintf (stderr,
//	   	    "serviceComp %d, language %d\n", serviceComp, language);
	   loffset += 24;
	}
	(void)serviceComp;

	return loffset / 8;
}

void	fib_processor::FIG0Extension8 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
int16_t	Length	= getBits (d, 3, 5);
uint8_t	PD_bit	= getBit (d, 8 + 2);

	while (used < Length) {
	   used = HandleFIG0Extension8 (d, used, PD_bit);
	}
}

int16_t	fib_processor::HandleFIG0Extension8 (uint8_t *d, int16_t used,
	                                     uint8_t pdBit) {
int16_t	lOffset	= used * 8;
uint32_t	SId	= getLBits (d, lOffset, pdBit == 1 ? 32 : 16);
uint8_t		lsFlag;
uint16_t	SCIds;
int16_t		SCid;
int16_t		MSCflag;
int16_t		SubChId;

	lOffset += pdBit == 1 ? 32 : 16;
	SCIds	= getBits (d, lOffset + 4, 4);
	lOffset += 8;

	lsFlag	= getBit (d, lOffset + 8);
	if (lsFlag == 1) {
	   SCid = getBits (d, lOffset + 4, 12);
	   lOffset += 16;
	}
	else {
	   MSCflag	= getBit (d, lOffset + 1);
	   SubChId	= getBits (d, lOffset + 2, 6);
//	   if (MSCflag == 1)
//	      fprintf (stderr, "Sid = %d, SCIds = %d, FIDCId = %d\n",
//	                  SId, SCIds, SubChId);
	   lOffset += 8;
	}
	lOffset += 8;	// skip Rfa
	(void)SId;
	(void)SCIds;
	(void)SCid;
	(void)SubChId;
	(void)MSCflag;
	return lOffset / 8;
}
//
//	FIG0/9 and FIG0/10 are copied from the work of
//	Michael Hoehn
void fib_processor::FIG0Extension9 (uint8_t *d) {
int16_t	offset	= 16;

	dateTime [6] = (getBit (d, offset + 2) == 1)?
	                -1 * getBits (d, offset + 3, 4):
	                     getBits (d, offset + 3, 4);
	dateTime [7] = (getBit (d, offset + 7) == 1)? 30 : 0;
}
//
void fib_processor::FIG0Extension10 (uint8_t *fig) {
int16_t		offset = 16;
int32_t		mjd	= getBits (fig, offset + 1, 17);
// Modified Julian Date umrechnen (Nach wikipedia)
int32_t J	= mjd + 2400001;
int32_t j	= J + 32044;
int32_t g	= j / 146097; 
int32_t	dg	= j % 146097;
int32_t c	= ((dg / 36524) + 1) * 3 / 4; 
int32_t dc	= dg - c * 36524;
int32_t b	= dc / 1461;
int32_t db	= dc%1461;
int32_t a	= ((db / 365) + 1) * 3 / 4; 
int32_t da	= db - a * 365;
int32_t y	= g * 400 + c * 100 + b * 4 + a;
int32_t m	= ((da * 5 + 308) / 153) - 2;
int32_t d	= da - ((m + 4) * 153 / 5) + 122;
int32_t Y	= y - 4800 + ((m + 2) / 12); 
int32_t M	= ((m + 2) % 12) + 1; 
int32_t D	= d + 1;
	
	dateTime [0] = Y;	// Jahr
	dateTime [1] = M;	// Monat
	dateTime [2] = D;	// Tag
	dateTime [3] = getBits (fig, offset + 21, 5);	// Stunden
	if (getBits (fig ,offset + 26, 6) != dateTime [4]) {
	   dateTime [5] =  0;	// Sekunden (Uebergang abfangen)
	}
	dateTime [4] = getBits (fig, offset + 26, 6);	// Minuten
	if (fig [offset + 20] == 1)
	   dateTime [5]= getBits (fig, offset + 32, 6);	// Sekunden

//	fprintf (stderr, "%d %d %d %d %d %d\n",
//	                  dateTime [0], dateTime [1], dateTime [2],
//	                  dateTime [3], dateTime [4], dateTime [5]);
	dateFlag	= true;
}

void	fib_processor::FIG0Extension13 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
int16_t	Length	= getBits (d, 3, 5);
uint8_t	PD_bit	= getBit (d, 8 + 2);

	while (used < Length) 
	   used = HandleFIG0Extension13 (d, used, PD_bit);
}

int16_t	fib_processor::HandleFIG0Extension13 (uint8_t *d,
	                                     int16_t used,
	                                     uint8_t pdBit) {
int16_t	lOffset		= used * 8;
uint32_t	SId	= getLBits (d, lOffset, pdBit == 1 ? 32 : 16);
uint16_t	SCIds;
int16_t		NoApplications;
int16_t		i;

	lOffset += pdBit == 1 ? 32 : 16;
	SCIds	= getBits (d, lOffset, 4);
	NoApplications	= getBits (d, lOffset + 4, 4);
	lOffset += 8;

	for (i = 0; i < NoApplications; i ++) {
	   int16_t appType = getBits (d, lOffset, 11);
	   int16_t length	= getBits (d, lOffset + 11, 5);
	   lOffset += (11 + 5 + 8 * length);
	   switch (appType) {
	      case 0x000:		// reserved for future use
	      case 0x001:		// not used
	         break;

	      case 0x002:		// MOT slideshow
	      case 0x003:		// MOT Broadcast Web Site
	      case 0x004:		// TPEG
	      case 0x005:		// DGPS
	      case 0x006:		// TMC
	      case 0x007:		// EPG
	      case 0x008:		// DAB Java
	         break;

	      case 0x44a:		// Journaline
//	         fprintf (stderr, "Journaline\n");
	         break;

	      default:
	         break;
	   }
	}

	(void)SId;
	(void)SCIds;
	return lOffset / 8;
}

void	fib_processor::FIG0Extension14 (uint8_t *d) {
int16_t	Length	= getBits (d, 3, 5);	// in Bytes
int16_t	used	= 2;			// in Bytes
	while (used < Length) {
	   int16_t SubChId	= getBits (d, used * 8, 6);
	   uint8_t FEC_Scheme	= getBits (d, used * 8 + 6, 2);
	   used = used + 1;
//	   fprintf (stderr, "FEC subchannel %d has scheme %d\n",
//	                           SubChId, FEC_Scheme);
	   (void)SubChId;
	   (void)FEC_Scheme;
	}
}

void	fib_processor::FIG0Extension16 (uint8_t *d) {
int16_t	length 	= getBits (d, 3, 5);	// in bytes
int16_t	offset	= 16;			// in bits
serviceId	*s;

	while (offset < length * 8) {
	   uint16_t	SId	= getBits (d, offset, 16);
	   s	= findServiceId (SId);
	   if (!s -> hasPNum) {
	      uint8_t PNum = getBits (d, offset + 16, 16);
	      s -> pNum		= PNum;
	      s -> hasPNum	= true;
//	      fprintf (stderr, "Program number info SId = %.8X, PNum = %d\n",
//	      	                               SId, PNum);
	   }
	   offset += 72;
	}
}

void	fib_processor::FIG0Extension17 (uint8_t *d) {
int16_t	length	= getBits (d, 3, 5);
int16_t	offset	= 16;
serviceId	*s;

	while (offset < length * 8) {
	   uint16_t	SId	= getBits (d, offset, 16);
	   bool	L_flag	= getBit (d, offset + 18);
	   bool	CC_flag	= getBit (d, offset + 19);
	   int16_t type;
	   int16_t Language = 0x00;	// init with unknown language
	   s	= findServiceId (SId);
	   if (L_flag) {		// language field present
	      Language = getBits (d, offset + 24, 8);
	      s -> language = Language;
	      s -> hasLanguage = true;
	      offset += 8;
	   }

	   type	= getBits (d, offset + 27, 5);
	   s	-> programType	= type;
	   if (CC_flag)			// cc flag
	      offset += 40;
	   else
	      offset += 32;
	}
}

void	fib_processor::FIG0Extension18 (uint8_t *d) {
int16_t	offset	= 16;		// bits
uint16_t	SId, AsuFlags;
int16_t		Length	= getBits (d, 3, 5);

	while (offset / 8 < Length - 1 ) {
	   int16_t NumClusters = getBits (d, offset + 35, 5);
	   SId	= getBits (d, offset, 16);
	   AsuFlags	= getBits (d, offset + 16, 16);
//	   fprintf (stderr, "Announcement %d for SId %d with %d clusters\n",
//	                    AsuFlags, SId, NumClusters);
	   offset += 40 + NumClusters * 8;
	}
	(void)SId;
	(void)AsuFlags;
}

void	fib_processor::FIG0Extension19 (uint8_t *d) {
int16_t		offset	= 16;		// bits
uint16_t	AswFlags;
int16_t		Length	= getBits (d, 3, 5);
uint8_t		region_Id_Lower;

	while (offset / 8 < Length - 1) {
	   uint8_t ClusterId	= getBits (d, offset, 8);
	   bool new_flag	= getBit (d, offset + 24);
	   bool	region_flag	= getBit (d, offset + 25);
	   uint8_t SubChId	= getBits (d, offset + 26, 6);

	   AswFlags	= getBits (d, offset + 8, 16);
//	   fprintf (stderr,
//	          "%s %s Announcement %d for Cluster %2u on SubCh %2u ",
//	              ((new_flag==1)?"new":"old"),
//	              ((region_flag==1)?"regional":""),
//	              AswFlags, ClusterId,SubChId);
	   if (region_flag) {
	      region_Id_Lower = getBits (d, offset + 34, 6);
	      offset += 40;
//           fprintf(stderr,"for region %u",region_Id_Lower);
	   }
	   else
	      offset += 32;

//	   fprintf(stderr,"\n");
	   (void)ClusterId;
	   (void)new_flag;
	   (void)SubChId;
	}
	(void)AswFlags;
	(void)region_Id_Lower;
}

void	fib_processor::FIG0Extension21 (uint8_t *d) {
//	fprintf (stderr, "Frequency information\n");
	(void)d;
}

void	fib_processor::FIG0Extension22 (uint8_t *d) {
int16_t	Length	= getBits (d, 3, 5);
int16_t	offset	= 16;		// on bits
int16_t	used	= 2;

	while (used < Length) 
	   used = HandleFIG0Extension22 (d, used);
	(void)offset;
}

int16_t	fib_processor::HandleFIG0Extension22 (uint8_t *d, int16_t used) {
uint8_t MS;
int16_t	mainId;
int16_t	noSubfields;

	mainId	= getBits (d, used * 8 + 1, 7);
	(void)mainId;
	MS	= getBits (d, used * 8, 1);
	if (MS == 0) {		// fixed size
	   int16_t latitudeCoarse = getBits (d, used * 8 + 8, 16);
	   int16_t longitudeCoarse = getBits (d, used * 8 + 24, 16);
//	   fprintf (stderr, "Id = %d, (%d %d)\n", mainId,
//	                              latitudeCoarse, longitudeCoarse);
	   (void)latitudeCoarse;
	   (void)longitudeCoarse;
	   return used + 48 / 6;
	}
	//	MS == 1

	noSubfields = getBits (d, used * 8 + 13, 3);
//	fprintf (stderr, "Id = %d, subfields = %d\n", mainId, noSubfields);
	used += (16 + noSubfields * 48) / 8;

	return used;
}

//	FIG 1
//
void	fib_processor::process_FIG1 (uint8_t *d) {
uint8_t		charSet, extension;
uint32_t	SId	= 0;
uint8_t		oe;
int16_t		offset	= 0;
serviceId	*myIndex;
int16_t		i;
uint8_t		pd_flag;
uint8_t		SCidS;
uint8_t		XPAD_aid;
uint8_t		flagfield;
uint8_t		region_id;
char		label [17];
//
//	from byte 1 we deduce:
	charSet		= getBits (d, 8, 4);
	oe		= getBit (d, 8 + 4);
	extension	= getBits (d, 8 + 5, 3); 
	label [16]	= 0x00;
	switch (extension) {
/*
	   default:
	      return;
 */
	   case 0:	// ensemble label
	      SId	= getBits (d, 16, 16);
	      offset	= 32;
	      if ((charSet <= 16)) { // EBU Latin based repertoire
	         for (i = 0; i < 16; i ++) {
	            label [i] = getBits (d, offset + 8 * i, 8);
	            addEnsembleChar (label [i], i);
	         }
//	         fprintf (stderr, "Ensemblename: %16s\n", label);
	         if (!oe) 
	            nameofEnsemble (SId, label);
	      }
//	      fprintf (stderr,
//	               "charset %d is used for ensemblename\n", charSet);
	      break;

	   case 1:	// 16 bit Identifier field for service label
	      SId	= getBits (d, 16, 16);
	      offset	= 32;
	      myIndex	= findServiceId (SId);
	      if ((!myIndex -> serviceLabel. hasName) && (charSet <= 16)) {
	         for (i = 0; i < 16; i ++) {
	            label [i] = getBits (d, offset + 8 * i, 8);
	         }
	         memcpy (myIndex -> serviceLabel. label, label,  sizeof (label) - 1);
//	         fprintf (stderr, "FIG1/1: SId = %4x\t%s\n", SId, label);
	         addtoEnsemble (toQStringUsingCharset ((
	                        const char *)myIndex -> serviceLabel. label,
	                           (CharacterSet) charSet));
	         myIndex -> serviceLabel. hasName = true;
	      }
	      break;

	   case 3:
	      // region label
	      region_id = getBits (d, 16 + 2, 6);
	      offset = 24;
	      for (i = 0; i < 16; i ++) 
	         label [i] = getBits (d, offset + 8 * i, 8);

	      fprintf (stderr, "FIG1/3: RegionID = %2x\t%s\n", region_id, label);
	      break;

	   case 4:
	      pd_flag	= getLBits (d, 16, 1);
	      SCidS	= getLBits (d, 20, 4);
	      if (pd_flag) {	// 32 bit identifier field for service component label
	         SId	= getLBits (d, 24, 32);
	         offset	= 56;
	      }
	      else {	// 16 bit identifier field for service component label
	         SId	= getLBits (d, 24, 16);
	         offset	= 40;
	      }

	      flagfield	= getLBits (d, offset + 128, 16);
	      for (i = 0; i < 16; i ++)
	         label [i] = getBits (d, offset + 8 * i, 8);
//	      fprintf (stderr, "FIG1/4: Sid = %8x\tp/d=%d\tSCidS=%1X\tflag=%8X\t%s\n",
//	                        SId, pd_flag, SCidS, flagfield, label);
	      break;


	   case 5:	// 32 bit Identifier field for service label
	      SId	= getLBits (d, 16, 32);
	      offset	= 48;
	      for (i = 0; i < 16; i ++)
	         label [i] = getBits (d, offset + 8 * i, 8);

//	      fprintf (stderr, "FIG1/5: SId = %8x\t%s\n", SId, label);
	      break;

	   case 6:	// XPAD label
	      pd_flag	= getLBits (d, 16, 1);
	      SCidS	= getLBits (d, 20, 4);
	      if (pd_flag) {	// 32 bits identifier for XPAD label
	         SId		= getLBits (d, 24, 32);
	         XPAD_aid	= getLBits (d, 59, 5);
	         offset 	= 64;
	      }
	      else {	// 16 bit identifier for XPAD label
	         SId		= getLBits (d, 24, 16);
	         XPAD_aid	= getLBits (d, 43, 5);
	         offset		= 48;
	      }

	      for (i = 0; i < 16; i ++)
	         label [i] = getBits (d, offset + 8 * i, 8);

//	      fprintf (stderr, "FIG1/6: SId = %8x\tp/d = %d\t SCidS = %1X\tXPAD_aid = %2u\t%s\n",
//		       SId, pd_flag, SCidS, XPAD_aid, label);
	      break;

	   default:
	      fprintf (stderr, "FIG1/%d: not handled now\n", extension);
	      break;
	}
	(void)SCidS;
	(void)XPAD_aid;
	(void)flagfield;
}
//
//	locate - and create if needed - a reference to the entry
//	for the serviceId serviceId
serviceId	*fib_processor::findServiceId (int32_t serviceId) {
int16_t	i;

	for (i = 0; i < 64; i ++)
	   if ((listofServices [i]. inUse) &&
	        (listofServices [i]. serviceId == serviceId))
	      return &listofServices [i];

	for (i = 0; i < 64; i ++)
	   if (!listofServices [i]. inUse) {
	      listofServices [i]. inUse = true;
	      listofServices [i]. serviceLabel. hasName = false;
	      listofServices [i]. serviceId = serviceId;
	      return &listofServices [i];
	   }

	return &listofServices [0];	// should not happen
}
//
//	bindService is the main processor for - what the name suggests -
//	connecting the description of services to various components,
//	reach component related to a channel
void	fib_processor::bindService (int8_t TMid,
	                            int32_t SId, int16_t compnr,
	                            int16_t subChId,
	                            int16_t ps_flag, int16_t ASCTy) {
serviceId *s	= findServiceId	(SId);
int16_t	i;
//
//	if TMid == 0, we have an audiostream, if (TMid == 1)
//	MSC stream data
	if ((TMid == 0) || (TMid == 1)) {
	   for (i = 0; i < 64; i ++) 
	      if (((components [i]. inUse) &&
	          (components [i]. service == s)&&
	          (components [i]. componentNr == compnr)) ||
	          (!components [i]. inUse)) {
	         components [i]. inUse = true;
	         components [i]. service = s;
	         components [i]. subchannelId = subChId;
	         components [i]. componentNr = compnr;
	         components [i]. PS_flag = ps_flag;
	         components [i]. ASCTy = ASCTy;
	         components [i]. TMid	= TMid;
	         if (TMid == 1) fprintf (stderr, "DSCTy = %d\n", ASCTy);
	         return;
	   }
	}
}

void	fib_processor::setupforNewFrame (void) {
int16_t	i;

	for (i = 0; i < 64; i ++)
	   components [i]. inUse = false;
}

void	fib_processor::clearEnsemble (void) {
int16_t i, j;
	setupforNewFrame ();
	memset (listofServices, 0, sizeof (listofServices));
	memset (components, 0, sizeof (components));
	memset (ficList, 0, sizeof (ficList));
	for (i = 0; i < 64; i ++) {
	   listofServices [i]. inUse = false;
	   listofServices [i]. serviceId = -1;
	   for (j = 0; j < 16; j ++)
	      listofServices [i]. serviceLabel. label [j] = ' ';
	}
	selectedService	= -1;
	myDecoder	-> stopProcessing ();
}

void	fib_processor::printActions (int16_t ficno) {
int16_t	i, j;
uint8_t		*p;
uint16_t	subchId;

	for (i = 0; i < 64; i ++) {
	   if (!components [i]. inUse)
	      continue;

	   p = &(components [i]. service -> serviceLabel. label [0]);
	   if (*p == ' ')
	      continue;

	   if (selectedService != components [i]. service -> serviceId)
	      continue;

//	   fprintf (stderr, "%d >>> (component = %d) ", ficno, 
//	                                 components [i]. componentNr);
//	   for (j = 0; j < 16; j ++)
//	      fprintf (stderr, "%1c", p [j]);
//	   fprintf (stderr, "with SId = %d requests ", 
//	                                       components [i]. service -> serviceId);
//
//	   fprintf (stderr, "SubChId = %d ", components [i]. subchannelId);
	   subchId	= components [i]. subchannelId;

//	   fprintf (stderr, "StartAdd = %d ", ficList [subchId]. StartAddr);
//	   fprintf (stderr, "Length = %d ", ficList [subchId]. Length);
//	   fprintf (stderr, "uepFlag = %d ", ficList [subchId]. uepFlag);
//	   fprintf (stderr, "protLevel = %d ", ficList [subchId]. protLevel);
//	   fprintf (stderr, "ASCTy = %d ", components [i]. ASCTy);
//	   fprintf (stderr, "BitRate = %d\n", ficList [subchId]. BitRate);
	}
}
//
//	for now we assume that selecting a service means
//	selecting a named music stream
void	fib_processor::setSelectedService (char *s) {
int16_t	i, j;
bool	equal;

	for (i = 0; i < 64; i ++) {
	   if (!listofServices [i]. inUse)
	      continue;

	   if (!listofServices [i]. serviceLabel. hasName)
	      continue;

	   equal = true;
	   for (j = 0; j < 15; j ++) 
	      if (listofServices [i]. serviceLabel. label [j] != s [j]) {
	         equal = false;
	         break;
	      }

	   if (!equal)
	      continue;

//	   fprintf (stderr, "we found for %s serviceId %d\n", s, 
//	                      listofServices [i]. serviceId);
	   selectedService = listofServices [i]. serviceId;
	   for (j = 0; j < 64; j ++) {
	      int16_t subchId;
	      if (selectedService != components [j]. service -> serviceId)
	         continue;
	
//	      fprintf (stderr, "Comp %d SubChId = %d ",
//	                                j,  components [j]. subchannelId);
	      subchId	= components [j]. subchannelId;

//	      fprintf (stderr,
//	             "StartAdd = %d ", ficList [subchId]. StartAddr);
//	      fprintf (stderr, "Length = %d ", ficList [subchId]. Length);
//	      fprintf (stderr, "uepFlag = %d ", ficList [subchId]. uepFlag);
//	      fprintf (stderr,
//	               "protLevel = %d ", ficList [subchId]. protLevel);
//	      fprintf (stderr, "ASCTy = %d ", components [j]. ASCTy);
//	      fprintf (stderr, "TMid = %d ", components [j]. TMid);
//	      fprintf (stderr, "BitRate = %d\n", ficList [subchId]. BitRate);
	      if (components [j]. TMid != 0) 	// no audio stream
	         continue;
	      myDecoder -> setChannel (subchId, 
	                               ficList [subchId]. uepFlag,
	                               ficList [subchId]. StartAddr,
	                               ficList [subchId]. Length,
	                               ficList [subchId]. protLevel,
	                               ficList [subchId]. BitRate,
	                               components [j]. ASCTy,
	                               listofServices [i]. language,
	                               listofServices [i]. programType);
	      technicalData (subchId, 
	                               ficList [subchId]. uepFlag,
	                               ficList [subchId]. StartAddr,
	                               ficList [subchId]. Length,
	                               ficList [subchId]. protLevel,
	                               ficList [subchId]. BitRate,
	                               components [i]. ASCTy);
	      return;
	   }
	}
	selectedService = -1;
	fprintf (stderr, "We did not find %s\n", s);
}
//
//
//	select a particular channel
void	fib_processor::setunnamedService (int n) {
int16_t	i;

	for (i = 0; i < 64; i ++) {
	   int16_t subchId;

	   if (components [i]. subchannelId != n)
	      continue;
//	   fprintf (stderr, "Compnr = %d ", i);
//	   fprintf (stderr, "SubChId = %d ", components [i]. subchannelId);
	   subchId	= n;

//	   fprintf (stderr,
//	             "StartAdd = %d ", ficList [subchId]. StartAddr);
//	   fprintf (stderr, "Length = %d ", ficList [subchId]. Length);
//	   fprintf (stderr, "uepFlag = %d ", ficList [subchId]. uepFlag);
//	   fprintf (stderr,
//	               "protLevel = %d ", ficList [subchId]. protLevel);
//	   fprintf (stderr, "ASCTy = %d ", components [i]. ASCTy);
//	   fprintf (stderr, "TMid = %d ", components [i]. TMid);
//	   fprintf (stderr, "BitRate = %d\n", ficList [subchId]. BitRate);
	   if (components [i]. TMid != 0) 	// no audio stream
	      continue;
	   if ((ficList [subchId]. Length == 0) &&
	          (ficList [subchId]. protLevel == 0))
	      continue;

	   myDecoder -> setChannel (subchId, 
	                               ficList [subchId]. uepFlag,
	                               ficList [subchId]. StartAddr,
	                               ficList [subchId]. Length,
	                               ficList [subchId]. protLevel,
	                               ficList [subchId]. BitRate,
	                               components [i]. ASCTy, 0, 0);
	   technicalData (subchId, 
	                               ficList [subchId]. uepFlag,
	                               ficList [subchId]. StartAddr,
	                               ficList [subchId]. Length,
	                               ficList [subchId]. protLevel,
	                               ficList [subchId]. BitRate,
	                               components [i]. ASCTy);

	   return;
	}
	selectedService = -1;
	fprintf (stderr, "channel %d not in use\n", n);
}
//

