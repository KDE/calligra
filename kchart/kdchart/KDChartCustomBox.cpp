/* -*- Mode: C++ -*-
   $Id$
   KDChart - a multi-platform charting engine
*/

/****************************************************************************
** Copyright (C) 2001-2002 Klarälvdalens Datakonsult AB.  All rights reserved.
**
** This file is part of the KDChart library.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid commercial KDChart licenses may use this file in
** accordance with the KDChart Commercial License Agreement provided with
** the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.klaralvdalens-datakonsult.se/Public/products/ for
**   information about KDChart Commercial License Agreements.
**
** Contact info@klaralvdalens-datakonsult.se if any conditions of this
** licensing are not clear to you.
**
**********************************************************************/
#include <KDChartCustomBox.h>
#include <KDXMLTools.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qregion.h>
#include <qpalette.h>
#include <qpoint.h>


KDChartCustomBox::~KDChartCustomBox()
{
    // Intentionally left blank for now.
}


QRect KDChartCustomBox::trueRect( QPoint anchor, double averageWidthP1000, double averageHeightP1000 ) const
{
    int x = (0 > _deltaX) ? static_cast < int > ( -averageWidthP1000  * _deltaX ) : _deltaX;
    int y = (0 > _deltaY) ? static_cast < int > ( -averageHeightP1000 * _deltaY ) : _deltaY;
    int w = (0 > _width ) ? static_cast < int > ( -averageWidthP1000  * _width  ) : _width ;
    int h = (0 > _height) ? static_cast < int > ( -averageHeightP1000 * _height ) : _height;

    if (      Qt::AlignLeft  == (Qt::AlignLeft  & _anchorAlign) )
        x =  anchor.x() + x;
    else if ( Qt::AlignRight == (Qt::AlignRight & _anchorAlign) )
        x = (anchor.x() - x) - w + 1;
    else
        x =  anchor.x() - w / 2;

    if (      Qt::AlignTop    == (Qt::AlignTop    & _anchorAlign) )
        y =  anchor.y() + y;
    else if ( Qt::AlignBottom == (Qt::AlignBottom & _anchorAlign) )
        y = (anchor.y() - y) - h + 1;
    else
        y =  anchor.y() - h / 2;

    return QRect( x,y, w, h );
}


void KDChartCustomBox::paint( QPainter* painter,
                              QPoint anchor,
                              double areaWidthP1000,
                              double areaHeightP1000,
                              const QColor * color,
                              const QBrush * paper ) const
{
    painter->save();
    QRect myRect( trueRect( anchor, areaWidthP1000, areaHeightP1000 ) );
    if ( myRect.isValid() ) {
        if ( _fontSize ) {
            QFont font( content().font() );
            float size;
            if( 0 > _fontSize ) {
                if( _fontScaleGlobal )
                    size = _fontSize * (areaWidthP1000 + areaHeightP1000);
                else
                    size = _fontSize * (myRect.width() + myRect.height());
                size /= -2.0;
            }
            else
                size = _fontSize;
            font.setPointSizeFloat( size );
            KDChartTextPiece tmpTextPiece( content().text(), font );
            tmpTextPiece.draw( painter, myRect.x(), myRect.y(), QRegion( myRect ),
                               color ? *color :  _color,
                               paper ?  paper : &_paper );

        } else
            content().draw( painter, myRect.x(), myRect.y(), QRegion( myRect ),
                            color ? *color :  _color,
                            paper ?  paper : &_paper );
    }
    painter->restore();
}


void KDChartCustomBox::createCustomBoxNode( QDomDocument& document,
					    QDomNode& parent,
					    const QString& elementName,
					    const KDChartCustomBox* custombox )
{
    QDomElement customBoxElement = document.createElement( elementName );
    parent.appendChild( customBoxElement );
    KDXML::createStringNode( document, parent, "ContentText",
			     custombox->_content.text() );
    KDXML::createFontNode( document, parent, "ContentFont",
			   custombox->_content.font() );
    KDXML::createIntNode( document, parent, "FontSize", custombox->_fontSize );
    KDXML::createBoolNode( document, parent, "FontScaleGlobal",
			   custombox->_fontScaleGlobal );
    KDXML::createIntNode( document, parent, "DeltaX", custombox->_deltaX );
    KDXML::createIntNode( document, parent, "DeltaY", custombox->_deltaY );
    KDXML::createIntNode( document, parent, "Width", custombox->_width );
    KDXML::createIntNode( document, parent, "Height", custombox->_height );
    KDXML::createColorNode( document, parent, "Color", custombox->_color );
    KDXML::createBrushNode( document, parent, "Paper", custombox->_paper );
    KDXML::createIntNode( document, parent, "AnchorArea",
			  custombox->_anchorArea );
    KDXML::createStringNode( document, parent, "AnchorPos",
			     KDChartEnums::positionFlagToString( custombox->_anchorPos ) );
    KDXML::createIntNode( document, parent, "AnchorAlign",
			  custombox->_anchorAlign );
    KDXML::createIntNode( document, parent, "DataRow",
			  custombox->_dataRow );
    KDXML::createIntNode( document, parent, "DataCol",
			  custombox->_dataCol );
    KDXML::createIntNode( document, parent, "Data3rd",
			  custombox->_data3rd );
}


bool KDChartCustomBox::readCustomBoxNode( const QDomElement& element,
                                          KDChartCustomBox& custombox )
{
    bool ok = true;
    QString tempContentText;
    QFont tempContentFont;
    int tempFontSize, tempDeltaX, tempDeltaY, tempWidth, tempHeight,
	tempAnchorArea, tempAnchorAlign, tempDataRow, tempDataCol, 
        tempData3rd;
    bool tempFontScaleGlobal;
    QColor tempColor;
    QBrush tempPaper;
    KDChartEnums::PositionFlag tempAnchorPos;
    QDomNode node = element.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "ContentText" ) {
                ok = ok & KDXML::readStringNode( element, tempContentText );
            } else if( tagName == "ContentFont" ) {
                ok = ok & KDXML::readFontNode( element, tempContentFont );
            } else if( tagName == "FontSize" ) {
                ok = ok & KDXML::readIntNode( element, tempFontSize );
            } else if( tagName == "FontScaleGlobal" ) {
                ok = ok & KDXML::readBoolNode( element, tempFontScaleGlobal );
            } else if( tagName == "DeltaX" ) {
                ok = ok & KDXML::readIntNode( element, tempDeltaX );
            } else if( tagName == "DeltaY" ) {
                ok = ok & KDXML::readIntNode( element, tempDeltaY );
            } else if( tagName == "Width" ) {
                ok = ok & KDXML::readIntNode( element, tempWidth );
            } else if( tagName == "Height" ) {
                ok = ok & KDXML::readIntNode( element, tempHeight );
            } else if( tagName == "Color" ) {
                ok = ok & KDXML::readColorNode( element, tempColor );
            } else if( tagName == "Paper" ) {
                ok = ok & KDXML::readBrushNode( element, tempPaper );
            } else if( tagName == "AnchorArea" ) {
                ok = ok & KDXML::readIntNode( element, tempAnchorArea );
            } else if( tagName == "AnchorPos" ) {
		QString value;
                ok = ok & KDXML::readStringNode( element, value );
		tempAnchorPos = KDChartEnums::stringToPositionFlag( value );
            } else if( tagName == "AnchorAlign" ) {
                ok = ok & KDXML::readIntNode( element, tempAnchorAlign );
            } else if( tagName == "DataRow" ) {
                ok = ok & KDXML::readIntNode( element, tempDataRow );
            } else if( tagName == "DataCol" ) {
                ok = ok & KDXML::readIntNode( element, tempDataCol );
            } else if( tagName == "Data3rd" ) {
                ok = ok & KDXML::readIntNode( element, tempData3rd );
            } else {
                qDebug( "Unknown tag in custom box" );
            }
        }
        node = node.nextSibling();
    }

    if( ok ) {
	custombox._content = KDChartTextPiece( tempContentText,
					       tempContentFont );
	custombox._fontSize = tempFontSize;
	custombox._fontScaleGlobal = tempFontScaleGlobal;
	custombox._deltaX = tempDeltaX;
	custombox._deltaY = tempDeltaY;
	custombox._width = tempWidth;
	custombox._height = tempHeight;
	custombox._color = tempColor;
	custombox._paper = tempPaper;
	custombox._anchorArea = tempAnchorArea;
	custombox._anchorPos = tempAnchorPos;
	custombox._anchorAlign = tempAnchorAlign;
	custombox._dataRow = tempDataRow;
	custombox._dataCol = tempDataCol;
	custombox._data3rd = tempData3rd;
    }

    return ok;
}
