/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Pendurthi Kaushik <kaushiksjce@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "Accelerator.h"
#include "MainWindow.h"
#include "Common.h"
#ifndef Q_OS_ANDROID
#include <QtDBus/QDBusMessage>
#include <QDBusReply>
#endif
#include <QDialog>

enum  scrollingAttributes  { stateFix,scrollUp,scrollDown,scrollLeft,scrollRight,toggleScrollTransition };
enum  slidingAttributes  { nextEnabled,previousEnabled,toggleSlideTransition,crossedRange,vibrationIsSet };

AcceleratorScrollSlide::AcceleratorScrollSlide(QObject *parent) :QObject(parent)
{

    qDebug()<<"Accelerator starts";

    // ALL THE SCROLLING ATTRIBUTES SET TO RESPECTIVE STATES

    scrollingAttributes.resize(6);
    scrollingAttributes.setBit(stateFix,true);
    scrollingAttributes.setBit(toggleScrollTransition,true);
    scrollingAttributes.setBit(scrollUp,true);
    scrollingAttributes.setBit(scrollLeft,true);
    scrollingAttributes.setBit(scrollRight,true);
    scrollingAttributes.setBit(scrollDown,true);

    // ALL THE SLIDING ATTRIBUTES SET TO RESPECTIVE STATES

    slidingAttributes.resize(5);
    slidingAttributes.fill(false);

    stateFixValueX=0;
    stateFixValueY=0;
    countSteppedInRange=5;


}


AcceleratorScrollSlide::~AcceleratorScrollSlide()
{
    qDebug()<<"Accelertor ends";

}


/////////////////////////////////////////////
//////////       SLIDING       //////////////
//////////                     //////////////
/////////////////////////////////////////////

void AcceleratorScrollSlide::startSlideSettings()
{

    vibrationDialog = new QDialog;
    Q_CHECK_PTR(vibrationDialog);
    slideSetting=new QLabel("do you want vibration ?");
    Q_CHECK_PTR(slideSetting);
    spin = new QSpinBox;
    Q_CHECK_PTR(spin);
    spin->setRange(1,10);
    spin->setSingleStep(1);
    yesToVibration=new QPushButton("yes");
    Q_CHECK_PTR(yesToVibration);
    noToVibration=new QPushButton("no");
    Q_CHECK_PTR(noToVibration);
    vibrationLayout=new QVBoxLayout();
    Q_CHECK_PTR(vibrationLayout);
    vibrationLayout->addWidget(slideSetting);
    vibrationLayout->addWidget(spin);
    vibrationLayout->addWidget(yesToVibration);
    vibrationLayout->addWidget(noToVibration);
    vibrationDialog->setLayout(vibrationLayout);
    vibrationDialog->show();
    vibrationDialog->raise();

    disconnect(yesToVibration,SIGNAL(clicked()),this,SLOT(ifYesVibration()));
    connect(yesToVibration,SIGNAL(clicked()),this,SLOT(ifYesVibration()));

    disconnect(noToVibration,SIGNAL(clicked()),this,SLOT(ifNoVibration()));
    connect(noToVibration,SIGNAL(clicked()),this,SLOT(ifNoVibration()));
}

void AcceleratorScrollSlide::startRecognitionSlide()
{

#ifdef Q_WS_MAEMO_5

    qDebug()<<"Accelerator has begun";
    interfaceForAcceleratorSliding = new QDBusInterface(MCE_SERVICE,MCE_REQUEST_PATH,
                                                        MCE_REQUEST_IF, QDBusConnection::systemBus(),
                                                        this);
    Q_CHECK_PTR(interfaceForAcceleratorSliding);
    QDBusMessage replyAccelerator = interfaceForAcceleratorSliding->call(MCE_ACCELEROMETER_ENABLE_REQ);
    QDBusMessage replyVibrator = interfaceForAcceleratorSliding->call(MCE_ENABLE_VIBRATOR);

    if (replyAccelerator.type() == QDBusMessage::ErrorMessage) {
        qDebug() << replyAccelerator.errorMessage();
    }
    if (replyVibrator.type() == QDBusMessage::ErrorMessage)
        qDebug() << replyVibrator.errorMessage();

#endif
    vibrationLevelPatterns
            <<VIBRATIONLEVEL1
            <<VIBRATIONLEVEL2
            <<VIBRATIONLEVEL3
            <<VIBRATIONLEVEL4
            <<VIBRATIONLEVEL5
            <<VIBRATIONLEVEL6
            <<VIBRATIONLEVEL7
            <<VIBRATIONLEVEL8
            <<VIBRATIONLEVEL9
            <<VIBRATIONLEVEL10;

    slidingAttributes.setBit(toggleSlideTransition,true);

    QTimer *timerAcceleratorSliding=new QTimer(this);
    Q_CHECK_PTR(timerAcceleratorSliding);
    QObject::connect(timerAcceleratorSliding, SIGNAL(timeout()),this,SLOT(beginSliding()));
    timerAcceleratorSliding->start(1);


}


void AcceleratorScrollSlide::beginSliding()
{
    if(slidingAttributes.at(toggleSlideTransition))
    {

#ifdef Q_WS_MAEMO_5

        QDBusMessage replyForFindingCordinates=interfaceForAcceleratorSliding->call(MCE_DEVICE_ORIENTATION_GET);
        int xCordinate=replyForFindingCordinates.arguments().at(3).value<int>();

        //Geometry settings for the SLIDE TRANSITION using the STEERING option

        if(xCordinate>0){
            slidingAttributes.setBit(previousEnabled,true);                       //bent to left
            slidingAttributes.setBit(nextEnabled,false);
        }
        if(xCordinate<0){
            xCordinate=xCordinate*(-1);                                            //bent to right
            slidingAttributes.setBit(previousEnabled,false);
            slidingAttributes.setBit(nextEnabled,true);
        }
        if(xCordinate>LIMITFORENTERINGRANGE){
            slidingAttributes.setBit(crossedRange,true);                           //device bent into the range
            countSteppedInRange--;
            if(countSteppedInRange<4)
            {
                slidingAttributes.setBit(crossedRange,false);
            }
        }
        if(xCordinate<LIMITFOREXITINGRANGE){                                       //device exits from the range
            countSteppedInRange=5;
        }
        if(slidingAttributes.at(crossedRange))
        {

            slidingAttributes.setBit(crossedRange,false);


            QDBusMessage replyForVibratorController;
            if(slidingAttributes.at(nextEnabled)){
                if(slidingAttributes.at(vibrationIsSet))
                {
                    qDebug()<<"vibration is set vibrating";
                    replyForVibratorController =interfaceForAcceleratorSliding->call(MCE_ACTIVATE_VIBRATOR_PATTERN,vibrationLevelPatterns.at(vibrationValueLevel-1));
                    if (replyForVibratorController.type() == QDBusMessage::ErrorMessage) {
                        qDebug() << replyForVibratorController.errorMessage();
                    }
                }
                qDebug()<<"signal next is emitted ";
                emit next();

            }

            if(slidingAttributes.at(previousEnabled)) {
                if(slidingAttributes.at(vibrationIsSet))
                {
                    replyForVibratorController =interfaceForAcceleratorSliding->call(MCE_ACTIVATE_VIBRATOR_PATTERN,vibrationLevelPatterns.at(vibrationValueLevel-1));
                    if (replyForVibratorController.type() == QDBusMessage::ErrorMessage) {
                        qDebug() << replyForVibratorController.errorMessage();
                    }
                }
                qDebug()<<"signal previous is emitted ";
                emit previous();

            }

            slidingAttributes.setBit(nextEnabled,false);
            slidingAttributes.setBit(previousEnabled,false);
        }


        QDBusMessage replyForTheBlanking =interfaceForAcceleratorSliding->call(MCE_PREVENT_BLANK_REQ);
        if (replyForTheBlanking.type() == QDBusMessage::ErrorMessage) {
            qDebug() << replyForTheBlanking.errorMessage();
        }
#endif
    }
}

void AcceleratorScrollSlide::ifYesVibration()
{
    slidingAttributes.setBit(vibrationIsSet,true);
    vibrationValueLevel = spin->value();
    vibrationDialog->close();
    closeSlideDialog();
}


void AcceleratorScrollSlide::ifNoVibration()
{
    slidingAttributes.setBit(vibrationIsSet,false);
    vibrationDialog->close();
    closeSlideDialog();
}

void AcceleratorScrollSlide::closeSlideDialog()
{

    delete slideSetting;
    slideSetting=0;

    delete spin;
    spin=0;

    delete yesToVibration;
    yesToVibration=0;

    delete noToVibration;
    noToVibration=0;

    delete vibrationLayout;
    vibrationLayout=0;

    delete vibrationDialog;
    vibrationDialog=0;

}



void AcceleratorScrollSlide::stopRecognition()
{


    slidingAttributes.setBit(toggleSlideTransition,false);
    slidingAttributes.setBit(vibrationIsSet,false);
#ifdef Q_WS_MAEMO_5

    QDBusMessage replyAccelerometer;
    replyAccelerometer =  interfaceForAcceleratorSliding->call(MCE_ACCELEROMETER_DISABLE_REQ);
    if (replyAccelerometer.type() == QDBusMessage::ErrorMessage) {
        qDebug() <<replyAccelerometer.errorMessage();
    }
    qDebug()<<"distroyed accelerator sliding";

    QDBusMessage replyVibrator;
    replyVibrator =  interfaceForAcceleratorSliding->call(MCE_DISABLE_VIBRATOR);
    if (replyVibrator.type() == QDBusMessage::ErrorMessage) {
        qDebug() <<replyVibrator.errorMessage();
    }

    delete interfaceForAcceleratorSliding;
    interfaceForAcceleratorSliding=0;
#endif
}


///////////////////////////////////////////////////////
/////          ACCELERATOR SCROLLING          /////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

void AcceleratorScrollSlide::startScrollSettings()
{

    scrollSettingsDialog =new QDialog;
    Q_CHECK_PTR(scrollSettingsDialog);
    addScroll=new QLabel(" enable Accerelator scrolling?   (Ctrl+G)");
    Q_CHECK_PTR(addScroll);
    yesToScroll=new QPushButton("yes");
    Q_CHECK_PTR(yesToScroll);
    noToScroll=new QPushButton("no");
    Q_CHECK_PTR(noToScroll);
    scrollLayout=new QVBoxLayout();
    Q_CHECK_PTR(scrollLayout);
    scrollLayout->addWidget(addScroll);
    scrollLayout->addWidget(yesToScroll);
    scrollLayout->addWidget(noToScroll);
    scrollSettingsDialog->setLayout(scrollLayout);
    scrollSettingsDialog->show();
    scrollSettingsDialog->raise();

    disconnect(yesToScroll,SIGNAL(clicked()),this,SLOT(ifYesScroll()));
    connect(yesToScroll,SIGNAL(clicked()),this,SLOT(ifYesScroll()));

    disconnect(noToScroll,SIGNAL(clicked()),this,SLOT(ifNoScroll()));
    connect(noToScroll,SIGNAL(clicked()),this,SLOT(ifNoScroll()));

}

void AcceleratorScrollSlide::ifNoScroll()
{
#ifdef Q_WS_MAEMO_5
    MainWindow::stopAcceleratorScrolling=true;
#endif
    scrollSettingsDialog->close();
    closeScrollDialog();
#ifdef Q_WS_MAEMO_5
    if (MainWindow::enableScrolling) {

        emit stopTheAccelerator();
    }
#endif
}

void AcceleratorScrollSlide::ifYesScroll()
{
#ifdef Q_WS_MAEMO_5
    MainWindow::stopAcceleratorScrolling=false;
#endif
    scrollSettingsDialog->close();
    closeScrollDialog();
}

void AcceleratorScrollSlide::closeScrollDialog()
{

    delete addScroll;
    addScroll=0;

    delete yesToScroll;
    yesToScroll=0;

    delete noToScroll;
    noToScroll=0;

    delete scrollLayout;
    scrollLayout=0;


    delete scrollSettingsDialog;
    scrollSettingsDialog=0;

}



void AcceleratorScrollSlide::startRecognitionScroll()
{
    scrollingAttributes.setBit(toggleScrollTransition,true);
    scrollingAttributes.setBit(stateFix,true);
    verticalScrollValue=0;
    horizontalScrollValue=0;

#ifdef Q_WS_MAEMO_5

    interfaceForAcceleratorScrolling =new QDBusInterface(MCE_SERVICE, MCE_REQUEST_PATH,
                                                         MCE_REQUEST_IF, QDBusConnection::systemBus(),
                                                         this);
    Q_CHECK_PTR(interfaceForAcceleratorScrolling);

    QDBusMessage replyAccelerometer =interfaceForAcceleratorScrolling->call(MCE_ACCELEROMETER_ENABLE_REQ);
    if (replyAccelerometer.type() == QDBusMessage::ErrorMessage)
        qDebug() << replyAccelerometer.errorMessage();

    QDBusMessage replyVibrator =interfaceForAcceleratorScrolling->call(MCE_ENABLE_VIBRATOR);
    if (replyVibrator.type() == QDBusMessage::ErrorMessage)
        qDebug() << replyVibrator.errorMessage();
#endif

    QTimer *timerAcceleratorScrolling=new QTimer(this);
    Q_CHECK_PTR(timerAcceleratorScrolling);
    QObject::connect(timerAcceleratorScrolling, SIGNAL(timeout()),this,SLOT(beginScrolling()));
    timerAcceleratorScrolling->start(1);

}


void AcceleratorScrollSlide::beginScrolling()
{
#ifdef Q_WS_MAEMO_5

    if (scrollingAttributes.at(toggleScrollTransition)) {

        verticalScrollValue=0;
        horizontalScrollValue=0;


        QDBusMessage replyToGetCoOrdinates=interfaceForAcceleratorScrolling->call(MCE_DEVICE_ORIENTATION_GET);
        int xCordinate=replyToGetCoOrdinates.arguments().at(3).value<int>();
        int yCordinate=replyToGetCoOrdinates.arguments().at(4).value<int>();

        // Initial positions of the device are fixed
        // stateFix =Reading position

        if (scrollingAttributes.at(stateFix)) {
            scrollingAttributes.setBit(stateFix,false);
            stateFixValueX=xCordinate;
            stateFixValueY=yCordinate;
        }

        xCordinate=xCordinate-stateFixValueX;
        yCordinate=yCordinate-stateFixValueY;

        if (yCordinate<0) {
            yCordinate=yCordinate*(-1);
            scrollingAttributes.setBit(scrollUp,true);
            scrollingAttributes.setBit(scrollDown,false);
        }
        else {
            scrollingAttributes.setBit(scrollUp,false);
            scrollingAttributes.setBit(scrollDown,true);
        }

        if (xCordinate<0) {
            xCordinate=xCordinate*(-1);
            scrollingAttributes.setBit(scrollLeft,true);
            scrollingAttributes.setBit(scrollRight,false);
        }
        else {
            scrollingAttributes.setBit(scrollLeft,false);
            scrollingAttributes.setBit(scrollRight,true);
        }

        //    levels for the scorlling in both the horizontal
        //     and the vertical directions simultaneously


        //for the case of the vertical scrolling

        if (yCordinate>YCORDINATELIMITLEVEL1) {
            if(scrollingAttributes.at(scrollUp)) {
                verticalScrollValue+=3;
            }                                                        //very slow movement
            if(scrollingAttributes.at(scrollDown)) {                 //level1
                verticalScrollValue-=3;
            }
        }
        if (yCordinate>YCORDINATELIMITLEVEL2) {
            if(scrollingAttributes.at(scrollUp)) {
                verticalScrollValue+=6;
            }                                                        //level2
            if(scrollingAttributes.at(scrollDown)) {
                verticalScrollValue-=6;
            }

        }
        if (yCordinate>YCORDINATELIMITLEVEL3) {
            if(scrollingAttributes.at(scrollUp)) {
                verticalScrollValue+=10;
            }                                                        //level3
            if(scrollingAttributes.at(scrollDown)) {
                verticalScrollValue-=10;
            }

        }

        if (yCordinate>YCORDINATELIMITLEVEL4) {
            if(scrollingAttributes.at(scrollUp)) {
                verticalScrollValue+=15;
            }                                                        //level4
            if(scrollingAttributes.at(scrollDown)) {
                verticalScrollValue-=15;
            }
        }

        //for the case of the horizontal scrolling

        if (xCordinate>XCORDINATELIMITLEVEL1) {
            if(scrollingAttributes.at(scrollLeft)) {
                horizontalScrollValue+=3;                                   //very slow movement
            }                                                   //level 1
            if(scrollingAttributes.at(scrollRight)) {
                horizontalScrollValue-=3;
            }
        }
        if (xCordinate>XCORDINATELIMITLEVEL2) {
            if(scrollingAttributes.at(scrollLeft)) {
                horizontalScrollValue+=5;                                   //bending to level 2
            }                                                   //speed increased +5
            if(scrollingAttributes.at(scrollRight)) {
                horizontalScrollValue-=5;
            }
        }
        if (xCordinate>XCORDINATELIMITLEVEL1) {
            if(scrollingAttributes.at(scrollLeft)) {
                horizontalScrollValue+=10;                                   //bending to level 3
            }                                                    //speed increased +15
            if(scrollingAttributes.at(scrollRight)) {
                horizontalScrollValue-=10;
            }

        }

        if (xCordinate>XCORDINATELIMITLEVEL4) {
            if(scrollingAttributes.at(scrollLeft)) {
                horizontalScrollValue+=10;                                //bending to level 4
            }                                                 //speed increased +25
            if(scrollingAttributes.at(scrollRight)) {                                      //maximum speed
                horizontalScrollValue-=10;
            }

        }

    }


    //Change continually told to the main program";
    emit change();


    //During the scrolling screen is never blanked out";

    QDBusMessage replyBlanking = interfaceForAcceleratorScrolling->call(MCE_PREVENT_BLANK_REQ);
    if (replyBlanking.type() == QDBusMessage::ErrorMessage) {
        qDebug()<<replyBlanking.errorMessage();
    }
#endif
}

int AcceleratorScrollSlide::getHorizontalScrollValue()
{
    return horizontalScrollValue;
}

int AcceleratorScrollSlide::getVerticalScrollValue()
{
    return verticalScrollValue;
}

void AcceleratorScrollSlide::resetScrollValues()
{
    horizontalScrollValue=verticalScrollValue=0;
}

void AcceleratorScrollSlide::stopRecognitionScroll()
{
#ifdef Q_WS_MAEMO_5
    scrollingAttributes.setBit(toggleScrollTransition,false);
    verticalScrollValue=0;
    horizontalScrollValue=0;

    QDBusMessage replyAccelerometer;
    replyAccelerometer =  interfaceForAcceleratorScrolling->call(MCE_ACCELEROMETER_DISABLE_REQ);
    if (replyAccelerometer.type() == QDBusMessage::ErrorMessage) {
        qDebug() <<replyAccelerometer.errorMessage();
    }
    //distroyed accelerator scrolling

    QDBusMessage replyVibrator;
    replyVibrator =  interfaceForAcceleratorScrolling->call(MCE_DISABLE_VIBRATOR);
    if (replyVibrator.type() == QDBusMessage::ErrorMessage) {
        qDebug() <<replyAccelerometer.errorMessage();
    }
#endif
    //distroyed vibrator scrolling
}

