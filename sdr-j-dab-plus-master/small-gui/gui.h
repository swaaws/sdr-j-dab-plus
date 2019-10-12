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
#include	<QTimer>
#include	<QStringList>
#include	<QStringListModel>
#include	"ofdm-processor.h"
#include	"ringbuffer.h"
#include	"dab-constants.h"
#include	"virtual-input.h"

class	QSettings;
class	audioSink;
class	mscHandler;
class	ficHandler;
/*
 *	GThe main gui object. It inherits from
 *	QDialog and the generated form
 */
class RadioInterface: public QDialog,
		      private Ui_elektorSDR {
Q_OBJECT
public:
		RadioInterface		(QSettings	*,
	                                 const char	*,
	                                 const char	*theBand = "BAND III",
	                                 const char	*theMode = "Mode 1",
	                                 bool		showErrors = false,
	                                 QWidget *parent = NULL);
		~RadioInterface		(void);

private:
	QSettings	*dabSettings;
	bool		showErrors;
	void		dumpControlState	(QSettings *);
	int16_t		outputDevice;
	DabParams	dabModeParameters;
	void		setModeParameters	(int16_t);
	int32_t		vfoFrequency;
	QStringListModel	ensemble;
	QStringList	Services;
	bool		running;
	char		isSynced;
	bool		corrector;
	int32_t		outRate;
	int32_t		outBuffer;
	RingBuffer<DSPCOMPLEX> *scopeBuffer;
	RingBuffer<DSPCOMPLEX> *iqBuffer;

	void		setupChannels	(QComboBox *, uint8_t);
	QString		dabBand;
	QString		dabMode;
	QString		dabDevice;
	uint8_t		theProcessor;
	int16_t		basicBuffers;
	int32_t		ringbufferSize;
	char		ensembleLabel [32];
	virtualInput	*myRig;
	int16_t		*outTable;
	int16_t		numberofDevices;

	void		setTuner		(int32_t);

	void		stop_lcdTimer		(void);
	int16_t		CurrentRig;
	QTimer		*displayTimer;
	bool		setupSoundOut		(QComboBox *, audioSink *,
	                                         int32_t, int16_t *);
	ofdm_processor	*the_ofdmProcessor;
	ficHandler	*the_ficHandler;
	mscHandler	*the_mscHandler;
	audioSink	*our_audioSink;
	int32_t		TunedFrequency;
	int16_t		currentGain;
const	char		*get_programm_type_string (uint8_t);
const	char		*get_programm_language_string (uint8_t);
	void		updateErrors	(void);
private slots:
	void	updateTimeDisplay	(void);
	void	setStreamOutSelector	(int);

	void	handleReset		(void);
	void	abortSystem		(int);
	void	TerminateProcess	(void);
	void	set_channelSelect	(QString);
	void	selectService		(QModelIndex);
	void	setStart		(void);
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

