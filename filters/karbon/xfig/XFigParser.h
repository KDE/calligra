/*
    This file is part of the Calligra project, made within the KDE community.

    Copyright 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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
    Boston, MA 02110-1301, USA.
*/

#ifndef XFIGPARSER_H
#define XFIGPARSER_H

// filter
#include "XFigStreamLineReader.h"

class XFigDocument;
class XFigAbstractObject;
class XFigArrowHead;
class QTextDecoder;
class QIODevice;
struct XFigPoint;

template<typename T> class QVector;


class XFigParser
{
public:
    static XFigDocument* parse( QIODevice* device );

private:
    explicit XFigParser( QIODevice* device );
    ~XFigParser();

    XFigDocument* takeDocument() { XFigDocument* result = m_Document; m_Document = 0; return result; }

    bool parseHeader();

    void parseColorObject();

    XFigAbstractObject* parseArc();
    XFigAbstractObject* parseEllipse();
    XFigAbstractObject* parsePolyline();
    XFigAbstractObject* parseSpline();
    XFigAbstractObject* parseText();
    XFigAbstractObject* parseCompoundObject();

    XFigArrowHead* parseArrowHead();
    QVector<XFigPoint> parsePoints(int pointCount);
    QVector<double> parseFactors(int pointCount);

private:
    XFigDocument* m_Document;

    int m_XFigVersion;

    QTextDecoder* m_TextDecoder;
    XFigStreamLineReader m_XFigStreamLineReader;
};

#endif
