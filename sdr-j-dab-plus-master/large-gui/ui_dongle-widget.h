/********************************************************************************
** Form generated from reading UI file 'dongle-widget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DONGLE_2D_WIDGET_H
#define UI_DONGLE_2D_WIDGET_H

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

class Ui_dongleWidget
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

    void setupUi(QWidget *dongleWidget)
    {
        if (dongleWidget->objectName().isEmpty())
            dongleWidget->setObjectName(QString::fromUtf8("dongleWidget"));
        dongleWidget->resize(256, 133);
        frame = new QFrame(dongleWidget);
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

        retranslateUi(dongleWidget);

        QMetaObject::connectSlotsByName(dongleWidget);
    } // setupUi

    void retranslateUi(QWidget *dongleWidget)
    {
        dongleWidget->setWindowTitle(QApplication::translate("dongleWidget", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("dongleWidget", "mirics dongle", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("dongleWidget", "KHz", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("dongleWidget", "ppm", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("dongleWidget", "gain reductiom", 0, QApplication::UnicodeUTF8));
        statusLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class dongleWidget: public Ui_dongleWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DONGLE_2D_WIDGET_H
