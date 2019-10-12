#
/*
 *    Copyright (C) 2012, 2013, 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the  SDR-J (JSDR).
 *    Many of the ideas as implemented in SDR-J are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are acknowledged.
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
#include	<QSettings>
#include	<QMessageBox>
#include	<QFileDialog>
#include	<QDebug>
#include	<QDateTime>
#include	<QStringList>
#include	<QStringListModel>
#include	"dab-constants.h"
#include	"gui.h"
#include	"audiosink.h"
#include	"ofdm-processor-b.h"
#include	"dabstick.h"
#include	"sdrplay.h"
#include	"airspy.h"

#define		BAND_III	0100
#define		L_BAND		0101
/*
 *	We use the creation function merely to set up the
 *	user interface and make the connections between the
 *	gui elements and the handling agents.
 */
	RadioInterface::RadioInterface (QSettings	*Si,
	                                const char	*theDevice,
	                                const char	*theBand,
	                                const char	*theMode,
	                                bool		showErrors,
	                                QWidget		*parent): QDialog (parent) {
bool	success;
int16_t	i, k;

	dabSettings		= Si;
	this	-> showErrors	= showErrors;

	vfoFrequency		= 0;
// 	the setup for the generated part of the ui
	setupUi (this);
	basicBuffers		= 2;		// not used anymore
	ringbufferSize		= 1024;
//	The default:
	if (theDevice != NULL)
	   dabDevice = QString (theDevice);
	else
	   dabDevice	= dabSettings -> value ("device", "dabstick"). toString ();
	if ((dabDevice == "dabstick") || (dabDevice == "dabStick"))
	   myRig = new dabStick (dabSettings, &success);
	else
	if (dabDevice == "airspy")
	   myRig = new airspyHandler (dabSettings, &success);
	else
//	if ((dabDevice == "sdrPlay") || (dabDevice == "sdrplay"))
	   myRig = new sdrplay (dabSettings, &success);
	
	
	if (!success) {
	   QMessageBox::warning (this, tr ("sdr"),
	                               tr ("Device problem\n"));
	   exit (1);
	}

//	Note that the generation of values to be displayed
//	is in a separate thread, we need a buffer for communication
//	and a signal/slot combination for triggering.
//	In the "large" dab decoder, the scopes are made visible
	scopeBuffer		= new RingBuffer<DSPCOMPLEX>(2 * 2048);
	iqBuffer		= new RingBuffer<DSPCOMPLEX> (1024);
	isSynced		= false;
	syncedLabel		->
	               setStyleSheet ("QLabel {background-color : red}");
//
	TunedFrequency		= MHz (200);	// any value will do
	outRate			= 48000;
	outBuffer		= 32768;
//	Maybe we should move the outTable to the audioSink?
	our_audioSink		= new audioSink		(outRate, outBuffer);
	outTable		= new int16_t
	                             [our_audioSink -> numberofDevices ()];
	for (i = 0; i < our_audioSink -> numberofDevices (); i ++)
	   outTable [i] = -1;

	if (!setupSoundOut (streamOutSelector,
	                    our_audioSink, outRate, outTable)) {
	   fprintf (stderr, "Cannot open any output device\n");
	   exit (22);
	}

	dabBand	= dabSettings -> value ("band", theBand). toString ();
	setupChannels	(channelSelector, dabBand == "L_BAND" ? L_BAND : BAND_III);
	dabMode	= dabSettings -> value ("mode", theMode). toString ();
	setModeParameters (dabMode == "Mode 4" ? 4 :
	                   dabMode == "Mode 2" ? 2 :
	                   dabMode == "Mode 3" ? 3 : 1);
	
	the_mscHandler		= new mscHandler	(this,
	                                                 our_audioSink,
	                                                 1,
	                                                 true);
	the_ficHandler		= new ficHandler	(this,
	                                                 the_mscHandler);

	the_ofdmProcessor	= new ofdm_processor_b (myRig,
	                                                &dabModeParameters,
	                                                this,
	                                                the_mscHandler,
	                                                the_ficHandler,
	                                                2048,
	                                                scopeBuffer,
	                                                iqBuffer,
	                                                false);
//
	ensemble.setStringList (Services);
	ensembleDisplay	-> setModel (&ensemble);
	Services << " ";
	ensemble. setStringList (Services);
	ensembleDisplay	-> setModel (&ensemble);
//
//	The connects of the GUI to the handlers
	connect (ensembleDisplay, SIGNAL (clicked (QModelIndex)),
	            this, SLOT (selectService (QModelIndex)));
	connect (QuitButton, SIGNAL (clicked (void)),
	            this, SLOT (TerminateProcess (void)));
	connect (resetButton, SIGNAL (clicked (void)),
	            this, SLOT (handleReset (void)));
	connect (streamOutSelector, SIGNAL (activated (int)),
	            this, SLOT (setStreamOutSelector (int)));
	connect (channelSelector, SIGNAL (activated (const QString &)),
	            this, SLOT (set_channelSelect (const QString &)));
//
//	Timers
	displayTimer		= new QTimer ();
	displayTimer		-> setInterval (1000);
	connect (displayTimer,
	         SIGNAL (timeout (void)),
	         this,
	         SLOT (updateTimeDisplay (void)));
	QString h = dabSettings -> value ("channel", "12C"). toString ();
	k	= channelSelector -> findText (h);
	if (k != -1) {
	   channelSelector -> setCurrentIndex (k);
	}
	
//	display the version
	QString v = "sdr-j DAB(+) mini  " ;
	v. append (CURRENT_VERSION);
	versionName	-> setText (v. toLatin1 (). data ());
//	and start the timer
	displayTimer		-> start (1000);
	set_channelSelect	(channelSelector -> currentText ());
	the_ofdmProcessor	-> coarseCorrectorOn ();
	the_ofdmProcessor	-> reset ();
	h	= dabSettings -> value ("audioOut", "no device").
	                                                 toString ();
	k		= channelSelector -> findText (h);
	if (k != -1) {
	   channelSelector -> setCurrentIndex (k);
	   setStreamOutSelector (k);
	}
	else
	   our_audioSink	-> selectDefaultDevice ();
	running	= false;

	setStart ();
}

void	RadioInterface::setStart	(void) {
	if (running) 
	   myRig	-> stopReader ();
	soundDisplay	-> display (0);
	snrDisplay	-> display (0);
	int r = myRig		-> restartReader ();
	qDebug ("Starting %d\n", r);
	if (!r) {
	   QMessageBox::warning (this, tr ("sdr"),
	                               tr ("Opening  input stream failed\n"));
	   exit (1);
	}
//	This does not hurt
	our_audioSink	-> restart ();
	running	= true;
}

	RadioInterface::~RadioInterface () {
	myRig		-> stopReader ();
	usleep (1000);
	delete		the_ofdmProcessor;
	delete		displayTimer;
	delete		the_mscHandler;
	delete		the_ficHandler;
	delete		myRig;
}
//
//	at the end, save the values used
void	RadioInterface::dumpControlState (QSettings *s) {
	if (s == NULL)	// cannot happen
	   return;

	s	-> setValue ("channel",
	                      channelSelector -> currentText ());

	s	-> setValue ("audioOut",
	                     streamOutSelector -> currentText ());
	s	-> setValue ("band", dabBand. toLatin1 (). data ());
	s	-> setValue ("mode", dabMode. toLatin1 (). data ());
	s	-> setValue ("device", dabDevice. toLatin1 (). data ());
}
//
void	RadioInterface::TerminateProcess (void) {
	delete	displayTimer;
	dumpControlState (dabSettings);
	accept ();
	qDebug () <<  "Termination started";
	the_ofdmProcessor	-> stop ();
	delete	the_ofdmProcessor;
	delete	myRig;
}

void	RadioInterface::abortSystem (int d) {
	qDebug ("aborting for reason %d\n", d);
	accept ();
}
//
void	RadioInterface::setTuner (int32_t n) {
int32_t	cf	= 0;
	myRig		-> setVFOFrequency (n);
	cf		= myRig	 -> getVFOFrequency ();
	vfoFrequency	= n;
}

void	RadioInterface::updateTimeDisplay (void) {
QDateTime	currentTime = QDateTime::currentDateTime ();
	timeDisplay	-> setText (currentTime.
	                            toString (QString ("dd.MM.yy:hh:mm:ss")));
	if (showErrors)
	   updateErrors ();
}
//
void	RadioInterface::set_fineCorrectorDisplay (int v) {
//	fineCorrectorDisplay	-> display (v);
}

void	RadioInterface::set_coarseCorrectorDisplay (int v) {
//	coarseCorrectorDisplay	-> display (v);
}

//
struct dabFrequencies {
	const char	*key;
	int	fKHz;
};

struct dabFrequencies bandIII_frequencies [] = {
{"5A",	174928},
{"5B",	176640},
{"5C",	178352},
{"5D",	180064},
{"6A",	181936},
{"6B",	183648},
{"6C",	185360},
{"6D",	187072},
{"7A",	188928},
{"7B",	190640},
{"7C",	192352},
{"7D",	194064},
{"8A",	195936},
{"8B",	197648},
{"8C",	199360},
{"8D",	201072},
{"9A",	202928},
{"9B",	204640},
{"9C",	206352},
{"9D",	208064},
{"10A",	209936},
{"10B", 211648},
{"10C", 213360},
{"10D", 215072},
{"11A", 216928},
{"11B",	218640},
{"11C",	220352},
{"11D",	222064},
{"12A",	223936},
{"12B",	225648},
{"12C",	227360},
{"12D",	229072},
{"13A",	230748},
{"13B",	232496},
{"13C",	234208},
{"13D",	235776},
{"13E",	237448},
{"13F",	239200},
{NULL, 0}
};

struct dabFrequencies Lband_frequencies [] = {
{"LA", 1452960},
{"LB", 1454672},
{"LC", 1456384},
{"LD", 1458096},
{"LE", 1459808},
{"LF", 1461520},
{"LG", 1463232},
{"LH", 1464944},
{"LI", 1466656},
{"LJ", 1468368},
{"LK", 1470080},
{"LL", 1471792},
{"LM", 1473504},
{"LN", 1475216},
{"LO", 1476928},
{"LP", 1478640},
{NULL, 0}
};

void	RadioInterface::setupChannels (QComboBox *s, uint8_t band) {
struct dabFrequencies *t;
int16_t	i;
int16_t	c	= s -> count ();

	for (i = 0; i < c; i ++)
	   s	-> removeItem (c - (i + 1));
	if (band == L_BAND)
	   t = Lband_frequencies;
	else
	   t = bandIII_frequencies;

	for (i = 0; t [i]. key != NULL; i ++)
	   s -> insertItem (i, t [i]. key, QVariant (i));
}
//
void	RadioInterface::set_channelSelect (QString s) {
int16_t	i;
struct dabFrequencies *finger;
	our_audioSink	-> stop ();
	myRig		-> stopReader ();
	myRig		-> resetBuffer ();

	usleep (100);
	Services		= QStringList ();
	ensemble. setStringList (Services);
	ensembleDisplay		-> setModel (&ensemble);
	the_ficHandler		-> clearEnsemble ();
	ensembleId		-> display (0);
	ensembleName		-> setText (QString (""));
	TunedFrequency		= 0;
	if ((dabBand == "BAND III") || (dabBand == "BAND_III"))
	   finger = bandIII_frequencies;
	else
	   finger = Lband_frequencies;

	for (i = 0; finger [i]. key != NULL; i ++) {
	   if (finger [i]. key == s) {
	      TunedFrequency	= KHz (finger [i]. fKHz);
	      setTuner (TunedFrequency);
	      break;
	   }
	}

	if (TunedFrequency == 0)
	   return;
	setStart ();
}
//
//	on changing settings, we clear all things in the gui
//	related to the ensemble
void	RadioInterface::clearEnsemble	(void) {
//
//	first the real stuff
	Services = QStringList ();
	ensemble. setStringList (Services);
	ensembleDisplay		-> setModel (&ensemble);
	the_ficHandler		-> clearEnsemble ();
	the_ofdmProcessor	-> coarseCorrectorOn ();
	the_ofdmProcessor	-> reset ();
	ensembleId		-> display (0);
	ensembleName		-> setText (QString (""));
//
//	Then the various displayed items
//	programName		-> setText ("   ");
//	ensembleName		-> setText ("   ");
//	errorDisplay		-> display (0);
//	ficRatioDisplay		-> display (0);
//	snrDisplay		-> display (0);
//	uepFlagDisplay		-> display (0);
//	startAddrDisplay	-> display (0);
//	LengthDisplay		-> display (0);
//	protLevelDisplay	-> display (0);
//	bitRateDisplay		-> display (0);
//	ASCTyDisplay		-> display (0);
}

void	RadioInterface::addtoEnsemble (const QString &s) {
	Services << s;
	Services. removeDuplicates ();
	ensemble. setStringList (Services);
	ensembleDisplay	-> setModel (&ensemble);
}
//
//	a slot, called by the fib processor
void	RadioInterface::addEnsembleChar (char v, int i) {
	ensembleLabel [i] = v;
	ensembleLabel [i + 1] = (char)0;
}

void	RadioInterface::nameofEnsemble (int id, char *v) {
QString s;

	(void)v;
	ensembleId		-> display (id);
	ensembleName		-> setText (ensembleLabel);
	the_ofdmProcessor	-> coarseCorrectorOff ();
}

static 
const char *table12 [] = {
"none",
"news",
"current affairs",
"information",
"sport",
"education",
"drama",
"arts",
"science",
"talk",
"pop music",
"rock music",
"easy listening",
"light classical",
"classical music",
"other music",
"wheather",
"finance",
"children\'s",
"factual",
"religion",
"phone in",
"travel",
"leisure",
"jazz and blues",
"country music",
"national music",
"oldies music",
"folk music",
"entry 29 not used",
"entry 30 not used",
"entry 31 not used"
};

const char *RadioInterface::get_programm_type_string (uint8_t type) {
	if (type > 0x40) {
	   fprintf (stderr, "GUI: programmtype wrong (%d)\n", type);
	   return (table12 [0]);
	}

	return table12 [type];
}

static
const char *table9 [] = {
"unknown",
"Albanian",
"Breton",
"Catalan",
"Croatian",
"Welsh",
"Czech",
"Danish",
"German",
"English",
"Spanish",
"Esperanto",
"Estonian",
"Basque",
"Faroese",
"French",
"Frisian",
"Irish",
"Gaelic",
"Galician",
"Icelandic",
"Italian",
"Lappish",
"Latin",
"Latvian",
"Luxembourgian",
"Lithuanian",
"Hungarian",
"Maltese",
"Dutch",
"Norwegian",
"Occitan",
"Polish",
"Postuguese",
"Romanian",
"Romansh",
"Serbian",
"Slovak",
"Slovene",
"Finnish",
"Swedish",
"Tuskish",
"Flemish",
"Walloon"
};

const char *RadioInterface::get_programm_language_string (uint8_t language) {
	if (language > 43) {
	   fprintf (stderr, "GUI: wrong language )%d)\n", language);
	   return table9 [0];
	}
	return table9 [language];
}

void	RadioInterface::selectService (QModelIndex s) {
int8_t	dabModus;
int16_t	type, language;

QString a = ensemble. data (s, Qt::DisplayRole). toString ();
	the_ficHandler -> setSelectedService (a. toLatin1 (). data ());
	dabModus	= the_mscHandler	-> getMode ();
	language	= the_mscHandler	-> getLanguage ();
	type		= the_mscHandler	-> getType ();
//	programName	-> setText (a. toLatin1 (). data ());
//	switch (dabModus) {
//	   case DAB:	dabMode -> setText ("DAB");
//	                break;
//
//	   case DAB_PLUS:
//	                dabMode -> setText ("DAB +");
//	                break;
//
//	   default:
//	                dabMode -> setText ("No select");
//	                break;
//	}
//
//	nameofLanguage	-> setText (get_programm_language_string (language));
//	programType	-> setText (get_programm_type_string (type));
}

void	RadioInterface::channelData (int subchId,
	                             int uepFlag,
	                             int startAddr,
	                             int Length,
	                             int protLevel,
	                             int bitRate,
	                             int ASCTy) {
//	(void)subchId;
//	uepFlagDisplay		-> display (uepFlag);
//	startAddrDisplay	-> display (startAddr);
//	LengthDisplay		-> display (Length);
//	protLevelDisplay	-> display (protLevel);
//	bitRateDisplay		-> display (bitRate);
//	ASCTyDisplay		-> display (ASCTy);
}

void	RadioInterface::show_successRate (int s) {
	soundDisplay	-> display (s);
//	fprintf (stderr, "successrate = %d\n", s);
}

void	RadioInterface::show_ficRatio (int s) {
//	ficRatioDisplay	-> display (s);
}

void	RadioInterface::show_snr (int s) {
	snrDisplay	-> display (s);
}
//
//
//	showScope is called through the signal mechanism
//	It is called whenever the ofdm_processor has
//	filled the buffer with displaySize elements. 
//	In the small GUI version it is a noop (other than emptying the buffer)
//
void	RadioInterface::showScope (int amount) {
DSPCOMPLEX	*dump 	= (DSPCOMPLEX *)alloca (amount * sizeof (DSPCOMPLEX));
	scopeBuffer -> getDataFromBuffer (dump, amount);
}
//
//	signal, received from ofdm_decoder that a buffer is filled
//	with amount values ready for display
void	RadioInterface::showIQ	(int amount) {
DSPCOMPLEX *Values	= (DSPCOMPLEX *)alloca (amount * sizeof (DSPCOMPLEX));
	(void)iqBuffer -> getDataFromBuffer (Values, amount);
}

//	do not forget that ocnt starts with 1, due
//	to Qt list conventions
bool	RadioInterface::setupSoundOut (QComboBox	*streamOutSelector,
	                               audioSink	*our_audioSink,
	                               int32_t		cardRate,
	                               int16_t		*table) {
uint16_t	ocnt	= 1;
uint16_t	i;

	for (i = 0; i < our_audioSink -> numberofDevices (); i ++) {
	   const char *so =
	             our_audioSink -> outputChannelwithRate (i, cardRate);
	   qDebug ("Investigating Device %d\n", i);

	   if (so != NULL) {
	      streamOutSelector -> insertItem (ocnt, so, QVariant (i));
	      table [ocnt] = i;
	      qDebug (" (output):item %d wordt stream %d\n", ocnt , i);
	      ocnt ++;
	   }
	}

	qDebug () << "added items to combobox";
	return ocnt > 1;
}

void	RadioInterface::setStreamOutSelector (int idx) {

	if (idx == 0)
	   return;

	outputDevice = outTable [idx];
	if (!our_audioSink -> isValidDevice (outputDevice)) 
	   return;

	our_audioSink	-> stop	();
	if (!our_audioSink -> selectDevice (outputDevice)) {
	   QMessageBox::warning (this, tr ("sdr"),
	                               tr ("Selecting  output stream failed\n"));
	   our_audioSink -> selectDefaultDevice ();
	   return;
	}

	qWarning () << "selected output device " << idx << outputDevice;
	our_audioSink	-> restart ();
}
//
//	the values for the different Modes:
void	RadioInterface::setModeParameters (int16_t Mode) {
	if (Mode == 2) {
	   dabModeParameters. dabMode	= 2;
	   dabModeParameters. L		= 76;		// blocks per frame
	   dabModeParameters. K		= 384;		// carriers
	   dabModeParameters. T_null	= 664;		// null length
	   dabModeParameters. T_F	= 49152;	// samples per frame
	   dabModeParameters. T_s	= 638;		// block length
	   dabModeParameters. T_u	= 512;		// useful part
	   dabModeParameters. guardLength	= 126;
	   dabModeParameters. carrierDiff	= 4000;
	} else
	if (Mode == 4) {
	   dabModeParameters. dabMode		= 4;
	   dabModeParameters. L			= 76;
	   dabModeParameters. K			= 768;
	   dabModeParameters. T_F		= 98304;
	   dabModeParameters. T_null		= 1328;
	   dabModeParameters. T_s		= 1276;
	   dabModeParameters. T_u		= 1024;
	   dabModeParameters. guardLength	= 252;
	   dabModeParameters. carrierDiff	= 2000;
	} else 
	if (Mode == 3) {
	   dabModeParameters. dabMode		= 3;
	   dabModeParameters. L			= 153;
	   dabModeParameters. K			= 192;
	   dabModeParameters. T_F		= 49152;
	   dabModeParameters. T_null		= 345;
	   dabModeParameters. T_s		= 319;
	   dabModeParameters. T_u		= 256;
	   dabModeParameters. guardLength	= 63;
	   dabModeParameters. carrierDiff	= 2000;
	} else {	// default = Mode I
	   dabModeParameters. dabMode		= 1;
	   dabModeParameters. L			= 76;
	   dabModeParameters. K			= 1536;
	   dabModeParameters. T_F		= 196608;
	   dabModeParameters. T_null		= 2656;
	   dabModeParameters. T_s		= 2552;
	   dabModeParameters. T_u		= 2048;
	   dabModeParameters. guardLength	= 504;
	   dabModeParameters. carrierDiff	= 1000;
	}
}

void	RadioInterface::setSynced	(char b) {
	if (isSynced == b)
	   return;

	isSynced = b;
	switch (isSynced) {
	   case SYNCED:
	      syncedLabel -> 
	               setStyleSheet ("QLabel {background-color : green}");
	      break;
	   case LONG_LOW:
	      syncedLabel -> 
	               setStyleSheet ("QLabel {background-color : yellow}");
	      break;

	   case LONG_HIGH:
	      syncedLabel ->
	               setStyleSheet ("QLabel {background-color : blue}");
	      break;

	   default:
	      syncedLabel ->
	               setStyleSheet ("QLabel {background-color : red}");
	      break;
	}
}

void	RadioInterface::set_avgTokenLengthDisplay (int k) {
//	fprintf (stderr, "Token length = %d\n", k);
}

void	RadioInterface::handleReset	(void) {
	the_ofdmProcessor	-> coarseCorrectorOn ();
	the_ofdmProcessor	-> reset ();
	clearEnsemble ();		// the display
}

void	RadioInterface::updateErrors (void) {
int32_t	tmp = 0;

}
