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

#ifndef MSODIMPORT_H
#define MSODIMPORT_H

#include <koFilter.h>
#include <qmap.h>
#include <qobject.h>
#include <qstring.h>
#include <msod.h>

class MSODImport :
    public KoFilter, protected Msod
{
    Q_OBJECT

public:
    MSODImport(
        KoFilter *parent,
        const char *name);
    virtual ~MSODImport();

    virtual const bool filter1(
        const QString &fileIn,
        const QString &fileOut,
        const QString &prefixOut,
        const QString &from,
        const QString &to,
        const QString &config = QString::null);
    virtual bool supportsEmbedding() { return true; }

protected:

    virtual void gotPicture(
        unsigned id,
        QString extension,
        unsigned length,
        const char *data);
    virtual void gotPolygon(
        unsigned penColour,
        unsigned penStyle,
        unsigned penWidth,
        unsigned brushColour,
        unsigned brushStyle,
        const QPointArray &points);
    virtual void gotPolyline(
        unsigned penColour,
        unsigned penStyle,
        unsigned penWidth,
        const QPointArray &points);

private:
    // Debug support.

    static const int s_area = 30505;

    void pointArray(
        const QPointArray &points);
    QString m_text;

    // Embedded objects.

    class Part
    {
    public:
        QString mimeType;
        QString storageName;
        QString file;
    };

    QMap<unsigned, Part> m_parts;
    QString m_prefixOut;
    int m_nextPart;
};

#endif
