/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
 *
 * Contact: Gopalakrishna Bhat A <gopalakbhat@gmail.com>
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

#ifndef PRESENTATIONTOOL_H
#define PRESENTATIONTOOL_H

#include "MainWindow.h"
#include "KoCanvasControllerWidget.h"
#ifndef Q_OS_ANDROID
#include "PresentationToolAdaptor.h"
#endif

#include <QPoint>

class MainWindow;
class PresentationToolAdaptor;

class PresentationTool : public QObject
{
   Q_OBJECT

public :
    PresentationTool( MainWindow * window, KoCanvasControllerWidget * controller );
    ~PresentationTool();

    /** Get all the mouse event needed to paint */
    void handleMainWindowMousePressEvent( QMouseEvent * e );
    void handleMainWindowMouseMoveEvent( QMouseEvent * e );
    void handleMainWindowMouseReleaseEvent( QMouseEvent * e );

    /*!
     * get the status of various tools
     */
    bool getPenToolStatus();
    bool getHighlightToolStatus();
    bool toolsActivated();

    /*!
     * The image of the current slide
     */
    QImage *getImage();

public slots:
    /*!
     * Activate or deactivate ceratain tools
     */
    void togglePenTool();
    void toggleHighlightTool();
    void deactivateTool();

private :
    KoCanvasControllerWidget *m_controller;
    MainWindow *m_window;

    /*!
     * Holds the image map of the current state of
     * current slide
     */
    QImage *image;
    QImage *image1;

    /*!
     * The state-variables for various tools
     */
    bool m_penToolActivated;
    bool m_highlightToolActivated;
    bool scribbling; // XXX: always use m_ prefix!

    /*!
     * The points that are used by the pen tool
     */
    QPoint lastPoint;
    QPoint endPoint;

#ifndef Q_OS_ANDROID
    PresentationToolAdaptor *m_dbus;
#endif

    /*!
     * Draw a line to a particular point
     */
    void drawLineTo( const QPoint &endPoint );

    /*!
     * Draw a ellipse at this particular point
     */
    void drawEllipse( const QPoint &cpoint );

signals:
    /*!
     * Draw on presentation started at (pointx,pointy).
     */
    void startDrawPresentation(int pointx,int pointy);
    /*!
     * Drawing on presentation tool.
     * The parameters pointx and pointy specify a point in the path.
     */
    void drawOnPresentation(int pointx,int pointy);
    /*!
     * Drawing path stopped. The current path is stopped.
     */
    void stopDrawPresentation ();
    /*!
     * Presentation highlighted at (pointx, pointy).
     */
    void highlightPresentation(int pointx, int pointy);
    /*!
     * Normal presentation mode.
     */
    void normalPresentation();
};

#endif // PRESENTATIONTOOL_H
