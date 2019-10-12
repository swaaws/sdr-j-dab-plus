#
/*
 *    Copyright (C)  2010, 2011, 2012
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

#ifndef _GUI
#define _GUI

#include	<QDialog>
#include	"ui_sdrgui.h"
#include	<qwt.h>
#include	<qwt_slider.h>
#include	<qwt_plot.h>
#include	<qwt_plot_curve.h>
#include	<qwt_plot_marker.h>
#include	<qwt_plot_grid.h>
#include	<qwt_dial.h>
#include	<qwt_dial_needle.h>
#include	<qwt_plot_spectrogram.h>
#include	<QTimer>
#include	<sndfile.h>
#include	<QWheelEvent>
#include	<QStringList>
#include	<QStringListModel>
#include	"scope.h"
#include	"ofdm-processor.h"
#include	"iqdisplay.h"
#include	"ringbuffer.h"
#include	"dab-constants.h"

class	QSettings;
class	Scope;
class	virtualInput;
class	audioSink;

class	mscHandler;
class	ficHandler;

class	common_fft;

#define	HFSPECTRUM	0200
#define	LFSPECTRUM	0201
/*
 *	GThe main gui object. It inherits from
 *	QDialog and the generated form
 */
class RadioInterface: public QDialog,
		      private Ui_elektorSDR {
Q_OBJECT
public:
		RadioInterface		(QSettings	*,
	                                 int16_t,		// displaysize
	                                 int16_t,		// rastersize
	                                 bool,			// lightrun
	                                 QWidget *parent = NULL);
		~RadioInterface		();

private:
	int16_t		outputDevice;
	void		dumpControlState	(QSettings *);
	bool		sourceDumping;
	SNDFILE		*dumpfilePointer;
	bool		audioDumping;
	SNDFILE		*audiofilePointer;
	bool		lightRun;
	uint8_t		Concurrent;
	DabParams	dabModeParameters;
	void		setModeParameters	(int16_t);
	int32_t		vfoFrequency;
	int32_t		vfoOffset;
	QSettings	*dabSettings;
	QStringListModel	ensemble;
	QStringList	Services;

	char		isSynced;
	bool		corrector;
	int32_t		outRate;
	int32_t		outBuffer;
	Scope		*hfScope;
	RingBuffer<DSPCOMPLEX>	*scopeBuffer;
	int32_t		iqDisplaysize;
	IQDisplay	*myIQDisplay;
	RingBuffer<DSPCOMPLEX> *iqBuffer;

	void		setupChannels	(QComboBox *, uint8_t);
	uint8_t		dabBand;
	uint8_t		theProcessor;
	int16_t		basicBuffers;
	int32_t		ringbufferSize;
	bool		running;
	int16_t		displaySize;
	int16_t		spectrumSize;
	double		*displayBuffer;
	int16_t		averageCount;
	double		*Window;
	common_fft	*spectrum_fft;
	DSPCOMPLEX	*spectrumBuffer;
	int16_t		rastersize;

	char		ensembleLabel [32];
	virtualInput	*myRig;
	int16_t		*outTable;
	int16_t		numberofDevices;

	int16_t		currAttSliderValue;
	DSPFLOAT	omega;
	void		setTuner		(int32_t);
	QTimer		*sampleTimer;

	void		stop_lcdTimer		(void);
	int32_t		Panel;
	int16_t		CurrentRig;
	QTimer		*displayTimer;
	void		IncrementFrequency	(int32_t);

	bool		setupSoundOut		(QComboBox *, audioSink *,
	                                         int32_t, int16_t *);
	void		resetSelector		(void);
	int32_t		sampleCount;
	bool		spectrumWaterfall;
	ofdm_processor	*the_ofdmProcessor;
	ficHandler	*the_ficHandler;
	mscHandler	*the_mscHandler;
	audioSink	*our_audioSink;
	int32_t		FreqIncrement;
	int32_t		TunedFrequency;
	int16_t		scopeAmplification;
	bool		autoCorrector;
	FILE		*mp2File;
	FILE		*mp4File;
	int16_t		scopeWidth;
	int16_t		currentGain;
const	char		*get_programm_type_string (uint8_t);
const	char		*get_programm_language_string (uint8_t);
private slots:
	void	setStart		(void);
	void	updateTimeDisplay	(void);
	void	setStreamOutSelector	(int);
	void	setScopeWidth		(int);

	void	selectMode		(const QString &);
	void	autoCorrector_on	(void);
	void	setSpectrumShower	(void);
	void	setAmplification	(int);
	void	decT1000		(void);
	void	incT1000		(void);

	void	abortSystem		(int);
	void	TerminateProcess	(void);
	void	set_bandSelect		(QString);
	void	set_channelSelect	(QString);
	void	setDevice		(QString);
	void	selectService		(QModelIndex);
	void	selectChannel		(int);
	void	set_dumping		(void);
	void	set_mp2File		(void);
	void	set_mp4File		(void);
	void	set_audioDump		(void);
public slots:
	void	set_fineCorrectorDisplay	(int);
	void	set_coarseCorrectorDisplay	(int);
	void	set_avgTokenLengthDisplay	(int);
	void	clearEnsemble		(void);
	void	addtoEnsemble		(const QString &);
	void	nameofEnsemble		(int, char *);
	void	addEnsembleChar		(char, int);
	void	channelData		(int, int, int, int, int, int, int);
	void	show_successRate	(int);
	void	show_ficRatio		(int);
	void	show_snr		(int);
	void	showScope		(int);
	void	showIQ			(int);
	void	setSynced		(char);
};

#endif

