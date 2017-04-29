#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtWidgets/qmainwindow.h>
#include "ui_mainwindow.h"

//#include <QtCore/QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QIODevice>

#include <QAbstractSocket>
#include <cstring>
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);

public slots:
    void speakBand(int);
    void speakOneMHz(QString);
    void speakHundredMHz(QString);
    void speakThousandMHz(QString);
    void speakTenMHz(QString);
    void speakHundredkHz(QString);
    void speakTenkHz(QString);
    void speakOnekHz(QString);
    void speakHundredHz(QString);
    void speakTenHz(QString);
    void speakMode(int);
    void setDigits(QString);
    void writeToGqrx(QString);
    QString readGqrx(QString);
    void readyRead();
    void setSpeechOn(bool);
    void setSpeechOff(bool);
    void setSync(bool);
    void loadModeCombo();
private:
    Ui::MainWindow ui;
    QTcpSocket *socket;
    QString old_ThousandMHz;
    QString old_HundredMHz;
    QString old_TenMHz;
    QString old_OneMHz;
    QString old_HundredkHz;
    QString old_TenkHz;
    QString old_OnekHz;
    QString old_HundredHz;
    QString old_TenHz;
    bool rollOver;
    bool setNextDigitTenHz;
    bool setNextDigitHundredHz;
    bool setNextDigitOnekHz;
    bool setNextDigitTenkHz;
    bool setNextDigitHundredkHz;
    bool setNextDigitOneMHz;
    bool setNextDigitTenMHz;
    bool setNextDigitHundredMHz;
    bool setNextDigitThousandMHz;
    bool speak;
    bool old_speak;
};

#endif // MAINWINDOW_H
