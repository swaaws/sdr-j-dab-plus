#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J.
 *    Many of the ideas as implemented in SDR-J are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are recognized.
 *
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
#include	<QFileDialog>
#include	<QSettings>
#include	<QMessageBox>
#include	"elad-handler.h"
#include	"virtual-reader.h"
#include	"common-readers.h"

#ifndef	__MINGW32__
#include	"dlfcn.h"
#endif
using namespace std;

#ifdef __MINGW32__
#define	GETPROCADDRESS	GetProcAddress
#define	FREELIBRARY	FreeLibrary
#else
#define	GETPROCADDRESS	dlsym
#define	FREELIBRARY	dlclose
#endif
//

//	Simple interface routine to elad using the 3072 dll
//	One rate, which will be downconverted
//	The callback from the Extio*.dll does not contain a "context",
//	so we have to create one artificially in order to access
//	our functions
//	Our context here is the instance of the class, stored in
//	a static variable
static
eladHandler	*myContext	= NULL;

static
int	extioCallback (int cnt, int status, float IQoffs, void *IQData) {
	if (cnt > 0) { 	//	we got data
	   if (myContext != NULL && myContext -> isStarted) 
	      myContext -> theReader -> processData (IQoffs, IQData, cnt);
	}
	else	// we got something 
	if (cnt < 0) {
	   fprintf (stderr, "got a status cmd %d\n", status);
	   switch (status) {
	// for the SDR14
	      case extHw_Disconnected:
	      case extHw_READY:
	      case extHw_RUNNING:
	      case extHw_ERROR:
	      case extHw_OVERLOAD:
	         break;
	      case extHw_Changed_SampleRate:	// 100
	         break;
	      case extHw_Changed_LO:		// 101
	         break;
	      case extHw_Lock_LO:		// 102
	         break;
	      case extHw_Unlock_LO:		// 103
	         break;
	      case extHw_Changed_LO_Not_TUNE:	// 104
	         break;
	      case extHw_Changed_TUNE:		// 105
	         break;
	      case extHw_Changed_MODE:		// 106
	         break;
	      case extHw_Start:			// 107
	         break;
	      case extHw_Stop:			// 108
	         break;
	      case extHw_Changed_FILTER:	// 109
	         break;
	      default:
	         break;
	   }
	}
	return 1;
}
//
//	the seemingly additional complexity is caused by the fact
//	that naming of files in windows is in MultiBytechars
//
//	We assume that if there are settings possible, they
//	are dealt with by the producer of the extio, so here 
//	no frame whatsoever.
	eladHandler::eladHandler (QSettings *s, bool *success) {
#ifdef	__MINGW32__
char	temp [256];
wchar_t	*windowsName;
int16_t	wchars_num;
#endif
	*success	= false;

	inputRate	= 3072000;	// default
	lastFrequency	= Khz (25000);
	base_16		= s -> value ("base_16", 32767). toInt ();
	base_24		= s -> value ("base_24", 32767 * 256). toInt ();
	base_32		= s -> value ("base_32", 32767 * 32768). toInt ();
	isStarted	= false;
	theReader	= NULL;
	dll_open	= false;

	QString	dll_file	= "extio_elad_fdms2_3072k_v3_04_x64.dll";
#ifdef	__MINGW32__
	wchars_num = MultiByteToWideChar (CP_UTF8, 0,
	                              dll_file. toLatin1 (). data (),
	                              -1, NULL, 0);
	windowsName = new wchar_t [wchars_num];
	MultiByteToWideChar (CP_UTF8, 0,
	                     dll_file. toLatin1 (). data (),
	                     -1, windowsName, wchars_num);
	wcstombs (temp, windowsName, 128);
	Handle		= LoadLibrary (windowsName);
	fprintf (stderr, "Last error = %ld\n", GetLastError ());
#else
	Handle		= dlopen (dll_file. toLatin1 (). data (), RTLD_NOW);
#endif
	if (Handle == NULL) {
	   QMessageBox::warning (NULL, tr ("sdr"),
	                               tr ("loading dll failed\n"));
	   return;
	}

	if (!loadFunctions ()) {
	   QMessageBox::warning (NULL, tr ("sdr"),
	                               tr ("loading functions failed\n"));
	   return;
	}
//	apparently, the library is open, so record that
	dll_open	= true;
	myContext	= (eladHandler *)this;
//	
//	and start the rig
	rigName		= new char [128];
	rigModel	= new char [128];
	if (!((*InitHW) (rigName, rigModel, hardwareType))) {
	   QMessageBox::warning (NULL, tr ("sdr"),
	                               tr ("init failed\n"));
	   exit (1);
	}

	theBuffer	= new RingBuffer<DSPCOMPLEX>(1024 * 1024);
	fprintf (stderr, "hardware type = %d\n", hardwareType);
	switch (hardwareType) {
	   case exthwNone:
	   case exthwSDRX:
	   case exthwHPSDR:
	   case exthwSDR14:
	   case exthwSCdata:
	   default:
	      QMessageBox::warning (NULL, tr ("sdr"),
	                               tr ("device not supported\n"));
	      return;

	   case exthwUSBdata16:
	      theReader	= new reader_16 (theBuffer, base_16);
	      break;
	   case exthwUSBdata24:
	      theReader	= new reader_24 (theBuffer, base_24);
	      break;
	   case exthwUSBdata32:
	      theReader	= new reader_32 (theBuffer, base_32);
	      break;
	   case exthwUSBfloat32:
	      fprintf (stderr, "buffer with float input allocated\n");
	      theReader	= new reader_float (theBuffer);
	      break;
	}

	SetCallback (extioCallback);
	if (!(*OpenHW)()) {
	   QMessageBox::warning (NULL, tr ("sdr"),
	                               tr ("Opening hardware failed\n"));
	   exit (1);
	}
	ShowGUI ();
	fprintf (stderr, "Hw open successful\n");
	*success	= true;
}

	eladHandler::~eladHandler (void) {
	if (dll_open) {
	   HideGUI ();
	   StopHW ();
	   CloseHW ();
	}

	if (Handle != NULL)
	   FREELIBRARY (Handle);

	if (theReader != NULL)
	   delete theReader;
}

bool	eladHandler::loadFunctions (void) {
//	start binding addresses, 
	InitHW		= (pfnInitHW)GETPROCADDRESS (Handle, "InitHW");
	if (InitHW == NULL) {
	   fprintf (stderr, "Failed to load InitHW\n");
	   return false;
	}

	OpenHW		= (pfnOpenHW)GETPROCADDRESS (Handle, "OpenHW");
	if (OpenHW == NULL) {
	   fprintf (stderr, "Failed to load OpenHW\n");
	   return false;
	}

	StartHW		= (pfnStartHW)GETPROCADDRESS (Handle, "StartHW");
	if (StartHW == NULL) {
	   fprintf (stderr, "Failed to load StartHW\n");
	   return false;
	}

	StopHW		= (pfnStopHW)GETPROCADDRESS (Handle, "StopHW");
	if (StopHW == NULL) {
	   fprintf (stderr, "Failed to load StopHW\n");
	   return false;
	}

	CloseHW		= (pfnCloseHW)GETPROCADDRESS (Handle, "CloseHW");
	if (CloseHW == NULL) {
	   fprintf (stderr, "Failed to load CloseHW\n");
	   return false;
	}

	GetHWLO		= (pfnGetHWLO)GETPROCADDRESS (Handle, "GetHWLO");
	if (GetHWLO == NULL) {
	   fprintf (stderr, "Failed to load GetHWLO\n");
	   return false;
	}

	SetHWLO		= (pfnSetHWLO)GETPROCADDRESS (Handle, "SetHWLO");
	if (SetHWLO == NULL) {
	   fprintf (stderr, "Failed to load SetHWLO\n");
	   return false;
	}

	GetStatus	= (pfnGetStatus)GETPROCADDRESS (Handle, "GetStatus");
	if (GetStatus == NULL) {
	   fprintf (stderr, "Failed to load GetStatus\n");
	   return false;
	}

	SetCallback	= (pfnSetCallback)
	                      GETPROCADDRESS (Handle, "SetCallback");
	if (SetCallback == NULL) {
	   fprintf (stderr, "Failed to load SetCallback\n");
	   return false;
	}
//
//	the "non essentials", packed in envelope functions:
	L_ShowGUI	= (pfnShowGUI)GETPROCADDRESS (Handle, "ShowGUI");
	L_HideGUI	= (pfnHideGUI)GETPROCADDRESS (Handle, "HideGUI");
	L_GetHWSR	= (pfnGetHWSR)GETPROCADDRESS (Handle, "GetHWSR");
	L_GetFilters	= (pfnGetFilters)GETPROCADDRESS (Handle, "GetFilters");
	L_GetTune	= (pfnGetTune)GETPROCADDRESS (Handle, "GetTune");
	L_GetMode	= (pfnGetMode)GETPROCADDRESS (Handle, "GetMode");
//
	return true;
}

int32_t	eladHandler::getRate	(void) {
	return GetHWSR ();
}

void	eladHandler::setVFOFrequency (int32_t f) {
	fprintf (stderr, "setting freq to %d\n", f);
int	h =  (*SetHWLO) ((int)f);
	lastFrequency = f;
}

int32_t	eladHandler::getVFOFrequency (void) {
//	lastFrequency = (*GetHWLO)();
	return lastFrequency;
}

//
//	envelopes for functions that might or might not
//	be available
void	eladHandler::ShowGUI		(void) {
	if (L_ShowGUI != NULL)
	   (*L_ShowGUI) ();
}

void	eladHandler::HideGUI		(void) {
	if (L_HideGUI != NULL)
	   (*L_HideGUI) ();
}

long	eladHandler::GetHWSR		(void) {
	return L_GetHWSR != NULL ? (*L_GetHWSR) () : 192000;
}
//
//
//	Handling the data
bool	eladHandler::restartReader	(void) {
	fprintf (stderr, "restart reader entered (%d)\n", lastFrequency);
int32_t	size	= (*StartHW)(lastFrequency);
	fprintf (stderr, "restart reader returned with %d\n", size);
	theReader -> restartReader (size);
	fprintf (stderr, "now we have restarted the reader\n");
	isStarted	= true;
	return true;
}

void	eladHandler::stopReader	(void) {
	if (isStarted) {
	   (*StopHW)();
	   theReader	-> stopReader ();
	   isStarted = false;
	}
}

int32_t	eladHandler::Samples		(void) {
int32_t	x = theBuffer -> GetRingBufferReadAvailable ();
	if (x < 0)
	   fprintf (stderr, "toch een fout in ringbuffer\n");
	return x;
}

int32_t	eladHandler::getSamples		(DSPCOMPLEX *buffer,
	                                         int32_t number) {
int32_t	amount;

	amount	= theBuffer -> getDataFromBuffer (buffer, number);
	return amount;
}

int16_t	eladHandler::bitDepth		(void) {
	return	theReader	-> bitDepth ();
}

bool	eladHandler::legalFrequency (int32_t f) {
	return Khz (24000) <= f && f <= Khz (2000000);
}

int32_t	eladHandler::defaultFrequency (void) {
	return Khz (220000);
}

