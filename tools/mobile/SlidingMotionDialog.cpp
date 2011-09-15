/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Pratik Vyas <pratikvyas10@gmail.com>
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
 */

#include "SlidingMotionDialog.h"
#include <QFrame>
#include <QPushButton>
#include <QGridLayout>
#include <QString>
#include <QDialog>

SlidingMotionDialog::SlidingMotionDialog(QWidget */*parent*/)
    : m_slidingmotionframe(0),
    m_slidingmotionframelayout(0),
    m_opengl(0),
    m_acceleration(0),
    m_slidetransitionlabel(0),
    m_timerlabel(0),
    m_slidetransitionstyle(0),
    m_slidetransitiontime(0)
{
}

SlidingMotionDialog::~SlidingMotionDialog(){}

void SlidingMotionDialog::showDialog(MainWindow *main){

    m_slidingmotionframe = new QDialog(main);
    Q_CHECK_PTR(m_slidingmotionframe);

    m_slidingmotionframelayout = new QGridLayout;
    Q_CHECK_PTR(m_slidingmotionframelayout);
    m_slidingmotionframelayout->setVerticalSpacing(0);
    m_slidingmotionframelayout->setHorizontalSpacing(0);

    m_opengl=addFrameComponent("slide");
    m_acceleration=addFrameComponent("scroll");

    m_slidetransitionlabel = new QLabel(this);
    m_slidetransitionlabel->setPixmap(QPixmap(":/images/64x64/Acceleration/Presentation.png"));
    m_slidetransitionlabel->setToolTip("SlideTransitionType");
    m_slidetransitionlabel->setAlignment(Qt::AlignRight);

    m_timerlabel = new QLabel(this);
    m_timerlabel->setToolTip("Timer");
    m_timerlabel->setAlignment(Qt::AlignRight);
    m_timerlabel->setPixmap(QPixmap(":/images/64x64/Acceleration/TimerClock.png"));

    m_slidetransitionstyle = new QComboBox(this);
    Q_CHECK_PTR(m_slidetransitionstyle);
    m_slidetransitiontime = new QSpinBox(this);
    Q_CHECK_PTR(m_slidetransitiontime);

    m_slidetransitiontime->setSuffix(" Seconds");
    m_slidetransitiontime->setValue(m_time);

    m_slidetransitionstyle->addItem("Simple");
    m_slidetransitionstyle->addItem("Cubical moves");
    m_slidetransitionstyle->addItem("Wave motion");
    m_slidetransitionstyle->addItem("Jumping slides");
    m_slidingmotionframe->setWindowTitle("Slide Transition Options");
    m_slidetransitionstyle->setCurrentIndex(m_select);
    connect(m_slidetransitionstyle,SIGNAL(activated(int)),SLOT(prepglshow()));
    connect(m_slidetransitiontime,SIGNAL(valueChanged(int)),SLOT(prepglshow()));

    m_slidingmotionframelayout->addWidget(m_opengl,0,0);
    m_slidingmotionframelayout->addWidget(m_slidetransitionlabel,0,1);
    m_slidingmotionframelayout->addWidget(m_slidetransitionstyle,0,2);

    m_slidingmotionframelayout->addWidget(m_acceleration,1,0);
    m_slidingmotionframelayout->addWidget(m_timerlabel,1,1);
    m_slidingmotionframelayout->addWidget(m_slidetransitiontime,1,2);

    m_slidingmotionframe->setLayout(m_slidingmotionframelayout);
    m_slidingmotionframe->raise();
    m_slidingmotionframe->show();
}

void SlidingMotionDialog::deleteDialog(){
    if(m_slidingmotionframe){
        m_slidingmotionframe->close();
        delete m_slidingmotionframelayout;
        m_slidingmotionframelayout = 0;
        delete m_opengl;
        m_opengl = 0;
        delete m_acceleration;
        m_acceleration = 0;
        delete m_slidingmotionframe;
        m_slidingmotionframe = 0;
    }
}

QPushButton * SlidingMotionDialog::addFrameComponent(const QString &imagepath){
    QPushButton *btn = new QPushButton(imagepath,this);
    Q_CHECK_PTR(btn);
    return btn;
}

void SlidingMotionDialog::prepglshow(){
    if(m_slidetransitiontime->value()<7){
        m_slidetransitiontime->setValue(7);
    }
    emit startglshow(m_slidetransitionstyle->currentIndex(),m_slidetransitiontime->value());
}
