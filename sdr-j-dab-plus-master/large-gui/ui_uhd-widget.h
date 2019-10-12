/********************************************************************************
** Form generated from reading UI file 'uhd-widget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UHD_2D_WIDGET_H
#define UI_UHD_2D_WIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_uhdWidget
{
public:
    QFrame *frame;
    QLabel *label;
    QSpinBox *externalGain;
    QSpinBox *f_correction;
    QSpinBox *KhzOffset;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *statusLabel;

    void setupUi(QWidget *uhdWidget)
    {
        if (uhdWidget->objectName().isEmpty())
            uhdWidget->setObjectName(QString::fromUtf8("uhdWidget"));
        uhdWidget->resize(256, 133);
        frame = new QFrame(uhdWidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, 0, 211, 221));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 100, 101, 21));
        externalGain = new QSpinBox(frame);
        externalGain->setObjectName(QString::fromUtf8("externalGain"));
        externalGain->setGeometry(QRect(0, 0, 91, 21));
        externalGain->setMaximum(103);
        externalGain->setValue(55);
        f_correction = new QSpinBox(frame);
        f_correction->setObjectName(QString::fromUtf8("f_correction"));
        f_correction->setGeometry(QRect(0, 20, 91, 21));
        f_correction->setMinimum(-100);
        f_correction->setMaximum(100);
        KhzOffset = new QSpinBox(frame);
        KhzOffset->setObjectName(QString::fromUtf8("KhzOffset"));
        KhzOffset->setGeometry(QRect(0, 40, 91, 21));
        KhzOffset->setMaximum(1000000);
        label_3 = new QLabel(frame);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(100, 40, 41, 21));
        label_4 = new QLabel(frame);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(100, 20, 51, 21));
        label_5 = new QLabel(frame);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(100, 0, 101, 21));
        statusLabel = new QLabel(frame);
        statusLabel->setObjectName(QString::fromUtf8("statusLabel"));
        statusLabel->setGeometry(QRect(16, 180, 121, 21));

        retranslateUi(uhdWidget);

        QMetaObject::connectSlotsByName(uhdWidget);
    } // setupUi

    void retranslateUi(QWidget *uhdWidget)
    {
        uhdWidget->setWindowTitle(QApplication::translate("uhdWidget", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("uhdWidget", "uhd device", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("uhdWidget", "KHz", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("uhdWidget", "ppm", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("uhdWidget", "gain reductiom", 0, QApplication::UnicodeUTF8));
        statusLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class uhdWidget: public Ui_uhdWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UHD_2D_WIDGET_H
