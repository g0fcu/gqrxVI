/* gqrxvi - a gqrx  interface for visually impaired users

 gqrx must be started with
 the 'Remote Control via TCP' button clicked and
 Port used is the default for gqrx

 Copyright 2017 Simon Kennedy, G0FCU, g0fcu at g0fcu.com
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "mainwindow.h"
#include <QLoggingCategory>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)

{
    ui.setupUi(this);
    QLoggingCategory::setFilterRules(QStringLiteral("qt.speech.tts=true \n qt.speech.tts.*=true"));


    socket = new QTcpSocket(this);
    socket->connectToHost("127.0.0.1", 7356);

    if(!socket->waitForDisconnected(1000))
    {
        qDebug() << "Error: " << socket->errorString();
    }
    QString newFreq;
    newFreq = readGqrx("f\n");
    setDigits(newFreq);
    setNextDigitTenHz = false;
    setNextDigitHundredHz = false;
    setNextDigitOnekHz = false;
    setNextDigitTenkHz = false;
    setNextDigitHundredkHz = false;
    setNextDigitOneMHz = false;
    setNextDigitTenMHz = false;
    setNextDigitHundredMHz = false;
    setNextDigitThousandMHz = false;
    rollOver = false;
    speak = false;

    old_ThousandMHz = newFreq.mid(newFreq.length() - 11, 1);
    old_HundredMHz = newFreq.mid(newFreq.length() - 10, 1);
    old_TenMHz = newFreq.mid(newFreq.length() - 9, 1);
    old_OneMHz = newFreq.mid(newFreq.length() - 8, 1);
    old_HundredkHz = newFreq.mid(newFreq.length() - 7, 1);
    old_TenkHz = newFreq.mid(newFreq.length() - 6, 1);
    old_OnekHz = newFreq.mid(newFreq.length() - 5, 1);
    old_HundredHz = newFreq.mid(newFreq.length() - 4, 1);
    old_TenHz = newFreq.mid(newFreq.length() - 3, 1);

    loadModeCombo();

    connect(ui.Hambands, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &MainWindow::speakBand);
    connect(ui.comboMode, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &MainWindow::speakMode);
    connect(ui.OneMHz, static_cast<void(QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged), this, &MainWindow::speakOneMHz);
    connect(ui.TenMHz, static_cast<void(QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged), this, &MainWindow::speakTenMHz);
    connect(ui.ThousandMHz, static_cast<void(QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged), this, &MainWindow::speakThousandMHz);
    connect(ui.HundredMHz, static_cast<void(QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged), this, &MainWindow::speakHundredMHz);
    connect(ui.HundredkHz, static_cast<void(QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged), this, &MainWindow::speakHundredkHz);
    connect(ui.TenkHz, static_cast<void(QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged), this, &MainWindow::speakTenkHz);
    connect(ui.OnekHz, static_cast<void(QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged), this, &MainWindow::speakOnekHz);
    connect(ui.HundredHz, static_cast<void(QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged), this, &MainWindow::speakHundredHz);
    connect(ui.TenHz, static_cast<void(QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged), this, &MainWindow::speakTenHz);
    connect(ui.Speech_On, &QAction::triggered, this, &MainWindow::setSpeechOn);
    connect(ui.Speech_Off, &QAction::triggered, this, &MainWindow::setSpeechOff);
    connect(ui.actionSync, &QAction::triggered, this, &MainWindow::setSync);
}
void MainWindow::loadModeCombo()
{
    QStringList list;
    socket->write("M ?\n");
    socket->waitForReadyRead(100);
    char rc[100];
    // check return code
    socket->readLine(rc, sizeof(rc));
    QString returnCode = QString::fromLocal8Bit(rc);
    if(returnCode.left(4) != "RPRT")
    {
      QStringList list = returnCode.split(QRegExp("\\s+"), QString::SkipEmptyParts);
      ui.comboMode->addItems(list);
      int usbIndex =  ui.comboMode->findText("USB");
      ui.comboMode->setCurrentIndex(usbIndex);
    }
}
void MainWindow::setSpeechOn(bool Value)
{
        speak = true;
}
void MainWindow::setSpeechOff(bool Value)
{
        speak = false;
}
void MainWindow::setSync(bool Value)
{
        QString newFreq;
        old_speak = speak;
        speak = false;
        newFreq = readGqrx("f\n");
        setDigits(newFreq);
        speak = old_speak;
        if(speak == true)
        {
         //   m_speech->say("Frequency synced");
        }
}
void MainWindow::speakOneMHz(QString Value)
{
    QString newFreq, msg;
    int v;
    if(Value != old_OneMHz)
    {
        if(old_OneMHz.left(old_OneMHz.length() - 10) == "9" and Value.left(Value.length() - 10) == "0")
        {
            if((ui.OneMHz->value() == 0) and
               (ui.ThousandMHz->value() +
                ui.HundredMHz->value() +
                ui.TenMHz->value() == 0))
            {
                v = 1;
                rollOver = true;
            }
            else
            {
                v = ui.TenMHz->value() + 1;
            }
            setNextDigitTenMHz = true;

        }
        if(old_OneMHz.left(old_OneMHz.length() - 10) == "0" and Value.left(Value.length() - 10) == "9")
        {
            if((ui.OneMHz->value() == 9) and
               (ui.ThousandMHz->value() +
                ui.HundredMHz->value() +
                ui.TenMHz->value() == 0))
            {
                v = 0;
                rollOver = true;
            }
            else
            {
                v = ui.TenMHz->value() - 1;
            }
            setNextDigitTenMHz = true;
         }
    }
    if(setNextDigitTenMHz)
    {
        ui.TenMHz->setValue(v);
        setNextDigitTenMHz = false;
    }
    else
    {
        QString m1 = QString::number(ui.ThousandMHz->value());
        QString m2 = QString::number(ui.HundredMHz->value());
        QString m3 = QString::number(ui.TenMHz->value());
        QString m4 = QString::number(ui.OneMHz->value());
        QString m5 = QString::number(ui.HundredkHz->value());
        QString m6 = QString::number(ui.TenkHz->value());
        QString m7 = QString::number(ui.OnekHz->value());
        QString m8 = QString::number(ui.HundredHz->value());
        QString m9 = QString::number(ui.TenHz->value());
        msg = "F " + m1 + m2 + m3 + m4 + m5 + m6 + m7 + m8 + m9 + "0";
        writeToGqrx(msg);
        if(speak == true)
        {
        //    m_speech->say(Value);
        }
        newFreq = readGqrx("f\n");
    }
    old_OneMHz = Value;
    rollOver = false;
}

void MainWindow::speakThousandMHz(QString Value)
{
    QString newFreq, msg;
    int v;
    if(Value != old_ThousandMHz)
    {
        QString m1 = QString::number(ui.ThousandMHz->value());
        QString m2 = QString::number(ui.HundredMHz->value());
        QString m3 = QString::number(ui.TenMHz->value());
        QString m4 = QString::number(ui.OneMHz->value());
        QString m5 = QString::number(ui.HundredkHz->value());
        QString m6 = QString::number(ui.TenkHz->value());
        QString m7 = QString::number(ui.OnekHz->value());
        QString m8 = QString::number(ui.HundredHz->value());
        QString m9 = QString::number(ui.TenHz->value());
        msg = "F " + m1 + m2 + m3 + m4 + m5 + m6 + m7 + m8 + m9 + "0";
        writeToGqrx(msg);
        if(speak == true)
        {
         //   m_speech->say(Value);
        }
        newFreq = readGqrx("f\n");
    }
    old_ThousandMHz = Value;
}

void MainWindow::speakHundredMHz(QString Value)
{
    QString newFreq, msg;
    int v;
    if(Value != old_HundredMHz)
    {
        if(old_HundredMHz.left(old_HundredMHz.length() - 18) == "9" and Value.left(Value.length() - 18) == "0")
        {
            if((ui.HundredMHz->value() == 0) and
               (ui.ThousandMHz->value() == 0))
            {
                v = 1;
                rollOver = true;
            }
            else
            {
                v = ui.ThousandMHz->value() + 1;
            }
            setNextDigitThousandMHz = true;

        }
        if(old_HundredMHz.left(old_HundredMHz.length() - 18) == "0" and Value.left(Value.length() - 18) == "9")
        {
            if((ui.HundredMHz->value() == 9) and
               (ui.ThousandMHz->value() == 0))
            {
                v = 0;
                rollOver = true;
            }
            else
            {
                v = ui.ThousandMHz->value() - 1;
            }
            setNextDigitThousandMHz = true;
         }
    }


    if(setNextDigitThousandMHz)
    {
        ui.ThousandMHz->setValue(v);
        setNextDigitThousandMHz = false;
    }
    else
    {
        QString m1 = QString::number(ui.ThousandMHz->value());
        QString m2 = QString::number(ui.HundredMHz->value());
        QString m3 = QString::number(ui.TenMHz->value());
        QString m4 = QString::number(ui.OneMHz->value());
        QString m5 = QString::number(ui.HundredkHz->value());
        QString m6 = QString::number(ui.TenkHz->value());
        QString m7 = QString::number(ui.OnekHz->value());
        QString m8 = QString::number(ui.HundredHz->value());
        QString m9 = QString::number(ui.TenHz->value());
        msg = "F " + m1 + m2 + m3 + m4 + m5 + m6 + m7 + m8 + m9 + "0";
        writeToGqrx(msg);
        if(speak == true)
        {
       //     m_speech->say(Value);
        }
        newFreq = readGqrx("f\n");
    }
    old_HundredMHz = Value;
    rollOver = false;
}

void MainWindow::speakTenMHz(QString Value)
{
    QString newFreq, msg;
    int v;
    if(Value != old_TenMHz)
    {
        if(old_TenMHz.left(old_TenMHz.length() - 14) == "9" and Value.left(Value.length() - 14) == "0")
        {
            if((ui.TenMHz->value() == 0) and
               (ui.ThousandMHz->value() +
                ui.HundredMHz->value() == 0))
            {
                v = 1;
                rollOver = true;
            }
            else
            {
                v = ui.HundredMHz->value() + 1;
            }
            setNextDigitHundredMHz = true;

        }
        if(old_TenMHz.left(old_TenMHz.length() - 14) == "0" and Value.left(Value.length() - 14) == "9")
        {
            if((ui.TenMHz->value() == 9) and
               (ui.ThousandMHz->value() +
                ui.HundredMHz->value() == 0))
            {
                v = 0;
                rollOver = true;
            }
            else
            {
                v = ui.HundredMHz->value() - 1;
            }
            setNextDigitHundredMHz = true;
         }
    }

    if(setNextDigitHundredMHz)
    {
        ui.HundredMHz->setValue(v);
        setNextDigitHundredMHz = false;
    }
    else
    {
        QString m1 = QString::number(ui.ThousandMHz->value());
        QString m2 = QString::number(ui.HundredMHz->value());
        QString m3 = QString::number(ui.TenMHz->value());
        QString m4 = QString::number(ui.OneMHz->value());
        QString m5 = QString::number(ui.HundredkHz->value());
        QString m6 = QString::number(ui.TenkHz->value());
        QString m7 = QString::number(ui.OnekHz->value());
        QString m8 = QString::number(ui.HundredHz->value());
        QString m9 = QString::number(ui.TenHz->value());
        msg = "F " + m1 + m2 + m3 + m4 + m5 + m6 + m7 + m8 + m9 + "0";
        writeToGqrx(msg);
        if(speak == true)
        {
        //    m_speech->say(Value);
        }
        newFreq = readGqrx("f\n");
    }

    old_TenMHz = Value;
    rollOver = false;
}
void MainWindow::speakHundredkHz(QString Value)
{
    QString newFreq, msg;
    int v;
    if(Value != old_HundredkHz)
    {
        if(old_HundredkHz.left(old_HundredkHz.length() - 18) == "9" and Value.left(Value.length() - 18) == "0")
        {
            if((ui.HundredkHz->value() == 0) and
               (ui.ThousandMHz->value() +
                ui.HundredMHz->value() +
                ui.TenMHz->value() +
                ui.OneMHz->value() == 0))
            {
                v = 1;
                rollOver = true;
            }
            else
            {
                v = ui.OneMHz->value() + 1;
            }
            setNextDigitOneMHz = true;

        }
        if(old_HundredkHz.left(old_HundredkHz.length() - 18) == "0" and Value.left(Value.length() - 18) == "9")
        {
            if((ui.HundredkHz->value() == 9) and
               (ui.ThousandMHz->value() +
                ui.HundredMHz->value() +
                ui.TenMHz->value() +
                ui.OneMHz->value() == 0))
            {
                v = 0;
                rollOver = true;
            }
            else
            {
                v = ui.OneMHz->value() - 1;
            }
            setNextDigitOneMHz = true;
         }
    }


    if(setNextDigitOneMHz)
    {
        ui.OneMHz->setValue(v);
        setNextDigitOneMHz = false;
    }
    else
    {
        QString m1 = QString::number(ui.ThousandMHz->value());
        QString m2 = QString::number(ui.HundredMHz->value());
        QString m3 = QString::number(ui.TenMHz->value());
        QString m4 = QString::number(ui.OneMHz->value());
        QString m5 = QString::number(ui.HundredkHz->value());
        QString m6 = QString::number(ui.TenkHz->value());
        QString m7 = QString::number(ui.OnekHz->value());
        QString m8 = QString::number(ui.HundredHz->value());
        QString m9 = QString::number(ui.TenHz->value());
        msg = "F " + m1 + m2 + m3 + m4 + m5 + m6 + m7 + m8 + m9 + "0";
        writeToGqrx(msg);
        if(speak == true)
        {
        //    m_speech->say(Value);
        }
        newFreq = readGqrx("f\n");
    }
    old_HundredkHz = Value;
}
void MainWindow::speakTenkHz(QString Value)
{
    QString newFreq, msg;
    int v;
    if(Value != old_TenkHz)
    {
        if(old_TenkHz.left(old_TenkHz.length() - 14) == "9" and Value.left(Value.length() - 14) == "0")
        {
            if((ui.TenkHz->value() == 0) and
               (ui.ThousandMHz->value() +
                ui.HundredMHz->value() +
                ui.TenMHz->value() +
                ui.OneMHz->value() +
                ui.HundredkHz->value() == 0))
            {
                v = 1;
                rollOver = true;
            }
            else
            {
                v = ui.HundredkHz->value() + 1;
            }
            setNextDigitHundredkHz = true;

        }
        if(old_TenkHz.left(old_TenkHz.length() - 14) == "0" and Value.left(Value.length() - 14) == "9")
        {
            if((ui.TenkHz->value() == 9) and
               (ui.ThousandMHz->value() +
                ui.HundredMHz->value() +
                ui.OneMHz->value() +
                ui.HundredkHz->value() == 0))
            {
                v = 0;
                rollOver = true;
            }
            else
            {
                v = ui.HundredkHz->value() - 1;
            }
            setNextDigitHundredkHz = true;
         }
    }

    if(setNextDigitHundredkHz)
    {
        ui.HundredkHz->setValue(v);
        setNextDigitHundredkHz = false;
    }
    else
    {
        QString m1 = QString::number(ui.ThousandMHz->value());
        QString m2 = QString::number(ui.HundredMHz->value());
        QString m3 = QString::number(ui.TenMHz->value());
        QString m4 = QString::number(ui.OneMHz->value());
        QString m5 = QString::number(ui.HundredkHz->value());
        QString m6 = QString::number(ui.TenkHz->value());
        QString m7 = QString::number(ui.OnekHz->value());
        QString m8 = QString::number(ui.HundredHz->value());
        QString m9 = QString::number(ui.TenHz->value());
        msg = "F " + m1 + m2 + m3 + m4 + m5 + m6 + m7 + m8 + m9 + "0";
        writeToGqrx(msg);
        if(speak == true)
        {
         //   m_speech->say(Value);
        }
        newFreq = readGqrx("f\n");
    }
    old_TenkHz = Value;
}
void MainWindow::speakOnekHz(QString Value)
{
    QString newFreq, msg;
    int v;
    if(Value != old_OnekHz)
    {
        if(old_OnekHz.left(old_OnekHz.length() - 10) == "9" and Value.left(Value.length() - 10) == "0")
        {
            if((ui.OnekHz->value() == 0) and
               (ui.ThousandMHz->value() +
                ui.HundredMHz->value() +
                ui.TenMHz->value() +
                ui.OneMHz->value() +
                ui.HundredkHz->value() +
                ui.TenkHz->value() == 0))
            {
                v = 1;
                rollOver = true;
            }
            else
            {
                v = ui.TenkHz->value() + 1;
            }
            setNextDigitTenkHz = true;

        }
        if(old_OnekHz.left(old_OnekHz.length() - 10) == "0" and Value.left(Value.length() - 10) == "9")
        {
            if((ui.OnekHz->value() == 9) and
               (ui.ThousandMHz->value() +
                ui.HundredMHz->value() +
                ui.OneMHz->value() +
                ui.HundredkHz->value() +
                ui.TenkHz->value() == 0))
            {
                v = 0;
                rollOver = true;
            }
            else
            {
                v = ui.TenkHz->value() - 1;
            }
            setNextDigitTenkHz = true;
         }
    }

    if(setNextDigitTenkHz)
    {
        ui.TenkHz->setValue(v);
        setNextDigitTenkHz = false;
    }
    else
    {
        QString m1 = QString::number(ui.ThousandMHz->value());
        QString m2 = QString::number(ui.HundredMHz->value());
        QString m3 = QString::number(ui.TenMHz->value());
        QString m4 = QString::number(ui.OneMHz->value());
        QString m5 = QString::number(ui.HundredkHz->value());
        QString m6 = QString::number(ui.TenkHz->value());
        QString m7 = QString::number(ui.OnekHz->value());
        QString m8 = QString::number(ui.HundredHz->value());
        QString m9 = QString::number(ui.TenHz->value());
        msg = "F " + m1 + m2 + m3 + m4 + m5 + m6 + m7 + m8 + m9 + "0";
        writeToGqrx(msg);
        if(speak == true)
        {
          //  m_speech->say(Value);
        }
        newFreq = readGqrx("f\n");
    }
    old_OnekHz = Value;
}
void MainWindow::speakHundredHz(QString Value)
{
    QString newFreq, msg;
    int v;
    if(Value != old_HundredHz)
    {
        if(old_HundredHz.left(old_HundredHz.length() - 14) == "9" and Value.left(Value.length() - 14) == "0")
        {
            if((ui.HundredHz->value() == 0) and
               (ui.ThousandMHz->value() +
                ui.HundredMHz->value() +
                ui.TenMHz->value() +
                ui.OneMHz->value() +
                ui.HundredkHz->value() +
                ui.TenkHz->value() +
                ui.OnekHz->value() == 0))
            {
                v = 1;
                rollOver = true;
            }
            else
            {
                v = ui.OnekHz->value() + 1;
            }
            setNextDigitOnekHz = true;

        }
        if(old_HundredHz.left(old_HundredHz.length() - 14) == "0" and Value.left(Value.length() - 14) == "9")
        {
            if((ui.HundredHz->value() == 9) and
               (ui.ThousandMHz->value() +
                ui.HundredMHz->value() +
                ui.OneMHz->value() +
                ui.HundredkHz->value() +
                ui.TenkHz->value() +
                ui.OnekHz->value() == 0))
            {
                v = 0;
                rollOver = true;
            }
            else
            {
                v = ui.OnekHz->value() - 1;
            }
            setNextDigitOnekHz = true;
         }
    }

    if(setNextDigitOnekHz)
    {
        ui.OnekHz->setValue(v);
        setNextDigitOnekHz = false;
    }
    else
    {
        QString m1 = QString::number(ui.ThousandMHz->value());
        QString m2 = QString::number(ui.HundredMHz->value());
        QString m3 = QString::number(ui.TenMHz->value());
        QString m4 = QString::number(ui.OneMHz->value());
        QString m5 = QString::number(ui.HundredkHz->value());
        QString m6 = QString::number(ui.TenkHz->value());
        QString m7 = QString::number(ui.OnekHz->value());
        QString m8 = QString::number(ui.HundredHz->value());
        QString m9 = QString::number(ui.TenHz->value());
        msg = "F " + m1 + m2 + m3 + m4 + m5 + m6 + m7 + m8 + m9 + "0";
        writeToGqrx(msg);
        if(speak == true)
        {
         //   m_speech->say(Value);
        }
        newFreq = readGqrx("f\n");
    }
    old_HundredHz = Value;
}
void MainWindow::speakTenHz(QString Value)
{
    QString newFreq, msg;
    int v;
    if(Value != old_TenHz)
    {
        if(old_TenHz.left(old_TenHz.length() - 10) == "9" and Value.left(Value.length() - 10) == "0")
        {
            if((ui.TenHz->value() == 0) and
               (ui.ThousandMHz->value() +
                ui.HundredMHz->value() +
                ui.TenMHz->value() +
                ui.OneMHz->value() +
                ui.HundredkHz->value() +
                ui.TenkHz->value() +
                ui.OnekHz->value() +
                ui.HundredHz->value() == 0))
            {
                v = 1;
                rollOver = true;
            }
            else
            {
                v = ui.HundredHz->value() + 1;
            }
            setNextDigitHundredHz = true;

        }
        if(old_TenHz.left(old_TenHz.length() - 10) == "0" and Value.left(Value.length() - 10) == "9")
        {
            if((ui.TenHz->value() == 9) and
               (ui.ThousandMHz->value() +
                ui.HundredMHz->value() +
                ui.OneMHz->value() +
                ui.HundredkHz->value() +
                ui.OnekHz->value() +
                ui.HundredHz->value() == 0))
            {
                v = 0;
                rollOver = true;
            }
            else
            {
                v = ui.HundredHz->value() - 1;
            }
            setNextDigitHundredHz = true;
         }
    }

    if(setNextDigitHundredHz)
    {
        ui.HundredHz->setValue(v);
        setNextDigitHundredHz = false;
    }
    else
    {
        QString m1 = QString::number(ui.ThousandMHz->value());
        QString m2 = QString::number(ui.HundredMHz->value());
        QString m3 = QString::number(ui.TenMHz->value());
        QString m4 = QString::number(ui.OneMHz->value());
        QString m5 = QString::number(ui.HundredkHz->value());
        QString m6 = QString::number(ui.TenkHz->value());
        QString m7 = QString::number(ui.OnekHz->value());
        QString m8 = QString::number(ui.HundredHz->value());
        QString m9 = QString::number(ui.TenHz->value());
        msg = "F " + m1 + m2 + m3 + m4 + m5 + m6 + m7 + m8 + m9 + "0";
        writeToGqrx(msg);
        if(speak == true)
        {
         //   m_speech->say(Value);
        }
        newFreq = readGqrx("f\n");
    }
    old_TenHz = Value;
}

void MainWindow::speakMode(int Mode)
{
    old_speak = speak;
    speak = true;
    if(speak == true)
    {
      //  m_speech->say(ui.comboMode->currentText());
    }
   switch(Mode)
   {
   case(0):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   case(1):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   case(2):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   case(3):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   case(4):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   case(5):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   case(6):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   case(7):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   case(8):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   case(9):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   case(10):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   case(11):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   case(12):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   case(13):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   case(14):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   case(15):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   case(16):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   case(17):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   case(18):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   case(19):
       writeToGqrx("M " + ui.comboMode->currentText() + "\n");
       break;
   }
   speak = old_speak;
}
void MainWindow::speakBand(int Band)
{
    QString newFreq;
    old_speak = speak;
    speak = true;
    switch(Band)
    {
    case 0:
        if(speak == true)
        {
     //       m_speech->say("Top band");
        }
        writeToGqrx("F 1800000\n");
        newFreq = readGqrx("f\n");
        setDigits(newFreq);
        break;
    case 1:
        if(speak == true)
        {
      //      m_speech->say("Eighty metres");
        }
        writeToGqrx("F 3500000\n");
        newFreq = readGqrx("f\n");
        setDigits(newFreq);
        break;
    case 2:
        if(speak == true)
        {
      //      m_speech->say("Sixty meters");
        }
        writeToGqrx("F 5300000\n");
        newFreq = readGqrx("f\n");
        setDigits(newFreq);
        break;
    case 3:
        if(speak == true)
        {
      //      m_speech->say("Forty metres");
        }
        writeToGqrx("F 7000000\n");
        newFreq = readGqrx("f\n");
        setDigits(newFreq);
        break;
    case 4:
        if(speak == true)
        {
     //       m_speech->say("Thirty metres");
        }
        writeToGqrx("F 10100000\n");
        newFreq = readGqrx("f\n");
        setDigits(newFreq);
        break;
    case 5:
        if(speak == true)
        {
       //     m_speech->say("Twenty metres");
        }
        writeToGqrx("F 14000000\n");
        newFreq = readGqrx("f\n");
        setDigits(newFreq);
        break;
    case 6:
        if(speak == true)
        {
       //     m_speech->say("Seventeen metres");
        }
        writeToGqrx("F 18068000\n");
        newFreq = readGqrx("f\n");
        setDigits(newFreq);
        break;
    case 7:
        if(speak == true)
        {
      //      m_speech->say("Fifteen metres");
        }
        writeToGqrx("F 21000000\n");
        newFreq = readGqrx("f\n");
        setDigits(newFreq);
        break;
    case 8:
        if(speak == true)
        {
      //      m_speech->say("Twelve metres");
        }
        writeToGqrx("F 24890000\n");
        newFreq = readGqrx("f\n");
        setDigits(newFreq);
        break;
    case 9:
        if(speak == true)
        {
      //      m_speech->say("Ten metres");
        }
        writeToGqrx("F 28000000\n");
        newFreq = readGqrx("f\n");
        setDigits(newFreq);
        break;
    case 10:
        if(speak == true)
        {
      //      m_speech->say("Six metres");
        }
        writeToGqrx("F 50000000\n");
        newFreq = readGqrx("f\n");
        setDigits(newFreq);
        break;
    case 11:
        if(speak == true)
        {
      //      m_speech->say("Four metres");
        }
        writeToGqrx("F 70000000\n");
        newFreq = readGqrx("f\n");
        setDigits(newFreq);
        break;
    case 12:
        if(speak == true)
        {
      //      m_speech->say("Two metres");
        }
        writeToGqrx("F 144000000\n");
        newFreq = readGqrx("f\n");
        setDigits(newFreq);
        break;
    case 13:
        if(speak == true)
        {
      //      m_speech->say("Seventy centimetres");
        }
        writeToGqrx("F 430000000\n");
        newFreq = readGqrx("f\n");
        setDigits(newFreq);
        break;
    }
    speak = old_speak;
}

void MainWindow::writeToGqrx(QString msgToGqrx)
{
    QByteArray msg = msgToGqrx.toLocal8Bit();
    const char *c_str2 = msg.data();
    socket->write(c_str2, qstrlen(c_str2));
    socket->waitForReadyRead(100);
    char rc[12];
    // check return code
    socket->readLine(rc, sizeof(rc));
    QString returnCode = QString::fromLocal8Bit(rc);
    if(returnCode.left(6) != "RPRT 0")
    {
        if(returnCode.left(4) == "RPRT")
        //check for response from gqrx
        {
            if(returnCode.mid(6,1) == "-")
            {
            // check error code
       //         m_speech->say("gqrx reports error code");
        //        m_speech->say(returnCode);
                return;
            }
         }
     }
    socket->waitForReadyRead(100);
}
//}

QString MainWindow::readGqrx(QString msgToGqrx)
{
    qDebug() << "readGqrx";
    QByteArray msg = msgToGqrx.toLocal8Bit();
    const char *c_str2 = msg.data();
    socket->write(c_str2, qstrlen(c_str2));
    socket->waitForReadyRead(100);
    char rc[12];
    // check return code
    socket->readLine(rc, sizeof(rc));
    QString returnCode = QString::fromLocal8Bit(rc);
     qDebug() << returnCode;
    if(returnCode.left(4) != "RPRT")
    {
       //frequency returned
        ui.freq->setText(returnCode);
        //setDigits(returnCode);
        //if msg starts with RPRT and is not RPRT 0 then error
        //else new frequency has been returned
        return returnCode;
     }
     else
     {
     //if(returnCode.mid(6,1) == "-")
       // {
        // error
     //    m_speech->say("gqrx reports error code");
     //    m_speech->say(returnCode);

        //}
     }
}

void MainWindow::readyRead()
{
    qDebug() << "Reading...";
    //qDebug() << socket->readAll();
    QByteArray msg = socket->readAll();
    QString string = QString(QLatin1String(msg));
    //string = QString::fromLocal8Bit(msg);

    if(string.left(4) == "RPRT")
        // check for error
        {
         if(string.mid(6,1) == "-")
            {
            // error
        //     m_speech->say("gqrx reports error code");
        //     m_speech->say(string);
             return;
            }
         }
    else
    {
       ui.freq->setText(string);
    }
}

void MainWindow::setDigits(QString returnCode)
{
    int lengthFreq = returnCode.length();
    QString t;
    int v;
    qDebug() << returnCode;qDebug() << returnCode.length();
    switch(lengthFreq)
    {
    case 1:
        break;
    case 2:
        break;
    case 3:
        t = returnCode.right(3).left(1);
        v = t.toInt();
        ui.TenHz->setValue(v);
        ui.HundredHz->setValue(0);
        ui.OnekHz->setValue(0);
        ui.TenkHz->setValue(0);
        ui.HundredkHz->setValue(0);
        ui.OneMHz->setValue(0);
        ui.TenMHz->setValue(0);
        ui.HundredMHz->setValue(0);
        ui.ThousandMHz->setValue(0);
        break;
    case 4:
        t = returnCode.right(3).left(1);
        v = t.toInt();
        ui.TenHz->setValue(v);
        t = returnCode.right(4).left(1);
        v = t.toInt();
        ui.HundredHz->setValue(v);
        ui.OnekHz->setValue(0);
        ui.TenkHz->setValue(0);
        ui.HundredkHz->setValue(0);
        ui.OneMHz->setValue(0);
        ui.TenMHz->setValue(0);
        ui.HundredMHz->setValue(0);
        ui.ThousandMHz->setValue(0);
        break;
    case 5:
        t = returnCode.right(3).left(1);
        v = t.toInt();
        ui.TenHz->setValue(v);
        t = returnCode.right(4).left(1);
        v = t.toInt();
        ui.HundredHz->setValue(v);
        t = returnCode.right(5).left(1);
        v = t.toInt();
        ui.OnekHz->setValue(v);
        ui.TenkHz->setValue(0);
        ui.HundredkHz->setValue(0);
        ui.OneMHz->setValue(0);
        ui.TenMHz->setValue(0);
        ui.HundredMHz->setValue(0);
        ui.ThousandMHz->setValue(0);
        break;
    case 6:
        t = returnCode.right(3).left(1);
        v = t.toInt();
        ui.TenHz->setValue(v);
        t = returnCode.right(4).left(1);
        v = t.toInt();
        ui.HundredHz->setValue(v);
        t = returnCode.right(5).left(1);
        v = t.toInt();
        ui.OnekHz->setValue(v);
        t = returnCode.right(6).left(1);
        v = t.toInt();
        ui.TenkHz->setValue(v);
        ui.HundredkHz->setValue(0);
        ui.OneMHz->setValue(0);
        ui.TenMHz->setValue(0);
        ui.HundredMHz->setValue(0);
        ui.ThousandMHz->setValue(0);
        break;
    case 7:
        t = returnCode.right(3).left(1);
        v = t.toInt();
        ui.TenHz->setValue(v);
        t = returnCode.right(4).left(1);
        v = t.toInt();
        ui.HundredHz->setValue(v);
        t = returnCode.right(5).left(1);
        v = t.toInt();
        ui.OnekHz->setValue(v);
        t = returnCode.right(6).left(1);
        v = t.toInt();
        ui.TenkHz->setValue(v);
        t = returnCode.right(7).left(1);
        v = t.toInt();
        ui.HundredkHz->setValue(v);
        ui.OneMHz->setValue(0);
        ui.TenMHz->setValue(0);
        ui.HundredMHz->setValue(0);
        ui.ThousandMHz->setValue(0);
        break;
    case 8:
        t = returnCode.right(3).left(1);
        v = t.toInt();
        ui.TenHz->setValue(v);
        t = returnCode.right(4).left(1);
        v = t.toInt();
        ui.HundredHz->setValue(v);
        t = returnCode.right(5).left(1);
        v = t.toInt();
        ui.OnekHz->setValue(v);
        t = returnCode.right(6).left(1);
        v = t.toInt();
        ui.TenkHz->setValue(v);
        t = returnCode.right(7).left(1);
        v = t.toInt();
        ui.HundredkHz->setValue(v);
        t = returnCode.right(8).left(1);
        v = t.toInt();
        ui.OneMHz->setValue(v);
        ui.TenMHz->setValue(0);
        ui.HundredMHz->setValue(0);
        ui.ThousandMHz->setValue(0);
        break;
    case 9:
        t = returnCode.right(3).left(1);
        v = t.toInt();
        ui.TenHz->setValue(v);
        t = returnCode.right(4).left(1);
        v = t.toInt();
        ui.HundredHz->setValue(v);
        t = returnCode.right(5).left(1);
        v = t.toInt();
        ui.OnekHz->setValue(v);
        t = returnCode.right(6).left(1);
        v = t.toInt();
        ui.TenkHz->setValue(v);
        t = returnCode.right(7).left(1);
        v = t.toInt();
        ui.HundredkHz->setValue(v);
        t = returnCode.right(8).left(1);
        v = t.toInt();
        ui.OneMHz->setValue(v);
        t = returnCode.right(9).left(1);
        v = t.toInt();
        ui.TenMHz->setValue(v);
        ui.HundredMHz->setValue(0);
        ui.ThousandMHz->setValue(0);
        break;
    case 10:
        t = returnCode.right(3).left(1);
        v = t.toInt();
        ui.TenHz->setValue(v);
        t = returnCode.right(4).left(1);
        v = t.toInt();
        ui.HundredHz->setValue(v);
        t = returnCode.right(5).left(1);
        v = t.toInt();
        ui.OnekHz->setValue(v);
        t = returnCode.right(6).left(1);
        v = t.toInt();
        ui.TenkHz->setValue(v);
        t = returnCode.right(7).left(1);
        v = t.toInt();
        ui.HundredkHz->setValue(v);
        t = returnCode.right(8).left(1);
        v = t.toInt();
        ui.OneMHz->setValue(v);
        t = returnCode.right(9).left(1);
        v = t.toInt();
        ui.TenMHz->setValue(v);
        t = returnCode.right(10).left(1);
        v = t.toInt();
        ui.HundredMHz->setValue(v);
        ui.ThousandMHz->setValue(0);
        break;
    case 11:
        t = returnCode.right(3).left(1);
        v = t.toInt();
        ui.TenHz->setValue(v);
        t = returnCode.right(4).left(1);
        v = t.toInt();
        ui.HundredHz->setValue(v);
        t = returnCode.right(5).left(1);
        v = t.toInt();
        ui.OnekHz->setValue(v);
        t = returnCode.right(6).left(1);
        v = t.toInt();
        ui.TenkHz->setValue(v);
        t = returnCode.right(7).left(1);
        v = t.toInt();
        ui.HundredkHz->setValue(v);
        t = returnCode.right(8).left(1);
        v = t.toInt();
        ui.OneMHz->setValue(v);
        t = returnCode.right(9).left(1);
        v = t.toInt();
        ui.TenMHz->setValue(v);
        t = returnCode.right(10).left(1);
        v = t.toInt();
        ui.HundredMHz->setValue(v);
        t = returnCode.right(11).left(1);
        v = t.toInt();
        ui.ThousandMHz->setValue(v);
        break;
    }
}

