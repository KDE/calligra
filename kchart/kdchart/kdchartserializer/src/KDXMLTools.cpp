/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klaralvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDChart library.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License versions 2.0 or 3.0 as published by the Free Software
 ** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file.  Alternatively you may (at
 ** your option) use any later version of the GNU General Public
 ** License if such license has been publicly approved by
 ** Klarälvdalens Datakonsult AB (or its successors, if any).
 ** 
 ** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
 ** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE. Klarälvdalens Datakonsult AB reserves all rights
 ** not expressly granted herein.
 ** 
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 **********************************************************************/
#include "KDXMLTools.h"

#include <QBuffer>
#include <QByteArray>
#include <QImage>
#include <QPoint>
#include <QSize>
#include <QVariant>
#include <QImageWriter>

#include <QDebug>



// For some reason the ::compress algorithm is producing a list
// of Zero bytes onye, so we disable compression for now
// (khz, 03 27 2007)
//
// Also note: There is no zlib.h there on Windows.
#define DO_NOT_COMPRESS_PIXMAP_DATA


#ifndef DO_NOT_COMPRESS_PIXMAP_DATA
#include <zlib.h>
#endif



namespace KDXML {

    void setBoolAttribute( QDomElement& element,
                           const QString& attrName, bool value )
    {
        element.setAttribute( attrName, value ? "true" : "false" );
    }

    void setDoubleAttribute( QDomElement& element,
                             const QString& attrName, double value )
    {
        element.setAttribute( attrName, value );
    }

    void createNodeWithAttribute( QDomDocument& doc, QDomNode& parent,
                                  const QString& elementName,
                                  const QString& attrName,
                                  const QString& attrText )
    {
        QDomElement newElement =
            doc.createElement( elementName );
        parent.appendChild( newElement );
        newElement.setAttribute( attrName, attrText );
    }

    void createBoolNode( QDomDocument& doc, QDomNode& parent,
                         const QString& elementName, bool value )
    {
        QDomElement newElement =
            doc.createElement( elementName );
        parent.appendChild( newElement );
        QDomText elementContent =
            doc.createTextNode( value ? "true" : "false" );
        newElement.appendChild( elementContent );
    }


    void createOrientationNode( QDomDocument& doc, QDomNode& parent,
                                const QString& elementName, Qt::Orientation value )
    {
        QDomElement newElement =
            doc.createElement( elementName );
        parent.appendChild( newElement );
        QDomText elementContent =
            doc.createTextNode( (Qt::Vertical == value) ? "vertical" : "horizontal" );
        newElement.appendChild( elementContent );
    }

    void createAlignmentNode( QDomDocument& doc, QDomNode& parent,
                              const QString& elementName, Qt::Alignment value )
    {
        //qDebug() << "store alignment:" << value;
        QDomElement newElement =
            doc.createElement( elementName );
        parent.appendChild( newElement );
        if( Qt::AlignLeft & value )
            newElement.setAttribute( "Horizontal", "Left" );
        else if( Qt::AlignRight & value )
            newElement.setAttribute( "Horizontal", "Right" );
        else if( Qt::AlignHCenter & value )
            newElement.setAttribute( "Horizontal", "Center" );
        else if( Qt::AlignJustify & value )
            newElement.setAttribute( "Horizontal", "Justify" );
        if( Qt::AlignTop & value )
            newElement.setAttribute( "Vertical", "Top" );
        else if( Qt::AlignBottom & value )
            newElement.setAttribute( "Vertical", "Bottom" );
        else if( Qt::AlignVCenter & value )
            newElement.setAttribute( "Vertical", "Center" );
    }

    void createBoxLayoutDirectionNode( QDomDocument& doc, QDomNode& parent,
                                       const QString& elementName, QBoxLayout::Direction value )
    {
        QDomElement newElement =
            doc.createElement( elementName );
        parent.appendChild( newElement );
        if( value == QBoxLayout::LeftToRight)
            newElement.setAttribute( "Direction", "LeftToRight" );
        else if( value == QBoxLayout::RightToLeft)
            newElement.setAttribute( "Direction", "RightToLeft" );
        else if( value == QBoxLayout::TopToBottom)
            newElement.setAttribute( "Direction", "TopToBottom" );
        else if( value == QBoxLayout::BottomToTop)
            newElement.setAttribute( "Direction", "BottomToTop" );
    }


    void createSizeNode( QDomDocument& doc, QDomNode& parent,
                         const QString& elementName, const QSize& value )
    {
        QDomElement newElement =
            doc.createElement( elementName );
        parent.appendChild( newElement );
        setBoolAttribute( newElement, "Valid", value.isValid() );
        if( value.isValid() ){
            newElement.setAttribute( "Width", value.width() );
            newElement.setAttribute( "Height", value.height() );
        }
    }


    void createSizeFNode( QDomDocument& doc, QDomNode& parent,
                          const QString& elementName, const QSizeF& value )
    {
        QDomElement newElement =
            doc.createElement( elementName );
        parent.appendChild( newElement );
        setBoolAttribute( newElement, "Valid", value.isValid() );
        if( value.isValid() ){
            newElement.setAttribute( "Width", value.width() );
            newElement.setAttribute( "Height", value.height() );
        }
    }


    void createIntNode( QDomDocument& doc, QDomNode& parent,
                        const QString& elementName, int value )
    {
        QDomElement newElement =
            doc.createElement( elementName );
        parent.appendChild( newElement );
        QDomText elementContent =
            doc.createTextNode( QString::number( value ) );
        newElement.appendChild( elementContent );
    }


    void createDoubleNode( QDomDocument& doc, QDomNode& parent,
                           const QString& elementName, double value )
    {
        QDomElement newElement =
            doc.createElement( elementName );
        parent.appendChild( newElement );
        QDomText elementContent =
            doc.createTextNode( QString::number( value ) );
        newElement.appendChild( elementContent );
    }


    void createRealNode( QDomDocument& doc, QDomNode& parent,
                         const QString& elementName, qreal value )
    {
        QDomElement newElement =
            doc.createElement( elementName );
        parent.appendChild( newElement );
        QDomText elementContent =
            doc.createTextNode( QString::number( value ) );
        newElement.appendChild( elementContent );
    }


    void createStringNode( QDomDocument& doc, QDomNode& parent,
                           const QString& elementName,
                           const QString& text )
    {
        QDomElement newElement =
            doc.createElement( elementName );
        parent.appendChild( newElement );
        QDomText elementContent =
            doc.createTextNode( text );
        newElement.appendChild( elementContent );
    }

    void createStringNodeIfContent( QDomDocument& doc, QDomNode& parent,
                                    const QString& elementName,
                                    const QString& text )
    {
        if( ! text.isEmpty() )
            createStringNode( doc, parent, elementName, text );
    }

    void createPointFNode( QDomDocument& doc, QDomNode& parent,
                           const QString& elementName, const QPointF& point )
    {
        QDomElement element = doc.createElement( elementName );
        parent.appendChild( element );
        element.setAttribute( "x", QString::number( point.x() ) );
        element.setAttribute( "y", QString::number( point.y() ) );
    }

    void createColorNode( QDomDocument& doc, QDomNode& parent,
                          const QString& elementName, const QColor& color )
    {
        QDomElement colorElement = doc.createElement( elementName );
        parent.appendChild( colorElement );
        setBoolAttribute( colorElement, "Valid", color.isValid() );
        if( color.isValid() ){
            colorElement.setAttribute( "Red",
                                       QString::number( color.red() ) );
            colorElement.setAttribute( "Green",
                                       QString::number( color.green() ) );
            colorElement.setAttribute( "Blue",
                                       QString::number( color.blue() ) );
            colorElement.setAttribute( "Alpha",
                                       QString::number( color.alpha() ) );
        }
    }


    void createBrushNode( QDomDocument& doc, QDomNode& parent,
                          const QString& elementName, const QBrush& brush )

    {
        QDomElement brushElement = doc.createElement( elementName );
        parent.appendChild( brushElement );
        createColorNode( doc, brushElement, "Color", brush.color() );
        createStringNode( doc, brushElement, "Style",
                          KDXML::brushStyleToString( brush.style() ) );
        QPixmap pix( brush.texture() );
        if( ! pix.isNull() )
            createPixmapNode( doc, brushElement, "Pixmap", pix );
    }


    void createPixmapNode( QDomDocument& doc, QDomNode& parent,
                           const QString& elementName, const QPixmap& pixmap )
    {
        QDomElement pixmapElement = doc.createElement( elementName );
        parent.appendChild( pixmapElement );

        createStringNode( doc, pixmapElement, "Format", "XPM.GZ" );
        if( pixmap.isNull() ){
            // we store an empty image without any data
            createIntNode( doc, pixmapElement, "Length", 0 );
        }else{
            // Convert the pixmap to an image, save that image to an in-memory
            // XPM representation and compress this representation. This
            // conforms to the file format Qt Designer uses.
            QByteArray ba;
            QBuffer buffer( &ba );
            buffer.open( QIODevice::WriteOnly );
            QImageWriter imgio( &buffer, "XPM" );
            QImage image( pixmap.toImage() );
            imgio.write( image );
            buffer.close();
            ulong len = ba.size() * 2;
            QByteArray bazip;
            bazip.reserve( len );
#ifdef DO_NOT_COMPRESS_PIXMAP_DATA
            bazip = ba;
#else
            ::compress(  (uchar*) bazip.data(), &len, (uchar*) ba.data(), ba.size() );
#endif
            QString dataString;
            static const char hexchars[] = "0123456789abcdef";
            for ( int i = 0; i < (int)len; ++i ) {
                uchar c = (uchar) bazip[i];
                dataString += hexchars[c >> 4];
                dataString += hexchars[c & 0x0f];
            }
            createIntNode( doc, pixmapElement, "Length", ba.size() );
            createStringNode( doc, pixmapElement, "Data", dataString );
        }
    }


    void createRectNode( QDomDocument& doc, QDomNode& parent,
                         const QString& elementName, const QRect& rect )
    {
        QDomElement rectElement = doc.createElement( elementName );
        parent.appendChild( rectElement );
        QDomElement xElement = doc.createElement( "X" );
        rectElement.appendChild( xElement );
        QDomText xContent = doc.createTextNode( QString::number( rect.x() ) );
        xElement.appendChild( xContent );
        QDomElement yElement = doc.createElement( "Y" );
        rectElement.appendChild( yElement );
        QDomText yContent = doc.createTextNode( QString::number( rect.y() ) );
        yElement.appendChild( yContent );
        QDomElement widthElement = doc.createElement( "Width" );
        rectElement.appendChild( widthElement );
        QDomText widthContent = doc.createTextNode( QString::number( rect.width() ) );
        widthElement.appendChild( widthContent );
        QDomElement heightElement = doc.createElement( "Height" );
        rectElement.appendChild( heightElement );
        QDomText heightContent = doc.createTextNode( QString::number( rect.height() ) );
        heightElement.appendChild( heightContent );
    }


    void createStringListNodes( QDomDocument& doc, QDomNode& parent,
                                const QString& elementName,
                                const QStringList* list )
    {
        if( !list )
            return;

        QDomElement element = doc.createElement( elementName );
        parent.appendChild( element );
        for( QStringList::ConstIterator it = list->begin();
             it != list->end(); ++it ) {
            QDomText elementContent = doc.createTextNode( *it );
            element.appendChild( elementContent );
        }
    }


    void createFontNode( QDomDocument& doc, QDomNode& parent,
                         const QString& elementName, const QFont& font )
    {
        QDomElement fontElement = doc.createElement( elementName );
        parent.appendChild( fontElement );
        createStringNode( doc, fontElement, "Family", font.family() );
        createRealNode( doc, fontElement, "PointSize", font.pointSizeF() );
        createIntNode( doc, fontElement, "Weight", font.weight() );
        createBoolNode( doc, fontElement, "Italic", font.italic() );
    }


    void createPenNode( QDomDocument& doc, QDomNode& parent,
                        const QString& elementName, const QPen& pen )
    {
        QDomElement penElement = doc.createElement( elementName );
        parent.appendChild( penElement );
        createIntNode( doc, penElement, "Width", pen.width() );
        createBrushNode( doc, penElement, "Brush", pen.brush() );
        createColorNode( doc, penElement, "Color", pen.color() );
        createStringNode( doc, penElement, "Style",     penStyleToString(     pen.style() ) );
        createStringNode( doc, penElement, "CapStyle",  penCapStyleToString(  pen.capStyle() ) );
        createStringNode( doc, penElement, "JoinStyle", penJoinStyleToString( pen.joinStyle() ) );
    }


    void createDateTimeNode( QDomDocument& doc, QDomNode& parent,
                             const QString& elementName,
                             const QDateTime& datetime )
    {
        QDomElement dateTimeElement = doc.createElement( elementName );
        parent.appendChild( dateTimeElement );
        setBoolAttribute( dateTimeElement, "Valid", datetime.isValid() );
        if( datetime.isValid() ){
            createDateNode( doc, dateTimeElement, "Date", datetime.date() );
            createTimeNode( doc, dateTimeElement, "Time", datetime.time() );
        }
    }


    void createDateNode( QDomDocument& doc, QDomNode& parent,
                         const QString& elementName, const QDate& date )
    {
        QDomElement dateElement = doc.createElement( elementName );
        parent.appendChild( dateElement );
        setBoolAttribute( dateElement, "Valid", date.isValid() );
        if( date.isValid() ){
            dateElement.setAttribute( "Year", QString::number( date.year() ) );
            dateElement.setAttribute( "Month", QString::number( date.month() ) );
            dateElement.setAttribute( "Day", QString::number( date.day() ) );
        }
    }


    void createTimeNode( QDomDocument& doc, QDomNode& parent,
                         const QString& elementName, const QTime& time )
    {
        QDomElement timeElement = doc.createElement( elementName );
        parent.appendChild( timeElement );
        setBoolAttribute( timeElement, "Valid", time.isValid() );
        if( time.isValid() ){
            timeElement.setAttribute( "Hour",
                                      QString::number( time.hour() ) );
            timeElement.setAttribute( "Minute",
                                      QString::number( time.minute() ) );
            timeElement.setAttribute( "Second",
                                      QString::number( time.second() ) );
            timeElement.setAttribute( "Millisecond",
                                      QString::number( time.msec() ) );
        }
    }

    void createModelIndexNode( QDomDocument& doc, QDomNode& parent,
                               const QString& elementName, const QModelIndex& idx )
    {
        QDomElement element = doc.createElement( elementName );
        parent.appendChild( element );
        setBoolAttribute( element, "Valid", idx.isValid() );
        if( idx.isValid() ){
            element.setAttribute( "Column", QString::number( idx.column() ) );
            element.setAttribute( "Row",    QString::number( idx.row()    ) );
        }
    }

    void createRealPairNode( QDomDocument& doc, QDomNode& parent,
                             const QString& elementName, const QPair<qreal, qreal> & pair )
    {
        QDomElement element = doc.createElement( elementName );
        parent.appendChild( element );
        element.setAttribute( "first",  QString::number( pair.first  ) );
        element.setAttribute( "second", QString::number( pair.second ) );
    }

    void createPositionBooleansNode(
        QDomDocument& doc, QDomNode& parent, const QString& elementName,
        bool unknown, bool center,
        bool northWest, bool north, bool northEast,
        bool east, bool southEast, bool south, bool southWest, bool west,
        bool floating )

    {
        QDomElement element = doc.createElement( elementName );
        parent.appendChild( element );
        setBoolAttribute( element, "Unknown",   unknown );
        setBoolAttribute( element, "Center",    center );
        setBoolAttribute( element, "NorthWest", northWest );
        setBoolAttribute( element, "North",     north );
        setBoolAttribute( element, "NorthEast", northEast );
        setBoolAttribute( element, "East",      east );
        setBoolAttribute( element, "SouthEast", southEast );
        setBoolAttribute( element, "South",     south );
        setBoolAttribute( element, "SouthWest", southWest );
        setBoolAttribute( element, "West",      west );
        setBoolAttribute( element, "Floating",  floating );
    }

    QString penStyleToString( Qt::PenStyle style )
    {
        switch( style ) {
        case Qt::NoPen:
            return "NoPen";
        case Qt::SolidLine:
            return "SolidLine";
        case Qt::DashLine:
            return "DashLine";
        case Qt::DotLine:
            return "DotLine";
        case Qt::DashDotLine:
            return "DashDotLine";
        case Qt::DashDotDotLine:
            return "DashDotDotLine";
        default: // should not happen
            return "SolidLine";
        }
    }

    QString penCapStyleToString( Qt::PenCapStyle style )
    {
        switch( style ) {
        case Qt::SquareCap:
            return "SquareCap";
        case Qt::FlatCap:
            return "FlatCap";
        case Qt::RoundCap:
            return "RoundCap";
        default: // should not happen
            return "SquareCap";
        }
    }

    QString penJoinStyleToString( Qt::PenJoinStyle style )
    {
        switch( style ) {
        case Qt::BevelJoin:
            return "BevelJoin";
        case Qt::MiterJoin:
            return "MiterJoin";
        case Qt::RoundJoin:
            return "RoundJoin";
        default: // should not happen
            return "BevelJoin";
        }
    }



    QString brushStyleToString( Qt::BrushStyle style )
    {
        // PENDING(kalle) Support custom patterns
        switch( style ) {
        case Qt::NoBrush:
            return "NoBrush";
        case Qt::SolidPattern:
            return "SolidPattern";
        case Qt::Dense1Pattern:
            return "Dense1Pattern";
        case Qt::Dense2Pattern:
            return "Dense2Pattern";
        case Qt::Dense3Pattern:
            return "Dense3Pattern";
        case Qt::Dense4Pattern:
            return "Dense4Pattern";
        case Qt::Dense5Pattern:
            return "Dense5Pattern";
        case Qt::Dense6Pattern:
            return "Dense6Pattern";
        case Qt::Dense7Pattern:
            return "Dense7Pattern";
        case Qt::HorPattern:
            return "HorPattern";
        case Qt::VerPattern:
            return "VerPattern";
        case Qt::CrossPattern:
            return "CrossPattern";
        case Qt::BDiagPattern:
            return "BDiagPattern";
        case Qt::FDiagPattern:
            return "FDiagPattern";
        case Qt::DiagCrossPattern:
            return "DiagCrossPattern";
        default: // should not happen (but can for a custom pattern)
            return "SolidPattern";
        }
    }


    bool findStringAttribute( const QDomElement& e, const QString & name, QString& attr )
    {
        const bool bOK = e.hasAttribute( name );
        if( bOK )
            attr = e.attribute( name );
        return bOK;
    }

    bool findIntAttribute( const QDomElement& e, const QString & name, int& attr )
    {
        bool bOK = false;
        if( e.hasAttribute( name ) ){
            int val = e.attribute( name ).toInt( &bOK );
            if( bOK )
                attr = val;
        }
        return bOK;
    }

    bool findDoubleAttribute( const QDomElement& e, const QString & name, double& attr )
    {
        bool bOK = false;
        if( e.hasAttribute( name ) ){
            double val = e.attribute( name ).toDouble( &bOK );
            if( bOK )
                attr = val;
        }
        return bOK;
    }

    bool findBoolAttribute( const QDomElement& e, const QString & name, bool& attr )
    {
        bool bOK = false;
        if( e.hasAttribute( name ) ){
            const QString value = e.attribute( name );
            bool foundFalse = false;
            const bool foundTrue =
                ( ! value.compare("true", Qt::CaseInsensitive) ) ||
                ( ! value.compare("1",    Qt::CaseInsensitive) ) ||
                ( ! value.compare("yes",  Qt::CaseInsensitive) );
            if( ! foundTrue ){
                foundFalse =
                    ( ! value.compare("false", Qt::CaseInsensitive) ) ||
                    ( ! value.compare("0",     Qt::CaseInsensitive) ) ||
                    ( ! value.compare("no",    Qt::CaseInsensitive) );
            }
            bOK = ( foundTrue || foundFalse );
            if( bOK )
                attr = foundTrue;
        }
        return bOK;
    }

    bool readStringNode( const QDomElement& element, QString& value )
    {
        value = element.text();
        return true;
    }

    bool readStringListNode( const QDomElement& element, QStringList& value )
    {
        for(QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling())
        {
            QDomText t = n.toText();
            if (!t.isNull())
                value.append( t.data() );
        }
        return true;
    }


    bool readIntNode( const QDomElement& element, int& value )
    {
        bool ok = false;
        int temp = element.text().toInt( &ok );
        if( ok )
            value = temp;
        return ok;
    }


    bool readDoubleNode( const QDomElement& element, double& value )
    {
        bool ok = false;
        double temp = element.text().toDouble( &ok );
        if( ok )
            value = temp;
        return ok;
    }


    bool readRealNode( const QDomElement& element, qreal& value )
    {
        bool ok = false;
        qreal temp = element.text().toDouble( &ok );
        if( ok )
            value = temp;
        return ok;
    }


    bool readBoolNode( const QDomElement& element, bool& value )
    {
        if( element.text() == "true" ) {
            value = true;
            return true;
        } else if( element.text() == "false" ) {
            value = false;
            return true;
        } else
            return false;
    }


    bool readOrientationNode( const QDomElement& element, Qt::Orientation& value )
    {
        if( element.text() == "vertical" ) {
            value = Qt::Vertical;
            return true;
        } else if( element.text() == "horizontal" ) {
            value = Qt::Horizontal;
            return true;
        } else
            return false;
    }


    bool readAlignmentNode(const QDomElement& element, Qt::Alignment& value )
    {
        //qDebug() << "read alignment tagname:" << element.tagName();
        bool bOK = true;
        Qt::Alignment align = 0;
        QString s;
        if( findStringAttribute( element, "Horizontal", s ) ){
            //qDebug() << "read alignment horizontal:" << s;
            if( s == "Left" )
                align = align | Qt::AlignLeft;
            else if( s == "Right" )
                align = align | Qt::AlignRight;
            else if( s == "Center" )
                align = align | Qt::AlignHCenter;
            else if( s == "Justify" )
                align = align | Qt::AlignJustify;
            else{
                qDebug()<< "Could not parse AlignmentNode. Error in attribute \"Horizontal\"";
                bOK = false;
            }
        }
        if( findStringAttribute( element, "Vertical", s ) ){
            //qDebug() << "read alignment vertical:" << s;
            if( s == "Top" )
                align = align | Qt::AlignTop;
            else if( s == "Bottom" )
                align = align | Qt::AlignBottom;
            else if( s == "Center" )
                align = align | Qt::AlignVCenter;
            else{
                qDebug()<< "Could not parse AlignmentNode. Error in attribute \"Vertical\"";
                bOK = false;
            }
        }
        if( bOK )
            value = align;
        //qDebug() << "read alignment:" << value;
        return bOK;
    }

    bool readBoxLayoutDirectionNode(const QDomElement& element, QBoxLayout::Direction& value )
    {
        bool bOK = true;
        QBoxLayout::Direction dir;
        QString s;
        if( findStringAttribute( element, "Direction", s ) ){
            bOK = true;
            if( s == "LeftToRight" )
                dir = QBoxLayout::LeftToRight;
            else if( s == "RightToLeft" )
                dir = QBoxLayout::RightToLeft;
            else if( s == "TopToBottom" )
                dir = QBoxLayout::TopToBottom;
            else if( s == "BottomToTop" )
                dir = QBoxLayout::BottomToTop;
            else
                bOK = false;
        }else{
            bOK = false;
            //qDebug() << "readBoxLayoutDirectionNode failed" ;
        }
        if( bOK )
            value = dir;
        return bOK;
    }


    bool readSizeNode( const QDomElement& element, QSize& value )
    {
        bool bOk = false;
        bool bFlag;
        if( findBoolAttribute( element, "Valid", bFlag ) && bFlag ){
            int width, height;
            if( findIntAttribute( element, "Width", width ) &&
                findIntAttribute( element, "Height", height ) )
            {
                bOk = true;
                value.setWidth(  width );
                value.setHeight( height );
            }
        }else{
            bOk = true;
            value = QSize(); // correctly return an invalid size
        }
        return bOk;
    }


    bool readSizeFNode( const QDomElement& element, QSizeF& value )
    {
        bool bOk = false;
        bool bFlag;
        if( findBoolAttribute( element, "Valid", bFlag ) && bFlag ){
            double width, height;
            if( findDoubleAttribute( element, "Width", width ) &&
                findDoubleAttribute( element, "Height", height ) )
            {
                bOk = true;
                value.setWidth(  width );
                value.setHeight( height );
            }
        }else{
            bOk = true;
            value = QSizeF(); // correctly return an invalid size
        }
        return bOk;
    }


    bool readColorNode( const QDomElement& element, QColor& value )
    {
        bool bOk = false;
        bool bFlag;
        if( findBoolAttribute( element, "Valid", bFlag ) && bFlag ){
            int red, green, blue;
            // these must be there:
            bOk =
                findIntAttribute( element, "Red",   red ) &&
                findIntAttribute( element, "Green", green ) &&
                findIntAttribute( element, "Blue",  blue );
            if( bOk ){
                int alpha=255;
                // this is optional:
                findIntAttribute( element, "Alpha", alpha );

                value = QColor( red, green, blue, alpha );
            }
        }else{
            bOk = true;
            value = QColor(); // correctly return an invalid color
        }
        return bOk;
    }

    bool readPointFNode( const QDomElement& element, QPointF& value )
    {
        bool ok = true;
        qreal x=0, y=0;
        if( element.hasAttribute( "x" ) ) {
            bool bOk = false;
            x = element.attribute( "y" ).toDouble( &bOk );
            ok = ok & bOk;
        }
        if( element.hasAttribute( "y" ) ) {
            bool bOk = false;
            y = element.attribute( "y" ).toDouble( &bOk );
            ok = ok & bOk;
        }

        if( ok )
            value = QPointF(x, y);

        return ok;
    }


    bool readBrushNode( const QDomElement& element, QBrush& brush )
    {
        bool ok = true;
        QColor tempColor;
        Qt::BrushStyle tempStyle=Qt::SolidPattern;
        QPixmap tempPixmap;
        QDomNode node = element.firstChild();
        while( !node.isNull() ) {
            QDomElement element = node.toElement();
            if( !element.isNull() ) { // was really an element
                QString tagName = element.tagName();
                if( tagName == "Color" ) {
                    ok = ok & readColorNode( element, tempColor );
                } else if( tagName == "Style" ) {
                    QString value;
                    ok = ok & readStringNode( element, value );
                    tempStyle = stringToBrushStyle( value );
                } else if( tagName == "Pixmap" ) {
                    ok = ok & readPixmapNode( element, tempPixmap );
                } else {
                    qDebug() << "Unknown tag in brush:" << tagName;
                }
            }
            node = node.nextSibling();
        }

        if( ok ) {
            brush.setColor( tempColor );
            brush.setStyle( tempStyle );
            if( !tempPixmap.isNull() )
                brush.setTexture( tempPixmap );
        }

        return ok;
    }


    bool readPixmapNode( const QDomElement& element, QPixmap& pixmap )
    {
        bool ok = true;
        unsigned long tempLength;
        QString tempData;
        QDomNode node = element.firstChild();
        while( !node.isNull() ) {
            QDomElement element = node.toElement();
            if( !element.isNull() ) { // was really an element
                QString tagName = element.tagName();
                if( tagName == "Format" ) {
                    QString formatName;
                    ok = ok & readStringNode( element, formatName );
#ifndef NDEBUG
                    if( formatName != "XPM.GZ" )
                        qDebug( "Unsupported pixmap format in XML file" );
#endif
                } else if( tagName == "Length" ) {
                    int itempLength;
                    ok = ok & readIntNode( element, itempLength );
                    tempLength = itempLength;
                } else if( tagName == "Data" ) {
                    ok = ok & readStringNode( element, tempData );
                } else {
                    qDebug( "Unknown tag in Pixmap" );
                }
            }
            node = node.nextSibling();
        }

        if( ok ) {
            if( 0 < tempLength ) {
                // Decode the image file format in the same way Qt Designer does.
                char *ba = new char[ tempData.length() / 2 ];
                for ( int i = 0; i < (int)tempData.length() / 2; ++i ) {
                    char h = tempData[ 2 * i ].toLatin1();
                    char l = tempData[ 2 * i  + 1 ].toLatin1();
                    uchar r = 0;
                    if ( h <= '9' )
                        r += h - '0';
                    else
                        r += h - 'a' + 10;
                    r = r << 4;
                    if ( l <= '9' )
                        r += l - '0';
                    else
                        r += l - 'a' + 10;
                    ba[ i ] = r;
                }

                if( tempData.length() * 5 > static_cast<long>(tempLength) )
                    tempLength = tempData.length() * 5;
                QByteArray baunzip;
#ifdef DO_NOT_COMPRESS_PIXMAP_DATA
                baunzip = ba;
#else
                baunzip.reserve( tempLength );
                ::uncompress( (uchar*) baunzip.data(), &tempLength,
                              (uchar*) ba, tempData.length()/2 );
#endif
                QImage image;
                image.loadFromData( (const uchar*)baunzip.data(), tempLength, "XPM" );

                if( image.isNull() )
                    pixmap = QPixmap(); // This is _not_ an error, we just read a NULL pixmap!
                else
                    pixmap = QPixmap::fromImage( image, 0 );
            } else
                pixmap = QPixmap(); // This is _not_ an error, we just read a empty pixmap!
        }

        return ok;
    }


    bool readPenNode( const QDomElement& element, QPen& pen )
    {
        bool ok = true;
        int tempWidth = 0;
        QColor tempColor;
        QBrush tempBrush;
        Qt::PenStyle     tempStyle=Qt::SolidLine;
        Qt::PenCapStyle  tempCapStyle=Qt::SquareCap;
        Qt::PenJoinStyle tempJoinStyle=Qt::BevelJoin;
        QDomNode node = element.firstChild();
        while( !node.isNull() ) {
            QDomElement element = node.toElement();
            if( !element.isNull() ) { // was really an element
                QString tagName = element.tagName();
                if( tagName == "Width" ) {
                    ok = ok & readIntNode( element, tempWidth );
                } else if( tagName == "Color" ) {
                    ok = ok & readColorNode( element, tempColor );
                } else if( tagName == "Brush" ) {
                    ok = ok & readBrushNode( element, tempBrush );
                } else if( tagName == "Style" ) {
                    QString value;
                    ok = ok & readStringNode( element, value );
                    tempStyle = stringToPenStyle( value );
                } else if( tagName == "CapStyle" ) {
                    QString value;
                    ok = ok & readStringNode( element, value );
                    tempCapStyle = stringToPenCapStyle( value );
                } else if( tagName == "JoinStyle" ) {
                    QString value;
                    ok = ok & readStringNode( element, value );
                    tempJoinStyle = stringToPenJoinStyle( value );
                } else {
                    qDebug() << "Unknown tag in pen:" << tagName;
                }
            }
            node = node.nextSibling();
        }

        if( ok ) {
            pen.setWidth( tempWidth );
            pen.setBrush( tempBrush );
            pen.setColor( tempColor );
            pen.setStyle(     tempStyle );
            pen.setCapStyle(  tempCapStyle );
            pen.setJoinStyle( tempJoinStyle );
        }

        return ok;
    }

    bool readFontNode( const QDomElement& element, QFont& font )
    {
        bool ok = true;
        QString family;
        qreal pointSize;
        int weight;
        bool italic;
        int charSet;
        QDomNode node = element.firstChild();
        while( !node.isNull() ) {
            QDomElement element = node.toElement();
            if( !element.isNull() ) { // was really an element
                QString tagName = element.tagName();
                if( tagName == "Family" ) {
                    ok = ok & readStringNode( element, family );
                } else if( tagName == "PointSize" ) {
                    ok = ok & readRealNode( element, pointSize );
                } else if( tagName == "Weight" ) {
                    ok = ok & readIntNode( element, weight );
                } else if( tagName == "Italic" ) {
                    ok = ok & readBoolNode( element, italic );
                } else if( tagName == "CharSet" ) {
                    ok = ok & readIntNode( element, charSet );
                } else {
                    qDebug( "Unknown tag in color map" );
                }
            }
            node = node.nextSibling();
        }

        if( ok ) {
            font.setFamily( family );
            font.setPointSizeF( pointSize );
            font.setWeight( weight );
            font.setItalic( italic );
        }

        return ok;
    }

    bool readRectNode( const QDomElement& element, QRect& value )
    {
        bool ok = true;
        int width, height, x, y;
        QDomNode node = element.firstChild();
        while( !node.isNull() ) {
            QDomElement element = node.toElement();
            if( !element.isNull() ) { // was really an element
                QString tagName = element.tagName();
                if( tagName == "Width" ) {
                    ok = ok & readIntNode( element, width );
                } else if( tagName == "Height" ) {
                    ok = ok & readIntNode( element, height );
                } else if( tagName == "X" ) {
                    ok = ok & readIntNode( element, x );
                } else if( tagName == "Y" ) {
                    ok = ok & readIntNode( element, y );
                } else {
                    qDebug( "Unknown tag in rect" );
                }
            }
            node = node.nextSibling();
        }

        if( ok ) {
            value.setX( x );
            value.setY( y );
            value.setWidth( width );
            value.setHeight( height );
        }

        return ok;
    }



    bool readDateTimeNode( const QDomElement& element, QDateTime& datetime )
    {
        bool ok = true;
        bool bFlag;
        if( findBoolAttribute( element, "Valid", bFlag ) && bFlag ){
            QDate tempDate;
            QTime tempTime;
            QDomNode node = element.firstChild();
            while( !node.isNull() ) {
                QDomElement element = node.toElement();
                if( !element.isNull() ) { // was really an element
                    QString tagName = element.tagName();
                    if( tagName == "Date" ) {
                        ok = ok & readDateNode( element, tempDate );
                    } else if( tagName == "Time" ) {
                        ok = ok & readTimeNode( element, tempTime );
                    } else {
                        qDebug( "Unknown tag in datetime" );
                    }
                }
                node = node.nextSibling();
            }

            if( ok ) {
                datetime.setDate( tempDate );
                datetime.setTime( tempTime );
            }
        }else{
            ok = true;
            datetime = QDateTime(); // correctly return an invalid datetime
        }

        return ok;
    }


    bool readDateNode( const QDomElement& element, QDate& value )
    {
        bool ok = true;
        bool bFlag;
        if( findBoolAttribute( element, "Valid", bFlag ) && bFlag ){
            int year=0, month=0, day=0;
            if( element.hasAttribute( "Year" ) ) {
                bool yearOk = false;
                year = element.attribute( "Year" ).toInt( &yearOk );
                ok = ok & yearOk;
            }
            if( element.hasAttribute( "Month" ) ) {
                bool monthOk = false;
                month = element.attribute( "Month" ).toInt( &monthOk );
                ok = ok & monthOk;
            }
            if( element.hasAttribute( "Day" ) ) {
                bool dayOk = false;
                day = element.attribute( "Day" ).toInt( &dayOk );
                ok = ok & dayOk;
            }
            if( ok )
                value.setYMD( year, month, day );
        }else{
            ok = true;
            value = QDate(); // correctly return an invalid date
        }

        return ok;
    }



    bool readTimeNode( const QDomElement& element, QTime& value )
    {
        bool ok = true;
        bool bFlag;
        if( findBoolAttribute( element, "Valid", bFlag ) && bFlag ){
            int hour=0, minute=0, second=0, msec=0;
            if( element.hasAttribute( "Hour" ) ) {
                bool hourOk = false;
                hour = element.attribute( "Hour" ).toInt( &hourOk );
                ok = ok & hourOk;
            }
            if( element.hasAttribute( "Minute" ) ) {
                bool minuteOk = false;
                minute = element.attribute( "Minute" ).toInt( &minuteOk );
                ok = ok & minuteOk;
            }
            if( element.hasAttribute( "Second" ) ) {
                bool secondOk = false;
                second = element.attribute( "Second" ).toInt( &secondOk );
                ok = ok & secondOk;
            }
            if( element.hasAttribute( "Millisecond" ) ) {
                bool msecOk = false;
                msec = element.attribute( "Millisecond" ).toInt( &msecOk );
                ok = ok & msecOk;
            }
            if( ok )
                value.setHMS( hour, minute, second, msec );
        }else{
            ok = true;
            value = QTime(); // correctly return an invalid time
        }

        return ok;
    }

    bool readRealPairNode( const QDomElement& element, QPair<qreal, qreal> & pair )
    {
        qreal first, second;
        const bool bOK =
            findDoubleAttribute( element, "first", first ) &&
            findDoubleAttribute( element, "second", second );
        if( bOK ){
            pair.first = first;
            pair.second = second;
        }else{
            pair.first = 0.0;
            pair.second = 0.0;
        }
        return bOK;
    }

    bool readPositionBooleansNode(
        const QDomElement& element,
        bool& unknown, bool& center,
        bool& northWest, bool& north, bool& northEast,
        bool& east, bool& southEast, bool& south, bool& southWest, bool& west,
        bool& floating )
    {
        // at least one of the attributes needs to be set (no matter if true or false)
        // and any possible combination of settings is allowed
        return  findBoolAttribute( element, "Unknown", unknown ) ||
            findBoolAttribute( element, "Center",  center ) ||
            findBoolAttribute( element, "NorthWest", northWest ) ||
            findBoolAttribute( element, "North",     north ) ||
            findBoolAttribute( element, "NorthEast", northEast ) ||
            findBoolAttribute( element, "East",      east ) ||
            findBoolAttribute( element, "SouthEast", southEast ) ||
            findBoolAttribute( element, "South",     south ) ||
            findBoolAttribute( element, "SouthWest", southWest ) ||
            findBoolAttribute( element, "West",      west ) ||
            findBoolAttribute( element, "Floating",  floating );
    }

    bool readModelIndexNode(const QDomElement& element,
                            const QAbstractItemModel& model,
                            QModelIndex& idx )
    {
        bool ok = false;
        bool bFlag;
        if( findBoolAttribute( element, "Valid", bFlag ) && bFlag ){
            int column, row;
            if( findIntAttribute( element, "Column", column ) &&
                findIntAttribute( element, "Row", row ) )
            {
                ok = true;
                idx = model.index( row, column );
            }
        }else{
            ok = true;
            idx = QModelIndex(); // correctly return an invalid index
        }

        return ok;
    }

    bool readModelIndexNode(const QDomElement& element,
                            bool& isValid,
                            int& column,
                            int& row )
    {
        bool ok = findBoolAttribute( element, "Valid", isValid );
        if( ok && isValid ){
            int c, r;
            if( findIntAttribute( element, "Column", c ) &&
                findIntAttribute( element, "Row", r ) )
            {
                column = c;
                row = r;
            }else{
                ok = false;
            }
        }
        return ok;
    }

    Qt::PenStyle stringToPenStyle( const QString& style )
    {
        if( style == "NoPen" )
            return Qt::NoPen;
        else if( style == "SolidLine" )
            return Qt::SolidLine;
        else if( style == "DashLine" )
            return Qt::DashLine;
        else if( style == "DotLine" )
            return Qt::DotLine;
        else if( style == "DashDotLine" )
            return Qt::DashDotLine;
        else if( style == "DashDotDotLine" )
            return Qt::DashDotDotLine;
        else // should not happen
            return Qt::SolidLine;
    }

    Qt::PenCapStyle stringToPenCapStyle( const QString& style )
    {
        if( style == "SquareCap" )
            return Qt::SquareCap;
        else if( style == "FlatCap" )
            return Qt::FlatCap;
        else if( style == "RoundCap" )
            return Qt::RoundCap;
        else // should not happen
            return Qt::SquareCap;
    }

    Qt::PenJoinStyle stringToPenJoinStyle( const QString& style )
    {
        if( style == "BevelJoin" )
            return Qt::BevelJoin;
        else if( style == "MiterJoin" )
            return Qt::MiterJoin;
        else if( style == "RoundJoin" )
            return Qt::RoundJoin;
        else // should not happen
            return Qt::BevelJoin;
    }


    Qt::BrushStyle stringToBrushStyle( const QString& style )
    {
        // PENDING(kalle) Support custom patterns
        if( style == "NoBrush" )
            return Qt::NoBrush;
        else if( style == "SolidPattern" )
            return Qt::SolidPattern;
        else if( style == "Dense1Pattern" )
            return Qt::Dense1Pattern;
        else if( style == "Dense2Pattern" )
            return Qt::Dense2Pattern;
        else if( style == "Dense3Pattern" )
            return Qt::Dense3Pattern;
        else if( style == "Dense4Pattern" )
            return Qt::Dense4Pattern;
        else if( style == "Dense5Pattern" )
            return Qt::Dense5Pattern;
        else if( style == "Dense6Pattern" )
            return Qt::Dense6Pattern;
        else if( style == "Dense7Pattern" )
            return Qt::Dense7Pattern;
        else if( style == "HorPattern" )
            return Qt::HorPattern;
        else if( style == "VerPattern" )
            return Qt::VerPattern;
        else if( style == "CrossPattern" )
            return Qt::CrossPattern;
        else if( style == "BDiagPattern" )
            return Qt::BDiagPattern;
        else if( style == "FDiagPattern" )
            return Qt::FDiagPattern;
        else if( style == "DiagCrossPattern" )
            return Qt::DiagCrossPattern;
        else // should not happen (but can with custom patterns)
            return Qt::SolidPattern;
    }

    // FIXME readQVariantNode and createQVariantNode use a lot of
    // static strings, this is mostly because the strings are QStrings
    static const char* QRectWidth = "width";
    static const char* QRectHeight = "height";
    static const char* QRectX = "x";
    static const char* QRectY = "y";
    static const char* QSizeWidth = QRectWidth;
    static const char* QSizeHeight = QRectHeight;
    static const char* QPointX = QRectX;
    static const char* QPointY = QRectY;
    static const char* ValueAttributeName = "value";

    bool readQVariantNode( const QDomElement& element, QVariant& v, QString& name )
    {
        QString typeString = element.attribute( "type" );
        bool ok = true;
        int type = typeString.toInt( &ok );
        QString text = element.attribute( ValueAttributeName );

        if ( ! ok ) {
            qDebug() << "KDXML::readQVariantNode: error reading node";
        } else {
            name = element.attribute( "name" );
            switch( type ) {
            case QVariant::Bool: {
                qVariantSetValue< bool >( v, text == QString::fromLatin1( "true" ) );
            } break;
            case QVariant::String: {
                qVariantSetValue< QString >( v, text );
            } break;
            case QVariant::Rect: {
                int x = element.attribute( QRectX, "0" ).toInt();
                int y = element.attribute( QRectY, "0" ).toInt();
                int w = element.attribute( QRectWidth, "-1" ).toInt();
                int h = element.attribute( QRectHeight, "-1" ).toInt();
                const QRect rect( x, y, w, h );
                qVariantSetValue< QRect >( v, rect );
            } break;
            case QVariant::Int: {
                bool ok;
                const int number = text.toInt( &ok );
                if ( ok ) {
                    qVariantSetValue< int >( v, number );
                } else {
                    ok = false;
                }
            } break;
            case QVariant::Point: {
                const int x = element.attribute( QPointX, "0" ).toInt();
                const int y = element.attribute( QPointY, "0" ).toInt();
                const QPoint point( x, y );
                qVariantSetValue< QPoint >( v, point );
            } break;
            case QVariant::Size: {
                const int w = element.attribute( QSizeWidth, "0" ).toInt();
                const int h = element.attribute( QSizeHeight, "0" ).toInt();
                const QSize size( w, h );
                qVariantSetValue< QSize >( v, size );
            } break;
            default:
                qDebug() << "KDXML::readQVariantNode: property"
                         << name << "of unknown type" << type << "found";
                ok = false;
            }
        }
        return ok;
    }

    void createQVariantNode( QDomDocument& doc, QDomNode& parent, const QString& name, const QVariant& value )
    {
        QDomElement property = doc.createElement( "qtproperty" );
        property.setAttribute( "type", value.type() );
        property.setAttribute( "name", name );
        switch( value.type() ) {
        case QVariant::Bool: {
            if ( qVariantValue< bool >( value ) == true ) {
                property.setAttribute( ValueAttributeName, QString::fromLatin1( "true" ) );
            } else {
                property.setAttribute( ValueAttributeName, QString::fromLatin1( "false" ) );
            }
        } break;
        case QVariant::String: {
            property.setAttribute( ValueAttributeName, qVariantValue< QString >( value ) );
        } break;
        case QVariant::Rect: {
            const QRect rect( qVariantValue< QRect >( value ) );
            property.setAttribute( QRectX, rect.x() );
            property.setAttribute( QRectY, rect.y() );
            property.setAttribute( QRectWidth, rect.width() );
            property.setAttribute( QRectHeight, rect.height() );
        } break;
        case QVariant::Int: {
            property.setAttribute( ValueAttributeName, qVariantValue< int >( value ) );
        } break;
        case QVariant::Point: {
            const QPoint point( qVariantValue< QPoint >( value ) );
            property.setAttribute( QPointX, point.x() );
            property.setAttribute( QPointY, point.y() );
        } break;
        case QVariant::Size: {
            const QSize size( qVariantValue< QSize >( value ) );
            property.setAttribute( QSizeWidth, size.width() );
            property.setAttribute( QSizeHeight, size.height() );
        } break;
        default:
            qDebug() << "createQVariantNode: cannot serialize QVariant subtype" << value.type()
                     << ", want me to abort? Nah :-)";

        }
        parent.appendChild( property );
    }
}
