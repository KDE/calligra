/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PRESENTATIONTOOLADAPTOR_H
#define PRESENTATIONTOOLADAPTOR_H

#include <QColor>
#include <QObject>
#include <QtDBus>
#include <QDBusAbstractAdaptor>

class MainWindow;
class PresentationTool;

class PresentationToolAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.nokia.CalligraMobile.presentation.tool")

public:
    PresentationToolAdaptor(PresentationTool *tool);
    ~PresentationToolAdaptor();

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

private:
    PresentationTool *m_tool;
};

#endif
