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
   Boston, MA 02110-1301, USA.
*/

#ifndef CDRODGWRITER_H
#define CDRODGWRITER_H

// Calligra
#include <KoOdfWriteStore.h>
#include <KoGenStyles.h>

class CdrDocument;
class CdrPage;
class CdrGroupObject;
class CdrRectangleObject;
class CdrEllipseObject;
class CdrPathObject;
class CdrTextObject;
class CdrBlockTextObject;
class CdrAbstractObject;
class CdrParagraph;
class CdrAbstractTransformation;

class KoXmlWriter;
class KoStore;
class KoGenStyle;

template<typename T> class QVector;
typedef qint16 CdrCoord;


class CdrOdgWriter
{
public:
    explicit CdrOdgWriter( KoStore* outputStore );
    ~CdrOdgWriter();

public:
    bool write( CdrDocument* document );

private:
    void storeThumbnailFile();
    void storePixelImageFiles();
    void storeSvgImageFiles();
    void storeMetaXml();
    void storeSettingsXml();
    void storeContentXml();

    void writeGraphicTextSvg( QIODevice* device, const CdrTextObject* object );

    void writeMasterPage();

    void writePage( const CdrPage* page );
    void writeObject( const CdrAbstractObject* object );
    void writeGroupObject( const CdrGroupObject* groupObject );
    void writeRectangleObject( const CdrRectangleObject* object );
    void writeEllipseObject( const CdrEllipseObject* object );
    void writePathObject( const CdrPathObject* pathObject );
    void writeTextObject( const CdrTextObject* object );
    void writeBlockTextObject( const CdrBlockTextObject* object );

    void writeParagraph( const CdrParagraph* paragraph );

    void writeFill( KoGenStyle& style, quint32 fillId );
    void writeStrokeColor( KoGenStyle& style, quint32 outlineId );
    void writeStrokeWidth( KoGenStyle& style, quint32 outlineId );
    void writeFont( KoGenStyle& style, quint16 styleId );

    void writeTransformation( const QVector<CdrAbstractTransformation*>& transformations );

    /// Returns the CDR x coord as ODF x coord (in pt)
    double odfXCoord( CdrCoord x ) const;
    /// Returns the CDR y coord as ODF y coord (in pt)
    double odfYCoord( CdrCoord y ) const;

private:
    KoOdfWriteStore mOdfWriteStore;
    KoStore* mOutputStore;
    KoXmlWriter* mManifestWriter;
    KoXmlWriter* mBodyWriter;

    KoGenStyles mStyleCollector;
    QString mMasterPageStyleName;
    int mPageCount;
    QString mLayerId;
    QHash<const CdrAbstractObject*,QString> mSvgFilePathByObject;

    CdrDocument* mDocument;
};

#endif
