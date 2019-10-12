/********************************************************************************
** Form generated from reading UI file 'sdrgui.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SDRGUI_H
#define UI_SDRGUI_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpinBox>
#include "qwt_plot.h"
#include "qwt_scale_widget.h"
#include "qwt_text_label.h"

QT_BEGIN_NAMESPACE

class Ui_elektorSDR
{
public:
    QPushButton *inc_1K;
    QLCDNumber *scopeWidthDisplay;
    QPushButton *dec_1K;
    QSlider *spectrumAmplitudeSlider;
    QPushButton *startButton;
    QwtPlot *hfscope;
    QLabel *timeDisplay;
    QPushButton *QuitButton;
    QPushButton *spectrumShower;
    QLCDNumber *coarseCorrectorDisplay;
    QLCDNumber *sampleRateDisplay;
    QLCDNumber *fineCorrectorDisplay;
    QwtPlot *iqDisplay;
    QComboBox *deviceSelector;
    QSpinBox *generator_offset;
    QLCDNumber *avgTokenLengthDisplay;
    QLCDNumber *offsetDisplay;
    QListView *ensembleDisplay;
    QLabel *ensembleName;
    QLCDNumber *ensembleId;
    QLabel *dabMode;
    QSpinBox *channelNumber;
    QLabel *nameofLanguage;
    QLabel *programType;
    QSlider *scopeSlider;
    QPushButton *dumpButton;
    QLCDNumber *uepFlagDisplay;
    QLCDNumber *startAddrDisplay;
    QLCDNumber *LengthDisplay;
    QLCDNumber *protLevelDisplay;
    QLCDNumber *bitRateDisplay;
    QLCDNumber *ASCTyDisplay;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLCDNumber *errorDisplay;
    QLabel *programName;
    QLCDNumber *ficRatioDisplay;
    QLCDNumber *snrDisplay;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QComboBox *streamOutSelector;
    QPushButton *correctorReset;
    QLabel *versionName;
    QPushButton *audioDump;
    QPushButton *mp2fileButton;
    QPushButton *aacfileButton;
    QComboBox *bandSelector;
    QComboBox *channelSelector;
    QComboBox *modeSelector;
    QLabel *syncedLabel;

    void setupUi(QDialog *elektorSDR)
    {
        if (elektorSDR->objectName().isEmpty())
            elektorSDR->setObjectName(QString::fromUtf8("elektorSDR"));
        elektorSDR->resize(827, 503);
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Active, QPalette::Window, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush);
        elektorSDR->setPalette(palette);
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        elektorSDR->setFont(font);
        inc_1K = new QPushButton(elektorSDR);
        inc_1K->setObjectName(QString::fromUtf8("inc_1K"));
        inc_1K->setGeometry(QRect(500, 190, 51, 31));
        QFont font1;
        font1.setPointSize(8);
        inc_1K->setFont(font1);
        scopeWidthDisplay = new QLCDNumber(elektorSDR);
        scopeWidthDisplay->setObjectName(QString::fromUtf8("scopeWidthDisplay"));
        scopeWidthDisplay->setGeometry(QRect(370, 300, 51, 31));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush1(QColor(255, 255, 0, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        scopeWidthDisplay->setPalette(palette1);
        scopeWidthDisplay->setAutoFillBackground(false);
        scopeWidthDisplay->setFrameShape(QFrame::NoFrame);
        scopeWidthDisplay->setFrameShadow(QFrame::Raised);
        scopeWidthDisplay->setLineWidth(1);
        scopeWidthDisplay->setDigitCount(3);
        scopeWidthDisplay->setSegmentStyle(QLCDNumber::Flat);
        dec_1K = new QPushButton(elektorSDR);
        dec_1K->setObjectName(QString::fromUtf8("dec_1K"));
        dec_1K->setGeometry(QRect(450, 190, 51, 31));
        dec_1K->setFont(font1);
        spectrumAmplitudeSlider = new QSlider(elektorSDR);
        spectrumAmplitudeSlider->setObjectName(QString::fromUtf8("spectrumAmplitudeSlider"));
        spectrumAmplitudeSlider->setGeometry(QRect(410, 0, 20, 291));
        QPalette palette2;
        QBrush brush2(QColor(255, 85, 0, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Button, brush2);
        palette2.setBrush(QPalette::Inactive, QPalette::Button, brush2);
        palette2.setBrush(QPalette::Disabled, QPalette::Button, brush2);
        spectrumAmplitudeSlider->setPalette(palette2);
        spectrumAmplitudeSlider->setMinimum(10);
        spectrumAmplitudeSlider->setMaximum(100);
        spectrumAmplitudeSlider->setSliderPosition(50);
        spectrumAmplitudeSlider->setOrientation(Qt::Vertical);
        spectrumAmplitudeSlider->setTickPosition(QSlider::TicksBothSides);
        spectrumAmplitudeSlider->setTickInterval(20);
        startButton = new QPushButton(elektorSDR);
        startButton->setObjectName(QString::fromUtf8("startButton"));
        startButton->setGeometry(QRect(750, 220, 71, 71));
        QPalette palette3;
        palette3.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush3(QColor(255, 0, 0, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette3.setBrush(QPalette::Active, QPalette::Window, brush3);
        palette3.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette3.setBrush(QPalette::Inactive, QPalette::Window, brush3);
        palette3.setBrush(QPalette::Disabled, QPalette::Base, brush3);
        palette3.setBrush(QPalette::Disabled, QPalette::Window, brush3);
        startButton->setPalette(palette3);
        QFont font2;
        font2.setBold(true);
        font2.setWeight(75);
        startButton->setFont(font2);
        startButton->setAutoFillBackground(true);
        hfscope = new QwtPlot(elektorSDR);
        hfscope->setObjectName(QString::fromUtf8("hfscope"));
        hfscope->setGeometry(QRect(450, 0, 361, 191));
        hfscope->setAutoFillBackground(false);
        hfscope->setFrameShape(QFrame::Panel);
        hfscope->setFrameShadow(QFrame::Raised);
        hfscope->setLineWidth(3);
        timeDisplay = new QLabel(elektorSDR);
        timeDisplay->setObjectName(QString::fromUtf8("timeDisplay"));
        timeDisplay->setGeometry(QRect(650, 190, 161, 31));
        timeDisplay->setFrameShape(QFrame::Panel);
        timeDisplay->setFrameShadow(QFrame::Raised);
        timeDisplay->setLineWidth(2);
        QuitButton = new QPushButton(elektorSDR);
        QuitButton->setObjectName(QString::fromUtf8("QuitButton"));
        QuitButton->setGeometry(QRect(750, 290, 71, 61));
        QPalette palette4;
        palette4.setBrush(QPalette::Active, QPalette::Button, brush3);
        palette4.setBrush(QPalette::Inactive, QPalette::Button, brush3);
        palette4.setBrush(QPalette::Disabled, QPalette::Button, brush3);
        QuitButton->setPalette(palette4);
        QuitButton->setFont(font2);
        QuitButton->setAutoFillBackground(true);
        spectrumShower = new QPushButton(elektorSDR);
        spectrumShower->setObjectName(QString::fromUtf8("spectrumShower"));
        spectrumShower->setGeometry(QRect(550, 190, 101, 31));
        coarseCorrectorDisplay = new QLCDNumber(elektorSDR);
        coarseCorrectorDisplay->setObjectName(QString::fromUtf8("coarseCorrectorDisplay"));
        coarseCorrectorDisplay->setGeometry(QRect(290, 330, 41, 31));
        QPalette palette5;
        palette5.setBrush(QPalette::Active, QPalette::Base, brush);
        palette5.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette5.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette5.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette5.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette5.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        coarseCorrectorDisplay->setPalette(palette5);
        coarseCorrectorDisplay->setAutoFillBackground(false);
        coarseCorrectorDisplay->setFrameShape(QFrame::NoFrame);
        coarseCorrectorDisplay->setLineWidth(1);
        coarseCorrectorDisplay->setDigitCount(3);
        coarseCorrectorDisplay->setSegmentStyle(QLCDNumber::Flat);
        sampleRateDisplay = new QLCDNumber(elektorSDR);
        sampleRateDisplay->setObjectName(QString::fromUtf8("sampleRateDisplay"));
        sampleRateDisplay->setGeometry(QRect(420, 330, 81, 31));
        QPalette palette6;
        palette6.setBrush(QPalette::Active, QPalette::Base, brush);
        palette6.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette6.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette6.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette6.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette6.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        sampleRateDisplay->setPalette(palette6);
        sampleRateDisplay->setAutoFillBackground(false);
        sampleRateDisplay->setFrameShape(QFrame::NoFrame);
        sampleRateDisplay->setLineWidth(1);
        sampleRateDisplay->setDigitCount(7);
        sampleRateDisplay->setSegmentStyle(QLCDNumber::Flat);
        fineCorrectorDisplay = new QLCDNumber(elektorSDR);
        fineCorrectorDisplay->setObjectName(QString::fromUtf8("fineCorrectorDisplay"));
        fineCorrectorDisplay->setGeometry(QRect(280, 360, 61, 31));
        QPalette palette7;
        palette7.setBrush(QPalette::Active, QPalette::Base, brush);
        palette7.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette7.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette7.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette7.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette7.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        fineCorrectorDisplay->setPalette(palette7);
        fineCorrectorDisplay->setAutoFillBackground(false);
        fineCorrectorDisplay->setFrameShape(QFrame::NoFrame);
        fineCorrectorDisplay->setLineWidth(1);
        fineCorrectorDisplay->setDigitCount(3);
        fineCorrectorDisplay->setSegmentStyle(QLCDNumber::Flat);
        iqDisplay = new QwtPlot(elektorSDR);
        iqDisplay->setObjectName(QString::fromUtf8("iqDisplay"));
        iqDisplay->setGeometry(QRect(0, 0, 391, 291));
        deviceSelector = new QComboBox(elektorSDR);
        deviceSelector->setObjectName(QString::fromUtf8("deviceSelector"));
        deviceSelector->setGeometry(QRect(10, 330, 101, 31));
        generator_offset = new QSpinBox(elektorSDR);
        generator_offset->setObjectName(QString::fromUtf8("generator_offset"));
        generator_offset->setGeometry(QRect(450, 240, 61, 31));
        generator_offset->setMinimum(-60000);
        generator_offset->setMaximum(60000);
        generator_offset->setSingleStep(1);
        avgTokenLengthDisplay = new QLCDNumber(elektorSDR);
        avgTokenLengthDisplay->setObjectName(QString::fromUtf8("avgTokenLengthDisplay"));
        avgTokenLengthDisplay->setGeometry(QRect(340, 330, 71, 31));
        QPalette palette8;
        palette8.setBrush(QPalette::Active, QPalette::Base, brush);
        palette8.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette8.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette8.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette8.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette8.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        avgTokenLengthDisplay->setPalette(palette8);
        avgTokenLengthDisplay->setAutoFillBackground(false);
        avgTokenLengthDisplay->setFrameShape(QFrame::NoFrame);
        avgTokenLengthDisplay->setLineWidth(1);
        avgTokenLengthDisplay->setDigitCount(6);
        avgTokenLengthDisplay->setSegmentStyle(QLCDNumber::Flat);
        offsetDisplay = new QLCDNumber(elektorSDR);
        offsetDisplay->setObjectName(QString::fromUtf8("offsetDisplay"));
        offsetDisplay->setGeometry(QRect(450, 220, 61, 21));
        QPalette palette9;
        palette9.setBrush(QPalette::Active, QPalette::Base, brush);
        palette9.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette9.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette9.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette9.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette9.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        offsetDisplay->setPalette(palette9);
        offsetDisplay->setAutoFillBackground(false);
        offsetDisplay->setLineWidth(1);
        offsetDisplay->setSmallDecimalPoint(false);
        offsetDisplay->setDigitCount(6);
        offsetDisplay->setSegmentStyle(QLCDNumber::Flat);
        ensembleDisplay = new QListView(elektorSDR);
        ensembleDisplay->setObjectName(QString::fromUtf8("ensembleDisplay"));
        ensembleDisplay->setGeometry(QRect(570, 250, 171, 191));
        ensembleName = new QLabel(elektorSDR);
        ensembleName->setObjectName(QString::fromUtf8("ensembleName"));
        ensembleName->setGeometry(QRect(510, 220, 151, 21));
        QFont font3;
        font3.setPointSize(11);
        ensembleName->setFont(font3);
        ensembleId = new QLCDNumber(elektorSDR);
        ensembleId->setObjectName(QString::fromUtf8("ensembleId"));
        ensembleId->setGeometry(QRect(680, 220, 61, 31));
        ensembleId->setFrameShape(QFrame::NoFrame);
        ensembleId->setMode(QLCDNumber::Hex);
        ensembleId->setSegmentStyle(QLCDNumber::Flat);
        dabMode = new QLabel(elektorSDR);
        dabMode->setObjectName(QString::fromUtf8("dabMode"));
        dabMode->setGeometry(QRect(470, 410, 81, 31));
        QFont font4;
        font4.setPointSize(14);
        font4.setBold(true);
        font4.setWeight(75);
        dabMode->setFont(font4);
        channelNumber = new QSpinBox(elektorSDR);
        channelNumber->setObjectName(QString::fromUtf8("channelNumber"));
        channelNumber->setGeometry(QRect(510, 240, 51, 31));
        channelNumber->setMaximum(63);
        nameofLanguage = new QLabel(elektorSDR);
        nameofLanguage->setObjectName(QString::fromUtf8("nameofLanguage"));
        nameofLanguage->setGeometry(QRect(20, 410, 161, 31));
        QFont font5;
        font5.setPointSize(12);
        nameofLanguage->setFont(font5);
        programType = new QLabel(elektorSDR);
        programType->setObjectName(QString::fromUtf8("programType"));
        programType->setGeometry(QRect(200, 410, 131, 31));
        programType->setFont(font5);
        scopeSlider = new QSlider(elektorSDR);
        scopeSlider->setObjectName(QString::fromUtf8("scopeSlider"));
        scopeSlider->setGeometry(QRect(20, 300, 351, 20));
        scopeSlider->setMaximum(100);
        scopeSlider->setSliderPosition(30);
        scopeSlider->setOrientation(Qt::Horizontal);
        scopeSlider->setTickPosition(QSlider::TicksBelow);
        dumpButton = new QPushButton(elektorSDR);
        dumpButton->setObjectName(QString::fromUtf8("dumpButton"));
        dumpButton->setGeometry(QRect(750, 350, 71, 51));
        uepFlagDisplay = new QLCDNumber(elektorSDR);
        uepFlagDisplay->setObjectName(QString::fromUtf8("uepFlagDisplay"));
        uepFlagDisplay->setGeometry(QRect(20, 450, 64, 23));
        uepFlagDisplay->setSegmentStyle(QLCDNumber::Flat);
        startAddrDisplay = new QLCDNumber(elektorSDR);
        startAddrDisplay->setObjectName(QString::fromUtf8("startAddrDisplay"));
        startAddrDisplay->setGeometry(QRect(80, 450, 64, 23));
        startAddrDisplay->setSegmentStyle(QLCDNumber::Flat);
        LengthDisplay = new QLCDNumber(elektorSDR);
        LengthDisplay->setObjectName(QString::fromUtf8("LengthDisplay"));
        LengthDisplay->setGeometry(QRect(140, 450, 64, 23));
        LengthDisplay->setSegmentStyle(QLCDNumber::Flat);
        protLevelDisplay = new QLCDNumber(elektorSDR);
        protLevelDisplay->setObjectName(QString::fromUtf8("protLevelDisplay"));
        protLevelDisplay->setGeometry(QRect(200, 450, 64, 23));
        protLevelDisplay->setSegmentStyle(QLCDNumber::Flat);
        bitRateDisplay = new QLCDNumber(elektorSDR);
        bitRateDisplay->setObjectName(QString::fromUtf8("bitRateDisplay"));
        bitRateDisplay->setGeometry(QRect(260, 450, 64, 23));
        bitRateDisplay->setSegmentStyle(QLCDNumber::Flat);
        ASCTyDisplay = new QLCDNumber(elektorSDR);
        ASCTyDisplay->setObjectName(QString::fromUtf8("ASCTyDisplay"));
        ASCTyDisplay->setGeometry(QRect(340, 450, 64, 23));
        ASCTyDisplay->setSegmentStyle(QLCDNumber::Flat);
        label = new QLabel(elektorSDR);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 470, 67, 21));
        label_2 = new QLabel(elektorSDR);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(80, 470, 67, 21));
        label_3 = new QLabel(elektorSDR);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(150, 470, 67, 21));
        label_4 = new QLabel(elektorSDR);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(200, 470, 67, 21));
        label_5 = new QLabel(elektorSDR);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(270, 470, 67, 21));
        label_6 = new QLabel(elektorSDR);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(350, 470, 67, 21));
        errorDisplay = new QLCDNumber(elektorSDR);
        errorDisplay->setObjectName(QString::fromUtf8("errorDisplay"));
        errorDisplay->setGeometry(QRect(220, 360, 61, 31));
        errorDisplay->setFrameShape(QFrame::NoFrame);
        errorDisplay->setDigitCount(4);
        errorDisplay->setSegmentStyle(QLCDNumber::Flat);
        programName = new QLabel(elektorSDR);
        programName->setObjectName(QString::fromUtf8("programName"));
        programName->setGeometry(QRect(330, 410, 121, 31));
        programName->setFont(font5);
        ficRatioDisplay = new QLCDNumber(elektorSDR);
        ficRatioDisplay->setObjectName(QString::fromUtf8("ficRatioDisplay"));
        ficRatioDisplay->setGeometry(QRect(120, 360, 64, 31));
        ficRatioDisplay->setFrameShape(QFrame::NoFrame);
        ficRatioDisplay->setDigitCount(4);
        ficRatioDisplay->setSegmentStyle(QLCDNumber::Flat);
        snrDisplay = new QLCDNumber(elektorSDR);
        snrDisplay->setObjectName(QString::fromUtf8("snrDisplay"));
        snrDisplay->setGeometry(QRect(10, 360, 64, 31));
        snrDisplay->setFrameShape(QFrame::NoFrame);
        snrDisplay->setDigitCount(4);
        snrDisplay->setSegmentStyle(QLCDNumber::Flat);
        label_8 = new QLabel(elektorSDR);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(40, 390, 31, 21));
        label_9 = new QLabel(elektorSDR);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(140, 390, 51, 21));
        label_10 = new QLabel(elektorSDR);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(200, 390, 91, 21));
        streamOutSelector = new QComboBox(elektorSDR);
        streamOutSelector->setObjectName(QString::fromUtf8("streamOutSelector"));
        streamOutSelector->setGeometry(QRect(640, 450, 181, 31));
        correctorReset = new QPushButton(elektorSDR);
        correctorReset->setObjectName(QString::fromUtf8("correctorReset"));
        correctorReset->setGeometry(QRect(450, 300, 111, 31));
        QPalette palette10;
        QBrush brush4(QColor(85, 170, 255, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette10.setBrush(QPalette::Active, QPalette::Button, brush4);
        palette10.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush5(QColor(0, 0, 0, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette10.setBrush(QPalette::Active, QPalette::Window, brush5);
        palette10.setBrush(QPalette::Inactive, QPalette::Button, brush4);
        palette10.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette10.setBrush(QPalette::Inactive, QPalette::Window, brush5);
        palette10.setBrush(QPalette::Disabled, QPalette::Button, brush4);
        palette10.setBrush(QPalette::Disabled, QPalette::Base, brush5);
        palette10.setBrush(QPalette::Disabled, QPalette::Window, brush5);
        correctorReset->setPalette(palette10);
        correctorReset->setAutoFillBackground(true);
        versionName = new QLabel(elektorSDR);
        versionName->setObjectName(QString::fromUtf8("versionName"));
        versionName->setGeometry(QRect(386, 370, 171, 31));
        QFont font6;
        font6.setPointSize(12);
        font6.setBold(true);
        font6.setWeight(75);
        versionName->setFont(font6);
        audioDump = new QPushButton(elektorSDR);
        audioDump->setObjectName(QString::fromUtf8("audioDump"));
        audioDump->setGeometry(QRect(750, 400, 71, 51));
        QFont font7;
        font7.setPointSize(9);
        audioDump->setFont(font7);
        mp2fileButton = new QPushButton(elektorSDR);
        mp2fileButton->setObjectName(QString::fromUtf8("mp2fileButton"));
        mp2fileButton->setGeometry(QRect(450, 270, 61, 31));
        aacfileButton = new QPushButton(elektorSDR);
        aacfileButton->setObjectName(QString::fromUtf8("aacfileButton"));
        aacfileButton->setGeometry(QRect(510, 270, 51, 31));
        bandSelector = new QComboBox(elektorSDR);
        bandSelector->setObjectName(QString::fromUtf8("bandSelector"));
        bandSelector->setGeometry(QRect(150, 330, 91, 31));
        channelSelector = new QComboBox(elektorSDR);
        channelSelector->setObjectName(QString::fromUtf8("channelSelector"));
        channelSelector->setGeometry(QRect(240, 330, 51, 31));
        modeSelector = new QComboBox(elektorSDR);
        modeSelector->setObjectName(QString::fromUtf8("modeSelector"));
        modeSelector->setGeometry(QRect(110, 330, 41, 31));
        syncedLabel = new QLabel(elektorSDR);
        syncedLabel->setObjectName(QString::fromUtf8("syncedLabel"));
        syncedLabel->setGeometry(QRect(420, 450, 65, 21));
        QPalette palette11;
        palette11.setBrush(QPalette::Active, QPalette::Base, brush);
        palette11.setBrush(QPalette::Active, QPalette::Window, brush3);
        palette11.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette11.setBrush(QPalette::Inactive, QPalette::Window, brush3);
        palette11.setBrush(QPalette::Disabled, QPalette::Base, brush3);
        palette11.setBrush(QPalette::Disabled, QPalette::Window, brush3);
        syncedLabel->setPalette(palette11);
        syncedLabel->setAutoFillBackground(true);
        syncedLabel->setFrameShape(QFrame::Box);

        retranslateUi(elektorSDR);

        QMetaObject::connectSlotsByName(elektorSDR);
    } // setupUi

    void retranslateUi(QDialog *elektorSDR)
    {
        elektorSDR->setWindowTitle(QApplication::translate("elektorSDR", "sdr-j DAB/DAB+ receiver ", 0, QApplication::UnicodeUTF8));
        elektorSDR->setWindowIconText(QApplication::translate("elektorSDR", "QUIT", 0, QApplication::UnicodeUTF8));
        inc_1K->setText(QApplication::translate("elektorSDR", "+1K", 0, QApplication::UnicodeUTF8));
        dec_1K->setText(QApplication::translate("elektorSDR", "-1K", 0, QApplication::UnicodeUTF8));
        startButton->setText(QApplication::translate("elektorSDR", "START", 0, QApplication::UnicodeUTF8));
        timeDisplay->setText(QApplication::translate("elektorSDR", "TextLabel", 0, QApplication::UnicodeUTF8));
        QuitButton->setText(QApplication::translate("elektorSDR", "QUIT", 0, QApplication::UnicodeUTF8));
        spectrumShower->setText(QApplication::translate("elektorSDR", "spectrum", 0, QApplication::UnicodeUTF8));
        deviceSelector->clear();
        deviceSelector->insertItems(0, QStringList()
         << QApplication::translate("elektorSDR", "no device", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("elektorSDR", "file input (.raw)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("elektorSDR", "file input (.wav)", 0, QApplication::UnicodeUTF8)
        );
        ensembleName->setText(QString());
        dabMode->setText(QString());
        nameofLanguage->setText(QString());
        programType->setText(QString());
        dumpButton->setText(QApplication::translate("elektorSDR", "dump", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("elektorSDR", "uepFlag", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("elektorSDR", "start addr", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("elektorSDR", "length", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("elektorSDR", "prot level", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("elektorSDR", "bit rate", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("elektorSDR", "ASCTy", 0, QApplication::UnicodeUTF8));
        programName->setText(QString());
        label_8->setText(QApplication::translate("elektorSDR", "SNR", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("elektorSDR", "fic ratio", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("elektorSDR", "aac/mp2 ratio", 0, QApplication::UnicodeUTF8));
        streamOutSelector->clear();
        streamOutSelector->insertItems(0, QStringList()
         << QApplication::translate("elektorSDR", "select output", 0, QApplication::UnicodeUTF8)
        );
        correctorReset->setText(QApplication::translate("elektorSDR", "reset", 0, QApplication::UnicodeUTF8));
        versionName->setText(QString());
        audioDump->setText(QApplication::translate("elektorSDR", "audioDump", 0, QApplication::UnicodeUTF8));
        mp2fileButton->setText(QApplication::translate("elektorSDR", "MP2", 0, QApplication::UnicodeUTF8));
        aacfileButton->setText(QApplication::translate("elektorSDR", "AAC", 0, QApplication::UnicodeUTF8));
        bandSelector->clear();
        bandSelector->insertItems(0, QStringList()
         << QApplication::translate("elektorSDR", "BAND III", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("elektorSDR", "L BAND", 0, QApplication::UnicodeUTF8)
        );
        modeSelector->clear();
        modeSelector->insertItems(0, QStringList()
         << QApplication::translate("elektorSDR", "1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("elektorSDR", "2", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("elektorSDR", "4", 0, QApplication::UnicodeUTF8)
        );
        syncedLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class elektorSDR: public Ui_elektorSDR {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SDRGUI_H
