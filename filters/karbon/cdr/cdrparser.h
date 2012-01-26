/* This file is part of the Calligra project, made within the KDE community.

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef CDRPARSER_H
#define CDRPARSER_H

// filter
#include "cdrstructs.h"
// Koralle
#include <Koralle0/RiffStreamReader>

class KarbonDocument;
class KoShapeGroup;
class KoPathShape;
class KoShape;
class QPointF;
class QFile;
template<class T> class QList;


class CdrParser
{
public:
    CdrParser();
    ~CdrParser();

public:
    bool parse( KarbonDocument* output, QFile& input );

private:
    void readCDR();
    void readVersion();
    void readDisp();
    void readInfo();
    void readDoc();
    void readPage();

    void readDocStsh();
    void readDocMCfg();
    void readDocFontTable();
    void readDocBitmapTable();
    void readDocFillTable();
    void readDocOutlineTable();
    void readDocStyleTable();
    void readDocStyle();

    void readPageFlags();

    void readLayer();
    void readLayerFlags();
    void readLayerLGOb();

    KoShapeGroup* readObjectGroup();
    void readObjectGroupFlags();

    KoShape* readObject();
    void readObjectFlags();
    KoShape* readObjectLGOb();

    void readTrfl();
    KoShape* readLoda();

    KoPathShape* readPathObject( const CdrArgumentData* argsData );

    qreal koXCoord( cdr4Coord cdrCoord ) const;
    qreal koYCoord( cdr4Coord cdrCoord ) const;
    QPointF koCoords( Cdr4Point cdrCoords ) const;

private:
    KarbonDocument* mDocument;

    int mCdrVersion;

    Koralle::RiffStreamReader mRiffStreamReader;
};

#endif


