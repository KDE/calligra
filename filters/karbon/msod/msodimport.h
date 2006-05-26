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
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

DESCRIPTION
*/

#ifndef MSODIMPORT_H
#define MSODIMPORT_H

#include <KoFilter.h>
#include <msod.h>
//Added by qt3to4:
#include <Q3PointArray>
#include <Q3CString>

class MSODImport :
    public KoEmbeddingFilter, protected Msod
{
    Q_OBJECT

public:
    MSODImport(
        QObject *parent,
        const QStringList&);
    virtual ~MSODImport();

    virtual KoFilter::ConversionStatus convert( const QByteArray& from, const QByteArray& to );

protected:

    virtual void gotEllipse(
        const DrawContext &dc,
        QString type,
        QPoint topLeft,
        QSize halfAxes,
        unsigned startAngle,
        unsigned stopAngle);
    virtual void gotPicture(
        unsigned id,
        QString extension,
        unsigned length,
        const char *data);
    virtual void gotPolygon(
        const DrawContext &dc,
        const QPolygon &points);
    virtual void gotPolyline(
        const DrawContext &dc,
        const QPolygon &points);
    virtual void gotRectangle(
        const DrawContext &dc,
        const QPolygon &points);

signals:
    // Communication signals to the parent filters
    void commSignalDelayStream( const char* delay );
    void commSignalShapeID( unsigned int& shapeID );

private:
    virtual void savePartContents( QIODevice* file );

    // Debug support.
    static const int s_area;

    void pointArray(
        const Q3PointArray &points);
    QString m_text;

    // Embedded objects.
    const char* m_embeddeeData;
    int m_embeddeeLength;
};

#endif
