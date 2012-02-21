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

// Koralle
#include <Koralle0/RiffStreamReader>

class CdrArgumentWithTypeListData;
class CdrDocument;
class CdrPage;
class CdrLayer;
class CdrParagraph;
class CdrAbstractTransformation;
class CdrLinkGroupObject;
class CdrGroupObject;
class CdrEllipseObject;
class CdrRectangleObject;
class CdrPathObject;
class CdrGraphicTextObject;
class CdrBlockTextObject;
class CdrAbstractObject;

class QPointF;
class QFile;
template<typename T> class QVector;


class CdrParser
{
public:
    CdrParser();
    ~CdrParser();

public:
    CdrDocument* parse( QFile& input );

private:
    void readCDR();
    void readVersion();
    void readDisp();
    void readInfo();
    void readDoc();
    CdrPage* readPage();

    void readDocStsh();
    void readDocMCfg();
    void readDocGuid();
    void readDocFontTable();
    void readDocLnkTable();
    void readDocBitmapTable();
    void readDocVecTable();
    void readDocFillTable();
    void readDocOutlineTable();
    void readDocStyleTable();
    void readDocBtxTable();
    void readStrl();
    CdrParagraph* readParl();

    CdrLayer* readLayer();
    void readLayerLGOb();

    CdrLinkGroupObject* readLinkGroupObject();
    CdrGroupObject* readGroupObject();

    void readGroupObjectLGOb( CdrGroupObject* group );
    CdrAbstractObject* readObject();
    CdrAbstractObject* readObjectLGOb();

    QVector<CdrAbstractTransformation*> readTrfl();
    CdrAbstractObject* readLoda();

    CdrRectangleObject* readRectangleObject( const CdrArgumentWithTypeListData& argsData );
    CdrEllipseObject* readEllipseObject( const CdrArgumentWithTypeListData& argsData );
    CdrPathObject* readPathObject( const CdrArgumentWithTypeListData& argsData );
    CdrGraphicTextObject* readGraphicTextObject( const CdrArgumentWithTypeListData& argsData );
    CdrBlockTextObject* readBlockTextObject( const CdrArgumentWithTypeListData& argsData );

private:
    CdrDocument* mDocument;

    int mCdrVersion;

    Koralle::RiffStreamReader mRiffStreamReader;
};

#endif
