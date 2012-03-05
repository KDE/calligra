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

// Qt
#include <QTextStream>

class XFigDocument;
class XFigAbstractObject;
class QTextDecoder;
class QIODevice;


class XFigParser
{
public:
    static XFigDocument* parse( QIODevice* device );

private:
    explicit XFigParser( QIODevice* device );
    ~XFigParser();

    XFigDocument* takeDocument() { XFigDocument* result = mDocument; mDocument = 0; return result; }

private:
    bool parseHeader();

    void parseColorObject();

    XFigAbstractObject* parseArc();
    XFigAbstractObject* parseEllipse();
    XFigAbstractObject* parsePolyline();
    XFigAbstractObject* parseSpline();
    XFigAbstractObject* parseText();
    XFigAbstractObject* parseCompoundObject();

private:
    XFigDocument* mDocument;

    int mXFigVersion;

    QTextDecoder* mTextDecoder;
    QTextStream mTextStream;
};

#endif
