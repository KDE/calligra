/*
    Copyright (C) 2000, S.R.Haque <shaheedhaque@hotmail.com>.
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

DESCRIPTION
*/

#ifndef WMFIMPORT_H
#define WMFIMPORT_H

#include <koFilter.h>
#include <qobject.h>
#include <qstring.h>
#include <kwmf.h>

class WMFImport :
    public KoFilter, protected KWmf
{
    Q_OBJECT

public:
    WMFImport(
        KoFilter *parent,
        const char *name);
    virtual ~WMFImport();

    virtual bool filter(
        const QString &fileIn,
        const QString &fileOut,
        const QString &from,
        const QString &to,
        const QString &config = QString::null);

protected:

    virtual void gotEllipse(
        const DrawContext &dc,
        QString type,
        QPoint topLeft,
        QSize halfAxes,
        unsigned startAngle,
        unsigned stopAngle);
    virtual void gotPolygon(
        const DrawContext &dc,
        const QPointArray &points);
    virtual void gotPolyline(
        const DrawContext &dc,
        const QPointArray &points);
    virtual void gotRectangle(
        const DrawContext &dc,
        const QPointArray &points);

private:
    // Debug support.

    static const int s_area = 38000;

    QString m_text;
    void pointArray(
        const QPointArray &points);
};

#endif
