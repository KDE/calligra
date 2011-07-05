/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>
 *
 * Contact: Manikandaprasad N C <manikandaprasad.chandrasekar@nokia.com>
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <KoAbstractApplicationWindow.h>

class ApplicationController;
class Splash;

/*!
 * \brief Main window of the application. KoCanvasControllerWidget is set as
 * the central widget. It displays the loaded documents.
 */
class MainWindow : public QMainWindow, public KoAbstractApplicationWindow
{
    Q_OBJECT

public:
    MainWindow(Splash *aSplash, QWidget *parent = 0);
    ~MainWindow();
    //void tabletEvent ( QTabletEvent * event );
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

    /*!
     * Event filter to catch all mouse events to be able to properly show and hide
     * the fullscreen button when in fullscreen mode.
     */
    bool eventFilter(QObject *watched, QEvent *event);

private:
        ApplicationController *m_controller;
};

#endif // MAINWINDOW_H
