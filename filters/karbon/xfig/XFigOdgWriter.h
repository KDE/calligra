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

#ifndef XFIGODGWRITER_H
#define XFIGODGWRITER_H

// Calligra
#include <KoOdfWriteStore.h>
#include <KoGenStyles.h>
// Qt
#include <QLocale>

class XFigDocument;
class XFigPage;
class XFigCompoundObject;
class XFigEllipseObject;
class XFigPolylineObject;
class XFigPolygonObject;
class XFigBoxObject;
class XFigPictureBoxObject;
class XFigSplineObject;
class XFigArcObject;
class XFigTextObject;
class XFigAbstractGraphObject;
class XFigAbstractObject;
class XFigLineable;
class XFigLineEndable;
class XFigFillable;
class XFigArrowHead;
struct XFigPoint;

class KoXmlWriter;
class KoStore;
class KoGenStyle;

template<typename T> class QVector;


class XFigOdgWriter
{
public:
    explicit XFigOdgWriter( KoStore* outputStore );
    ~XFigOdgWriter();

    bool write( XFigDocument* document );

private:
    enum LineEndType { LineStart, LineEnd };

    void storePixelImageFiles();
    void storeMetaXml();
    void storeContentXml();

    void writeMasterPage();

    void writePage( const XFigPage* page );
    void writeObject( const XFigAbstractObject* object );
    void writeCompoundObject( const XFigCompoundObject* groupObject );
    void writeEllipseObject( const XFigEllipseObject* object );
    void writePolylineObject( const XFigPolylineObject* object );
    void writePolygonObject( const XFigPolygonObject* object );
    void writeBoxObject( const XFigBoxObject* object );
    void writePictureBoxObject( const XFigPictureBoxObject* object );
    void writeSplineObject( const XFigSplineObject* pathObject );
    void writeArcObject( const XFigArcObject* pathObject );
    void writeTextObject( const XFigTextObject* object );

    void writeParagraphStyle( KoGenStyle& odfStyle, const XFigTextObject* textObject );
    void writeFont( KoGenStyle& odfStyle, const XFigTextObject* textObject );
    void writeFill( KoGenStyle& odfStyle, const XFigFillable* fillable, qint32 penColorId );
    void writeStroke( KoGenStyle& odfStyle, const XFigLineable* lineable );
    void writeDotDash( KoGenStyle& odfStyle, int lineType, double distance );
    void writeJoinType(KoGenStyle& odfStyle, int joinType);
    void writeCapType( KoGenStyle& odfStyle, const XFigLineEndable* lineEndable );
    void writeArrow(KoGenStyle& odfStyle, const XFigArrowHead* arrow, LineEndType lineEndType);
    void writeHatch(KoGenStyle& odfStyle, int patternType, const QString& colorString);

    void writeComment(const XFigAbstractObject* object);
    void writePoints( const QVector<XFigPoint>& points );
    /// Writes z-index and sets layer name
    void writeZIndex( const XFigAbstractGraphObject* graphObject );

    /// Returns the XFig x coord as ODF x coord (in pt)
    double odfXCoord( qint32 x ) const;
    /// Returns the XFig y coord as ODF y coord (in pt)
    double odfYCoord( qint32 y ) const;
    /// Returns the XFig length as ODF length (in pt)
    double odfLength( qint32 length ) const;
    double odfLength( double length ) const;

private:
    QLocale m_CLocale;
    KoOdfWriteStore m_OdfWriteStore;
    KoStore* m_OutputStore;
    KoXmlWriter* m_ManifestWriter;
    KoXmlWriter* m_BodyWriter;

    KoGenStyles m_StyleCollector;
    QString m_MasterPageStyleName;
    int m_PageCount;

    XFigDocument* m_Document;
};

#endif
