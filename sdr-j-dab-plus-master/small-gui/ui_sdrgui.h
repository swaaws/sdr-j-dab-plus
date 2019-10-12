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
#include "qwt_plot.h"
#include "qwt_scale_widget.h"
#include "qwt_text_label.h"

QT_BEGIN_NAMESPACE

class Ui_elektorSDR
{
public:
    QPushButton *QuitButton;
    QListView *ensembleDisplay;
    QLabel *ensembleName;
    QLCDNumber *ensembleId;
    QPushButton *resetButton;
    QComboBox *channelSelector;
    QLabel *syncedLabel;
    QLabel *versionName;
    QComboBox *streamOutSelector;
    QLabel *timeDisplay;
    QLCDNumber *soundDisplay;
    QLCDNumber *snrDisplay;

    void setupUi(QDialog *elektorSDR)
    {
        if (elektorSDR->objectName().isEmpty())
            elektorSDR->setObjectName(QString::fromUtf8("elektorSDR"));
        elektorSDR->resize(656, 98);
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
        QuitButton = new QPushButton(elektorSDR);
        QuitButton->setObjectName(QString::fromUtf8("QuitButton"));
        QuitButton->setGeometry(QRect(560, 50, 81, 41));
        QPalette palette1;
        QBrush brush1(QColor(255, 0, 0, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        QuitButton->setPalette(palette1);
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        QuitButton->setFont(font1);
        QuitButton->setAutoFillBackground(true);
        ensembleDisplay = new QListView(elektorSDR);
        ensembleDisplay->setObjectName(QString::fromUtf8("ensembleDisplay"));
        ensembleDisplay->setGeometry(QRect(370, 30, 171, 51));
        ensembleName = new QLabel(elektorSDR);
        ensembleName->setObjectName(QString::fromUtf8("ensembleName"));
        ensembleName->setGeometry(QRect(150, 50, 151, 31));
        QFont font2;
        font2.setPointSize(11);
        ensembleName->setFont(font2);
        ensembleName->setFrameShape(QFrame::Panel);
        ensembleId = new QLCDNumber(elektorSDR);
        ensembleId->setObjectName(QString::fromUtf8("ensembleId"));
        ensembleId->setGeometry(QRect(310, 50, 61, 31));
        ensembleId->setFrameShape(QFrame::Box);
        ensembleId->setMode(QLCDNumber::Hex);
        ensembleId->setSegmentStyle(QLCDNumber::Flat);
        resetButton = new QPushButton(elektorSDR);
        resetButton->setObjectName(QString::fromUtf8("resetButton"));
        resetButton->setGeometry(QRect(560, 10, 81, 41));
        QPalette palette2;
        QBrush brush2(QColor(85, 170, 255, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Button, brush2);
        palette2.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush3(QColor(0, 0, 0, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Window, brush3);
        palette2.setBrush(QPalette::Inactive, QPalette::Button, brush2);
        palette2.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Window, brush3);
        palette2.setBrush(QPalette::Disabled, QPalette::Button, brush2);
        palette2.setBrush(QPalette::Disabled, QPalette::Base, brush3);
        palette2.setBrush(QPalette::Disabled, QPalette::Window, brush3);
        resetButton->setPalette(palette2);
        resetButton->setAutoFillBackground(true);
        channelSelector = new QComboBox(elektorSDR);
        channelSelector->setObjectName(QString::fromUtf8("channelSelector"));
        channelSelector->setGeometry(QRect(20, 50, 51, 31));
        syncedLabel = new QLabel(elektorSDR);
        syncedLabel->setObjectName(QString::fromUtf8("syncedLabel"));
        syncedLabel->setGeometry(QRect(80, 50, 65, 31));
        QPalette palette3;
        palette3.setBrush(QPalette::Active, QPalette::Base, brush);
        palette3.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette3.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette3.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette3.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette3.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        syncedLabel->setPalette(palette3);
        syncedLabel->setAutoFillBackground(true);
        syncedLabel->setFrameShape(QFrame::Box);
        versionName = new QLabel(elektorSDR);
        versionName->setObjectName(QString::fromUtf8("versionName"));
        versionName->setGeometry(QRect(20, 30, 201, 21));
        streamOutSelector = new QComboBox(elektorSDR);
        streamOutSelector->setObjectName(QString::fromUtf8("streamOutSelector"));
        streamOutSelector->setGeometry(QRect(190, 10, 171, 29));
        timeDisplay = new QLabel(elektorSDR);
        timeDisplay->setObjectName(QString::fromUtf8("timeDisplay"));
        timeDisplay->setGeometry(QRect(400, 10, 181, 20));
        soundDisplay = new QLCDNumber(elektorSDR);
        soundDisplay->setObjectName(QString::fromUtf8("soundDisplay"));
        soundDisplay->setGeometry(QRect(120, 10, 64, 23));
        soundDisplay->setFrameShape(QFrame::NoFrame);
        soundDisplay->setNumDigits(4);
        soundDisplay->setSegmentStyle(QLCDNumber::Flat);
        snrDisplay = new QLCDNumber(elektorSDR);
        snrDisplay->setObjectName(QString::fromUtf8("snrDisplay"));
        snrDisplay->setGeometry(QRect(20, 10, 64, 23));
        snrDisplay->setFrameShape(QFrame::NoFrame);
        snrDisplay->setFrameShadow(QFrame::Plain);
        snrDisplay->setSegmentStyle(QLCDNumber::Flat);

        retranslateUi(elektorSDR);

        QMetaObject::connectSlotsByName(elektorSDR);
    } // setupUi

    void retranslateUi(QDialog *elektorSDR)
    {
        elektorSDR->setWindowTitle(QApplication::translate("elektorSDR", "sdr-j DAB/DAB+ receiver ", 0, QApplication::UnicodeUTF8));
        elektorSDR->setWindowIconText(QApplication::translate("elektorSDR", "QUIT", 0, QApplication::UnicodeUTF8));
        QuitButton->setText(QApplication::translate("elektorSDR", "QUIT", 0, QApplication::UnicodeUTF8));
        ensembleName->setText(QString());
        resetButton->setText(QApplication::translate("elektorSDR", "reset", 0, QApplication::UnicodeUTF8));
        syncedLabel->setText(QString());
        versionName->setText(QApplication::translate("elektorSDR", "sdr-j-dabreceiver ", 0, QApplication::UnicodeUTF8));
        streamOutSelector->clear();
        streamOutSelector->insertItems(0, QStringList()
         << QApplication::translate("elektorSDR", "select device", 0, QApplication::UnicodeUTF8)
        );
        timeDisplay->setText(QApplication::translate("elektorSDR", "TextLabel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class elektorSDR: public Ui_elektorSDR {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SDRGUI_H
