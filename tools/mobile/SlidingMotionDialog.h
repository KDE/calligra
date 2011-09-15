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

#ifndef SLIDINGMOTIONDIALOG_H
#define SLIDINGMOTIONDIALOG_H
#include <QFrame>
#include <QDialog>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>

#include "MainWindow.h"

class QGridLayout;
class QPushButton;
class QString;
class MainWindow;

class SlidingMotionDialog:public QMainWindow
{
 Q_OBJECT

public:
    SlidingMotionDialog(QWidget *parent = 0);
    ~SlidingMotionDialog();
    /*!
     * Sliding Motion Frame declaration
     */
    QDialog * m_slidingmotionframe;
    QGridLayout * m_slidingmotionframelayout;
    QPushButton * m_opengl;
    QPushButton * m_acceleration;
    QLabel * m_slidetransitionlabel;
    QLabel * m_timerlabel;
    QComboBox * m_slidetransitionstyle;
    QSpinBox * m_slidetransitiontime;
    int m_select;
    int m_time;

private:
    QPushButton * addFrameComponent(const QString &imagepath);

public:
    void showDialog(MainWindow *main);

public slots:
    void deleteDialog();
    void prepglshow();

signals:
    void startglshow(int,int);

};

#endif // SLIDINGMOTIONDIALOG_H

