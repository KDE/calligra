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

#ifndef ACCELERATOR_H
#define ACCELERATOR_H

#include <QObject>
#include <QtDebug>
#include <QTimer>
#include <QtDBus>
#include <QVBoxLayout>
#include <QBitArray>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QRadioButton>

#ifdef Q_WS_MAEMO_5
#include <mce/dbus-names.h>
#endif


class AcceleratorScrollSlide:public QObject
{
    Q_OBJECT

public:

    AcceleratorScrollSlide(QObject *parent=0);
    ~AcceleratorScrollSlide();
public:


    void  startRecognitionSlide();
    void  closeSlideDialog();
    void  closeScrollDialog();
    int   getVerticalScrollValue();
    int   getHorizontalScrollValue();
    void  resetScrollValues();

signals:

    void next();
    void previous();
    void change();
    void stopTheAccelerator();

private:
/*!
 * FOR SCROLLING
 */
    QBitArray scrollingAttributes;
    int  stateFixValueX;
    int  stateFixValueY;
    int verticalScrollValue;
    int horizontalScrollValue;
#ifdef Q_WS_MAEMO_5

    QDBusInterface *interfaceForAcceleratorScrolling;

#endif


    QDialog *scrollSettingsDialog;
    QVBoxLayout *scrollLayout;
    QPushButton *yesToScroll;
    QPushButton *noToScroll;
    QLabel *addScroll;

/*!
 * FOR SLIDING
 */
    QStringList vibrationLevelPatterns;
    QBitArray slidingAttributes;
    int  countSteppedInRange;
    int vibrationValueLevel;

#ifdef Q_WS_MAEMO_5

    QDBusInterface *interfaceForAcceleratorSliding;

#endif

    QDialog *vibrationDialog;
    QVBoxLayout *vibrationLayout;
    QPushButton *yesToVibration;
    QPushButton *noToVibration;
    QSpinBox *spin;
    QLabel *slideSetting;



private slots:
/*!
 * FOR SCROLL
 */
    void  startScrollSettings();
    void  beginScrolling();

/*!
 * FOR SLID
 */
    void  startSlideSettings();
    void  beginSliding();
    void  ifYesVibration();
    void  ifNoVibration();


public slots:

    void  stopRecognition();
    void  stopRecognitionScroll();
    void  startRecognitionScroll();
    void  ifYesScroll();
    void  ifNoScroll();


};
#endif // ACCELERATOR_H
