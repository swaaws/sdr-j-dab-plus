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
#include	<QFile>
#include	<QStringList>
#include	<QStringListModel>
#include	"dab-constants.h"
#include	"gui.h"
#include	"scope.h"
#include	"rawfiles.h"
#include	"wavfiles.h"
#include	"audiosink.h"
#include	"ofdm-processor-b.h"
#include	"fft.h"
#include	"dabstick.h"
#include	"sdrplay.h"
#include	"airspy.h"
#include	"mirics-dongle.h"
#ifdef HAVE_UHD
#include	"uhd-input.h"
#endif
#ifdef	HAVE_ELAD
#include	"elad-handler.h"
#endif

#define		BAND_III	0100
#define		L_BAND		0101
/*
 *	We use the creation function merely to set up the
 *	user interface and make the connections between the
 *	gui elements and the handling agents. All real action
 *	is embedded in actions, initiated by gui buttons
 */
	RadioInterface::RadioInterface (QSettings	*Si,
	                                int16_t		displaySize,
	                                int16_t		rastersize,
	                                bool		lightRun,
	                                QWidget		*parent): QDialog (parent) {
int16_t	i, k;

// 	the setup for the generated part of the ui
	setupUi (this);
	dabSettings		= Si;
	this	-> displaySize	= displaySize;
	this	-> rastersize	= rastersize;
	this	-> lightRun	= lightRun;
//
//	Before printing anything, we set
	setlocale (LC_ALL, "");
//	The default:
	myRig			= new virtualInput ();
	running			= false;
	
	this	-> displaySize	=
	            dabSettings -> value ("displaySize", displaySize). toInt ();
//	Note that displaySize should be a real power of 2
	if ((this -> displaySize & (this -> displaySize - 1)) != 0)
	   this -> displaySize = 512;

	displayBuffer		= new double [this -> displaySize];
	memset (displayBuffer, 0, this -> displaySize * sizeof (double));

	basicBuffers =
	           dabSettings -> value ("basicBuffers", basicBuffers). toInt ();
	ringbufferSize =
	           dabSettings -> value ("ringbufferSize", 1024). toInt ();
	averageCount	=
	           dabSettings -> value ("decay", 2). toInt ();

	autoCorrector =
	           dabSettings -> value ("autoCorrector", 1). toInt () == 1;
	spectrumSize		= 4 * displaySize;
	Window			= new double [spectrumSize];
	for (i = 0; i < spectrumSize; i ++) 
	   Window [i] =
	        0.42 - 0.5 * cos ((2.0 * M_PI * i) / (spectrumSize - 1)) +
	              0.08 * cos ((4.0 * M_PI * i) / (spectrumSize - 1));
	spectrum_fft		= new common_fft (spectrumSize);
	spectrumBuffer		= spectrum_fft -> getVector ();

//	Note that the generation of values to be displayed
//	is in a separate thread, we need a buffer for communication
//	and a signal/slot combination for triggering
	scopeBuffer		= new RingBuffer<DSPCOMPLEX>(2 *
	                                                 this -> spectrumSize);
	hfScope			= new Scope (hfscope,
	                                     this -> displaySize,
	                                     this -> rastersize);
	hfScope			-> SelectView (SPECTRUM_MODE);
	scopeAmplification	= 100;
	spectrumWaterfall	= false;

	isSynced		= UNSYNCED;
	syncedLabel		->
	               setStyleSheet ("QLabel {background-color : red}");
//
//	It is not a shower in the traditional sense
	connect (spectrumShower, SIGNAL (clicked ()),
	         this, SLOT (setSpectrumShower (void)));

	iqBuffer		= new RingBuffer<DSPCOMPLEX> (2 * 1536);
	iqDisplaysize	=
	               dabSettings -> value ("iqDisplaysize", 512). toInt ();
	myIQDisplay		= new IQDisplay (iqDisplay, iqDisplaysize);
//
//	'Concurrent' indicates whether the mp4/mp2 processing part
//	will be in a separate thread or not
	Concurrent		= true;
	TunedFrequency		= MHz (200);	// any value will do
	outRate			= 48000;
	outBuffer		=
	                    dabSettings -> value ("outBuffer", 32768). toInt ();
//
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

//	We will not bore our users with entries for which there is
//	no device
	deviceSelector -> addItem ("sdrplay");
	deviceSelector -> addItem ("dabstick");
	deviceSelector	-> addItem ("airspy");
	deviceSelector	-> addItem ("dongle");
#ifdef HAVE_UHD
	deviceSelector->addItem("UHD");
#endif
#ifdef HAVE_ELAD
	deviceSelector->addItem("elad s2");
#endif

	our_audioSink	-> selectDefaultDevice ();
//	Thanks to David Malone, who discovered while porting to OSX that
//	these initializations should NOT be forgotten.
	mp2File		= NULL;
	mp4File		= NULL;

	corrector	= dabSettings -> value ("faadcorrect", 1). toInt () == 0 ?
	                  false : true;
	setModeParameters (1);
	the_mscHandler		= new mscHandler	(this,
	                                                 our_audioSink,
	                                                 Concurrent,
	                                                 corrector);
	the_ficHandler		= new ficHandler	(this,
	                                                 the_mscHandler);

	the_ofdmProcessor = new ofdm_processor_b (myRig,
	                                          &dabModeParameters,
	                                          this,
	                                          the_mscHandler,
	                                          the_ficHandler,
	                                          spectrumSize,
	                                          scopeBuffer,
	                                          iqBuffer,
	                                          lightRun);
//
	ensemble.setStringList (Services);
	ensembleDisplay	-> setModel (&ensemble);
	Services << " ";
	ensemble. setStringList (Services);
	ensembleDisplay	-> setModel (&ensemble);

	dabBand		= BAND_III;
	setupChannels	(channelSelector, dabBand);
//
//	The connects of the GUI to the handlers
	connect (ensembleDisplay, SIGNAL (clicked (QModelIndex)),
	              this, SLOT (selectService (QModelIndex)));
	connect (channelNumber, SIGNAL (valueChanged (int)),
	              this, SLOT (selectChannel (int)));
	connect	(modeSelector, SIGNAL (activated (const QString &)),
	              this, SLOT (selectMode (const QString &)));
	connect (startButton, SIGNAL (clicked (void)),
	              this, SLOT (setStart (void)));
	connect (QuitButton, SIGNAL (clicked ()),
	              this, SLOT (TerminateProcess (void)));
	connect (deviceSelector, SIGNAL (activated (const QString &)),
	              this, SLOT (setDevice (const QString &)));
	connect (spectrumAmplitudeSlider, SIGNAL (valueChanged (int)),
	              this,  SLOT (setAmplification (int)));
	connect (streamOutSelector, SIGNAL (activated (int)),
	              this, SLOT (setStreamOutSelector (int)));
	connect (channelSelector, SIGNAL (activated (const QString &)),
	              this, SLOT (set_channelSelect (const QString &)));
	connect (bandSelector, SIGNAL (activated (const QString &)),
	              this, SLOT (set_bandSelect (const QString &)));
	connect (scopeSlider, SIGNAL (valueChanged (int) ),
		      this, SLOT (setScopeWidth (int) ) );
	connect (dumpButton, SIGNAL (clicked (void)),
	              this, SLOT (set_dumping (void)));
	connect (mp2fileButton, SIGNAL (clicked (void)),
	              this, SLOT (set_mp2File (void)));
	connect (aacfileButton, SIGNAL (clicked (void)),
	              this, SLOT (set_mp4File (void)));
	connect (audioDump, SIGNAL (clicked (void)),
	              this, SLOT (set_audioDump (void)));
	connect (correctorReset, SIGNAL (clicked (void)),
	              this, SLOT (autoCorrector_on (void)));
/*
 *	The increment and decrement buttons are limited to 
 *	a steps of 1 KHz
 */
	connect (dec_1K,  SIGNAL (clicked() ), this, SLOT (decT1000() ) );
	connect (inc_1K,  SIGNAL (clicked() ), this, SLOT (incT1000() ) );
//
//	Timers
	displayTimer		= new QTimer ();
	displayTimer		-> setInterval (1000);
	connect (displayTimer,
	         SIGNAL (timeout (void)),
	         this,
	         SLOT (updateTimeDisplay (void)));
//
	setAmplification	(spectrumAmplitudeSlider -> value ());
	setScopeWidth		(scopeSlider	 -> value ());
	FreqIncrement		= 0;
	sourceDumping		= false;
	dumpfilePointer		= NULL;
	audioDumping		= false;
	audiofilePointer	= NULL;
	sampleRateDisplay 	-> display (INPUT_RATE);
//
//	some settings may be influenced by the ini file
	setDevice 		(deviceSelector 	-> currentText ());
	QString h		=
	           dabSettings -> value ("device", "no device"). toString ();
	k		= deviceSelector -> findText (h);
	if (k != -1) {
	   deviceSelector	-> setCurrentIndex (k);
	   setDevice 		(deviceSelector 	-> currentText ());
	}
	h		= dabSettings -> value ("channel", "12C"). toString ();
	k		= channelSelector -> findText (h);
	if (k != -1) {
	   channelSelector -> setCurrentIndex (k);
	   set_channelSelect (h);
	}
	
//	display the version
	QString v = "sdr-j DAB(+)  " ;
	v. append (CURRENT_VERSION);
	versionName	-> setText (v. toLatin1 (). data ());
//	and start the timer
	displayTimer		-> start (1000);
}

	RadioInterface::~RadioInterface () {
	myRig		-> stopReader ();
	usleep (1000);
	delete		the_ofdmProcessor;
	delete		hfScope;
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

	s	-> setValue ("band", bandSelector -> currentText ());
	s	-> setValue ("channel",
	                      channelSelector -> currentText ());
	s	-> setValue ("device", deviceSelector -> currentText ());
	s	-> setValue ("Concurrent", Concurrent);
	s	-> setValue ("displaySize", displaySize);
	s	-> setValue ("decay", averageCount);
	s	-> setValue ("vfoOffset", vfoOffset);
	s	-> setValue ("iqDisplaysize", iqDisplaysize);
	s	-> setValue ("autoCorrector", autoCorrector ? 1 : 0);
	s	-> setValue ("basicBuffers", basicBuffers);
	s	-> setValue ("ringbufferSize", ringbufferSize);
}
//
//	On start, we ensure that the streams are stopped so
//	that they can be restarted again.
void	RadioInterface::setStart	(void) {
bool	r = 0;
	if (running)		// only listen when not running yet
	   return;
//
	r = myRig		-> restartReader ();
	qDebug ("Starting %d\n", r);
	if (!r) {
	   QMessageBox::warning (this, tr ("sdr"),
	                               tr ("Opening  input stream failed\n"));
	   return;
	}

	clearEnsemble ();		// the display
//
//	This does not hurt
	our_audioSink	-> restart ();
	running = true;
}

void	RadioInterface::TerminateProcess (void) {

	delete	displayTimer;
	if (sourceDumping) {
	   the_ofdmProcessor	-> stopDumping ();
	   sf_close (dumpfilePointer);
	}

	if (audioDumping) {
	   our_audioSink	-> stopDumping ();
	   sf_close (audiofilePointer);
	}

	if (mp2File != NULL) {
	   the_mscHandler	-> setFiles (NULL, NULL);
	   fclose (mp2File);
	}

	if (mp4File != NULL) {
	   the_mscHandler	-> setFiles (NULL, NULL);
	   fclose (mp4File);
	}

//	the_ofdmProcessor	-> stop ();
	dumpControlState (dabSettings);
	accept ();
	qDebug () <<  "Termination started";
	delete	the_ofdmProcessor;
	delete	myRig;
}

void	RadioInterface::abortSystem (int d) {
	qDebug ("aborting for reason %d\n", d);
	accept ();
}
//
void	RadioInterface::setAmplification (int n) {
	scopeAmplification	= 2 * n;
}
//
void	RadioInterface::setScopeWidth (int n) {
	scopeWidthDisplay	-> display (n);
	scopeWidth		= n;
}
//
void RadioInterface::decT1000 (void) {
	IncrementFrequency (-1000);
}

void RadioInterface::incT1000 (void) {
	IncrementFrequency (1000);
}

void	RadioInterface::IncrementFrequency (int32_t n) {

	FreqIncrement += n;
	offsetDisplay		-> display (FreqIncrement);
	the_ofdmProcessor	-> setOffset (FreqIncrement);
}

void	RadioInterface::setTuner (int32_t n) {
	myRig		-> setVFOFrequency (n);
	vfoFrequency	= n;
}

void	RadioInterface::updateTimeDisplay (void) {
QDateTime	currentTime = QDateTime::currentDateTime ();
	timeDisplay	-> setText (currentTime.
	                            toString (QString ("dd.MM.yy:hh:mm:ss")));
}
//
//	the wording is strange, it is not a shower as we know it,
//	it  shows rather than showers
void	RadioInterface::setSpectrumShower (void) {
	spectrumWaterfall	= !spectrumWaterfall;
	hfScope		-> SelectView (spectrumWaterfall ? WATERFALL_MODE :
	                                                   SPECTRUM_MODE);
}

void	RadioInterface::set_fineCorrectorDisplay (int v) {
	fineCorrectorDisplay	-> display (v);
}

void	RadioInterface::set_coarseCorrectorDisplay (int v) {
	coarseCorrectorDisplay	-> display (v);
}

void	RadioInterface::set_avgTokenLengthDisplay (int n) {
	avgTokenLengthDisplay -> display (n);
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

	if (band == BAND_III)
	   t = bandIII_frequencies;
	else
	   t = Lband_frequencies;

	for (i = 0; t [i]. key != NULL; i ++)
	   s -> insertItem (i, t [i]. key, QVariant (i));
}
//
void	RadioInterface::set_bandSelect (QString s) {
	if (running) {
	   running	= false;
	   myRig	-> stopReader ();
	   myRig	-> resetBuffer ();
	   our_audioSink	-> stop ();
	   usleep (100);
	   clearEnsemble ();
	}

	if (s == "BAND III")
	   dabBand	= BAND_III;
	else
	   dabBand	= L_BAND;
	setupChannels (channelSelector, dabBand);
}

void	RadioInterface::set_channelSelect (QString s) {
int16_t	i;
struct dabFrequencies *finger;
bool	localRunning	= running;

	if (localRunning) {
	   our_audioSink	-> stop ();
	   myRig		-> stopReader ();
	   myRig		-> resetBuffer ();
	}
	Services		= QStringList ();
	ensemble. setStringList (Services);
	ensembleDisplay		-> setModel (&ensemble);
	the_ficHandler		-> clearEnsemble ();
	TunedFrequency		= 0;
	if (dabBand == BAND_III)
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

	setTuner (TunedFrequency);
	if (localRunning) {
	   our_audioSink -> restart ();
	   myRig	 -> restartReader ();
	   running	 = true;
	}
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
	if (autoCorrector)
	   the_ofdmProcessor	-> coarseCorrectorOn ();
	the_ofdmProcessor	-> reset ();
//
//	Then the various displayed items
	programName		-> setText ("   ");
	ensembleName		-> setText ("   ");
	errorDisplay		-> display (0);
	ficRatioDisplay		-> display (0);
	snrDisplay		-> display (0);
	uepFlagDisplay		-> display (0);
	startAddrDisplay	-> display (0);
	LengthDisplay		-> display (0);
	protLevelDisplay	-> display (0);
	bitRateDisplay		-> display (0);
	ASCTyDisplay		-> display (0);
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
	programName	-> setText (a. toLatin1 (). data ());
	switch (dabModus) {
	   case DAB:	dabMode -> setText ("DAB");
	                break;

	   case DAB_PLUS:
	                dabMode -> setText ("DAB +");
	                break;

	   default:
	                dabMode -> setText ("No select");
	                break;
	}

	nameofLanguage	-> setText (get_programm_language_string (language));
	programType	-> setText (get_programm_type_string (type));
}

void	RadioInterface::selectChannel (int n) {
uint8_t	dabModus;

	the_ficHandler	-> unnamedService (n);
	programName	-> setText ("   ");
	dabModus	= the_mscHandler -> getMode ();
	switch (dabModus) {
	   case DAB:	dabMode -> setText ("DAB");
	                break;

	   case DAB_PLUS:
	                dabMode -> setText ("DAB +");
	                break;

	   default:
	                dabMode -> setText ("No select");
	                break;
	}
}

void	RadioInterface::set_dumping (void) {
SF_INFO *sf_info	= (SF_INFO *)alloca (sizeof (SF_INFO));

	if (!someStick (myRig -> myIdentity ()))
	   return;

	if (sourceDumping) {
	   the_ofdmProcessor	-> stopDumping ();
	   sf_close (dumpfilePointer);
	   sourceDumping = false;
	   dumpButton	-> setText ("dump");
	   return;
	}

	QString file = QFileDialog::getSaveFileName (this,
	                                     tr ("open file ..."),
	                                     QDir::homePath (),
	                                     tr ("raw data (*.sdr)"));
	file	= QDir::toNativeSeparators (file);
	sf_info	-> samplerate	= INPUT_RATE;
	sf_info	-> channels	= 2;
	sf_info	-> format	= SF_FORMAT_WAV | SF_FORMAT_PCM_24;

	dumpfilePointer	= sf_open (file. toLatin1 (). data (),
	                                   SFM_WRITE, sf_info);
	if (dumpfilePointer == NULL) {
	   qDebug () << "cannot open " << file. toLatin1 (). data ();
	   return;
	}
	dumpButton	-> setText ("writing");
	sourceDumping		= true;
	the_ofdmProcessor	-> startDumping (dumpfilePointer);
}

void	RadioInterface::set_mp2File (void) {

	if (mp2File != NULL) {
	   the_mscHandler	-> setFiles (NULL, mp4File);
	   fclose (mp2File);
	   mp2File	= NULL;
	   mp2fileButton	-> setText ("MP2");
	   return;
	}

	if (audioDumping) {
	   our_audioSink	-> stopDumping ();
	   sf_close (audiofilePointer);
	   audioDumping = false;
	   audioDump	-> setText ("audioDump");
	   return;
	}

	QString file = QFileDialog::getSaveFileName (this,
	                                     tr ("open file ..."),
	                                     QDir::homePath (),
	                                     tr ("mp2 data (*.mp2)"));
	file	= QDir::toNativeSeparators (file);
	mp2File	= fopen (file. toLatin1 (). data (), "w");
	if (mp2File == NULL) {
	   qDebug () << "cannot open " << file. toLatin1 (). data ();
	   return;
	}
	the_mscHandler	-> setFiles (mp2File, mp4File);
	mp2fileButton	-> setText ("writing");
}

void	RadioInterface::set_mp4File (void) {

	if (mp4File != NULL) {
	   the_mscHandler	-> setFiles (mp2File, NULL);
	   fclose (mp4File);
	   mp4File	= NULL;
	   aacfileButton	-> setText ("MP4");
	   return;
	}

	if (audioDumping) {
	   our_audioSink	-> stopDumping ();
	   sf_close (audiofilePointer);
	   audioDumping = false;
	   audioDump	-> setText ("audioDump");
	   return;
	}

	QString file = QFileDialog::getSaveFileName (this,
	                                     tr ("open file ..."),
	                                     QDir::homePath (),
	                                     tr ("aac data (*.aac)"));
	file	= QDir::toNativeSeparators (file);
	mp4File	= fopen (file. toLatin1 (). data (), "w");
	if (mp4File == NULL) {
	   qDebug () << "cannot open " << file. toLatin1 (). data ();
	   return;
	}

	the_mscHandler	-> setFiles (mp2File, mp4File);
	aacfileButton	-> setText ("writing");
}

void	RadioInterface::set_audioDump (void) {
SF_INFO	*sf_info	= (SF_INFO *)alloca (sizeof (SF_INFO));
	if (!someStick (myRig -> myIdentity ()))
	   return;

	if (!audioDumping && (mp2File != NULL || mp4File != NULL))
	   return;
	
	if (audioDumping) {
	   our_audioSink	-> stopDumping ();
	   sf_close (audiofilePointer);
	   audioDumping = false;
	   audioDump	-> setText ("audioDump");
	   return;
	}

	QString file = QFileDialog::getSaveFileName (this,
	                                        tr ("open file .."),
	                                        QDir::homePath (),
	                                        tr ("Sound (*.wav)"));
	file		= QDir::toNativeSeparators (file);
	sf_info		-> samplerate	= 48000;
	sf_info		-> channels	= 2;
	sf_info		-> format	= SF_FORMAT_WAV | SF_FORMAT_PCM_16;

	audiofilePointer	= sf_open (file. toLatin1 (). data (),
	                                   SFM_WRITE, sf_info);
	if (audiofilePointer == NULL) {
	   qDebug () << "Cannot open " << file. toLatin1 (). data ();
	   return;
	}

	audioDump		-> setText ("WRITING");
	audioDumping		= true;
	our_audioSink		-> startDumping (audiofilePointer);
}


void	RadioInterface::channelData (int subchId,
	                             int uepFlag,
	                             int startAddr,
	                             int Length,
	                             int protLevel,
	                             int bitRate,
	                             int ASCTy) {
	(void)subchId;
	uepFlagDisplay		-> display (uepFlag);
	startAddrDisplay	-> display (startAddr);
	LengthDisplay		-> display (Length);
	protLevelDisplay	-> display (protLevel);
	bitRateDisplay		-> display (bitRate);
	ASCTyDisplay		-> display (ASCTy);
}

void	RadioInterface::show_successRate (int s) {
	errorDisplay	-> display (s);
}

void	RadioInterface::show_ficRatio (int s) {
	ficRatioDisplay	-> display (s);
}

void	RadioInterface::show_snr (int s) {
	snrDisplay	-> display (s);
}
//
//	setDevice is called trough a signal from the gui
//	Operation is in three steps: 
//	- first dumping of any kind is stopped
//	- second the previously loaded device is stopped
//	- third, the new device is initiated
void	RadioInterface::setDevice (QString s) {
bool	success;
QString	file;
//
//	first stop dumping
	if (sourceDumping) {
	   the_ofdmProcessor -> stopDumping ();
	   sf_close (dumpfilePointer);
	   sourceDumping = false;
	   dumpButton	-> setText ("dump");
	}

	if (audioDumping) {
	   our_audioSink	-> stopDumping ();
	   sf_close (audiofilePointer);
	   audioDumping	= false;
	   audioDump -> setText ("audioDump");
	}

	running	= false;
	our_audioSink	-> stop ();
//
//
//	select. For all it holds that:
	myRig	-> stopReader ();
	delete	the_ofdmProcessor;
	delete	myRig;
//
	if (s == "sdrplay") {
	   myRig	= new sdrplay (dabSettings, &success);
	   if (!success) {
	      delete myRig;
	      QMessageBox::warning (this, tr ("sdr"),
	                               tr ("Mirics device: no library\n"));
	      myRig = new virtualInput ();
	      resetSelector ();
	   }
	   else 
	      set_channelSelect	(channelSelector -> currentText ());
	}
	else
	if (s == "dabstick") {
	   myRig	= new dabStick (dabSettings, &success);
	   if (!success) {
	      delete myRig;
	      QMessageBox::warning (this, tr ("sdr"),
	                               tr ("Dabstick: no luck\n"));
	      myRig = new virtualInput ();
	      resetSelector ();
	   }
	   else 
	      set_channelSelect	(channelSelector -> currentText ());
	}
	else
	if (s == "airspy") {
	   myRig	= new airspyHandler (dabSettings, &success);
	   if (!success) {
	      delete myRig;
	      QMessageBox::warning (this, tr ("sdr"),
	                               tr ("airspy: no luck\n"));
	      myRig = new virtualInput ();
	      resetSelector ();
	   }
	   else 
	      set_channelSelect	(channelSelector -> currentText ());
	}
	else
	if (s == "dongle") {
	   myRig	= new miricsDongle (dabSettings, &success);
	   if (!success) {
	      delete myRig;
	      QMessageBox::warning (this, tr ("sdr"),
	                               tr ("miricsDongle: no luck\n"));
	      myRig = new virtualInput ();
	      resetSelector ();
	   }
	   else 
	      set_channelSelect	(channelSelector -> currentText ());
	}
//
//	UHD is - at least in its current setting - for Linux
#ifdef HAVE_UHD
	else
	if (s == "UHD") {
	   myRig = new uhdInput (dabSettings, &success );
	   if (!success) {
	      delete myRig;
	      QMessageBox::warning( this, tr ("sdr"), tr ("UHD: no luck\n") );
	      myRig = new virtualInput();
	      resetSelector ();
	   }
	   else 
	      set_channelSelect( channelSelector->currentText() );
	}
#endif
//	Elad is - in its current settings - for Windows, it is a
//	wrap around the dll
#ifdef HAVE_ELAD
	else
	if (s == "elad s2") {
	   myRig = new eladHandler (dabSettings, &success );
	   if (!success) {
	      delete myRig;
	      QMessageBox::warning( this, tr ("sdr"), tr ("elad: no luck\n") );
	      myRig = new virtualInput();
	      resetSelector ();
	   }
	   else 
	      set_channelSelect( channelSelector->currentText() );
	}
#endif
	else
	if (s == "file input (.raw)") {
	   file		= QFileDialog::getOpenFileName (this,
	                                                tr ("open file ..."),
	                                                QDir::homePath (),
	                                                tr ("raw data (*.raw)"));
	   file		= QDir::toNativeSeparators (file);
	   myRig	= new rawFiles (file, &success);
	   if (!success) {
	      delete myRig;
	      myRig = new virtualInput ();
	      resetSelector ();
	   }
	}
	else
	if (s == "file input (.wav)") {
	   file		= QFileDialog::getOpenFileName (this,
	                                                tr ("open file ..."),
	                                                QDir::homePath (),
	                                                tr ("raw data (*.wav)"));
	   file		= QDir::toNativeSeparators (file);
	   myRig	= new wavFiles (file, &success);
	   if (!success) {
	      delete myRig;
	      myRig = new virtualInput ();
	      resetSelector ();
	   }
	}
	else {	// s == "no device" 
	   myRig	= new virtualInput ();
	}

	hfScope		-> setBitDepth (myRig -> bitDepth ());
	the_ofdmProcessor	= new ofdm_processor_b (myRig,
	                                                &dabModeParameters,
	                                                this,
	                                                the_mscHandler,
	                                                the_ficHandler,
	                                                spectrumSize,
	                                                scopeBuffer,
	                                                iqBuffer,
	                                                lightRun);
}
//
//
//	In case selection of a device did not work out for whatever
//	reason, the device selector is reset to "no device"
//	Qt will trigger on the chgange of value in the deviceSelector
//	which will cause selectdevice to be called again (while we
//	are in the middle, so we first disconnect the selector
//	from the slot. Obviously, after setting the index of
//	the device selector, we connect again

void	RadioInterface::resetSelector (void) {
	disconnect (deviceSelector, SIGNAL (activated (const QString &)),
	            this, SLOT (setDevice (const QString &)));
int	k	= deviceSelector -> findText (QString ("no device"));
	if (k != -1) { 		// should not happen
	   deviceSelector -> setCurrentIndex (k);
	}
	connect (deviceSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (setDevice (const QString &)));
}

//
//	showScope is called through the signal mechanism
//	It is called whenever the ofdm_processor has
//	filled the buffer with displaySize elements. Just
//	for verification purposes, we pass the amount of data passed
//
void	RadioInterface::showScope (int amount) {
double	*X_axis 	= (double *)alloca (displaySize * sizeof (double));
double	*Y_values	= (double *)alloca (displaySize * sizeof (double));
int16_t	i, j;
double	temp	= (double)INPUT_RATE / 2 / displaySize;
//simple check
	if ((amount > spectrumSize) || (amount < 0)) {	// should not happen
	   amount = spectrumSize;
	   fprintf (stderr, "H E L P\n");
	   return;
	}
//
//	first X axis labels
	for (i = 0; i < displaySize; i ++)
	   X_axis [i] = 
	      ((double)vfoFrequency - (double)(INPUT_RATE / 2) +
	        (double)((i) * (double) 2 * temp)) / ((double)1000);
//
//	get the buffer data
	memset (spectrumBuffer, 0, spectrumSize * sizeof (DSPCOMPLEX));
	scopeBuffer -> getDataFromBuffer (spectrumBuffer, amount);
//	and window it
	for (i = 0; i < amount; i ++)
	   spectrumBuffer [i] = cmul (spectrumBuffer [i], Window [i]);

	spectrum_fft	-> do_FFT ();
//
//	and map the spectrumSize values onto displaySize elements
	for (i = 0; i < displaySize / 2; i ++) {
	   double f	= 0;
	   for (j = 0; j < spectrumSize / displaySize; j ++)
	      f += abs (spectrumBuffer [spectrumSize / displaySize * i + j]);
	   Y_values [displaySize / 2 + i] = 
                 f / (spectrumSize / displaySize);
	   f = 0;
	   for (j = 0; j < spectrumSize / displaySize; j ++)
	      f += abs (spectrumBuffer [spectrumSize / 2 +
	                          spectrumSize / displaySize * i + j]);
	   Y_values [i] =
	         f / (spectrumSize / displaySize);
	}
//
//	average the image a little.
	for (i = 0; i < displaySize; i ++)
	   displayBuffer [i] = 
	             (double)(averageCount - 1) /averageCount * Y_values [i] +
	              1.0f / averageCount * displayBuffer [i];

	memcpy (Y_values, displayBuffer, displaySize * sizeof (double));
	hfScope -> Display (X_axis, Y_values,
	                    scopeAmplification,
	                    vfoFrequency / 1000);
}
//
//	signal, received from ofdm_decoder that a buffer is filled
//	with amount values ready for display
void	RadioInterface::showIQ	(int amount) {
DSPCOMPLEX *Values	= (DSPCOMPLEX *)alloca (amount * sizeof (DSPCOMPLEX));
int16_t	i;
int16_t	t;
double	sum	= 0;

	t = iqBuffer -> getDataFromBuffer (Values, amount);
	for (i = 0; i < t; i ++)
	   sum += abs (Values [i]);
	sum	/= t;

	for (i = 0; i < t; i ++) 
	   myIQDisplay -> DisplayIQ (Values [i], scopeWidth / sum);
//	   myIQDisplay -> DisplayIQ (Values [i], scopeWidth * 0.0000005);
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
}
//
//	This is a copy of the clearEnsemble, with as difference
//	that the autoCorrector is ON. We then need clean settings
void	RadioInterface::autoCorrector_on (void) {
//	first the real stuff
	Services		= QStringList ();
	ensemble. setStringList (Services);
	ensembleDisplay		-> setModel (&ensemble);
	the_ficHandler		-> clearEnsemble ();
	the_ofdmProcessor	-> coarseCorrectorOn ();
	the_ofdmProcessor	-> reset ();
//
//	Then the various displayed items
	programName		-> setText ("   ");
	ensembleName		-> setText ("   ");
	errorDisplay		-> display (0);
	ficRatioDisplay		-> display (0);
	snrDisplay		-> display (0);
	uepFlagDisplay		-> display (0);
	startAddrDisplay	-> display (0);
	LengthDisplay		-> display (0);
	protLevelDisplay	-> display (0);
	bitRateDisplay		-> display (0);
	ASCTyDisplay		-> display (0);
}
//
//	When selecting another mode, first ensure that all kinds
//	of dumping are stopped, or just stop them
void	RadioInterface::selectMode (const QString &s) {
uint8_t	Mode	= s. toInt ();

	if (sourceDumping) {
	   the_ofdmProcessor -> stopDumping ();
	   sf_close (dumpfilePointer);
	   sourceDumping = false;
	   dumpButton	-> setText ("dump");
	}

	if (audioDumping) {
	   our_audioSink	-> stopDumping ();
	   sf_close (audiofilePointer);
	   audioDumping	= false;
	   audioDump -> setText ("audioDump");
	}

	running	= false;
	our_audioSink		-> stop ();
	myRig			-> stopReader ();
	the_ofdmProcessor	-> stop ();
//
//	we have to create a new ofdmprocessor with the correct
//	settings of the parameters.
	delete 	the_ofdmProcessor;
	setModeParameters (Mode);
	the_ficHandler		-> setBitsperBlock	(2 * dabModeParameters. K);
	the_mscHandler		-> setMode		(&dabModeParameters);
	the_ofdmProcessor	= new ofdm_processor_b (myRig,
	                                                &dabModeParameters,
	                                                this,
	                                                the_mscHandler,
	                                                the_ficHandler,
	                                                spectrumSize,
	                                                scopeBuffer,
	                                                iqBuffer,
	                                                lightRun);
//	and wait for setStart
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

	   default:
	      syncedLabel ->
	               setStyleSheet ("QLabel {background-color : red}");
	      break;
	}
}


