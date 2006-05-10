/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klarälvdalens Datakonsult AB.  All rights reserved.
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
 ** See http://www.klaralvdalens-datakonsult.se/?page=products for
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@klaralvdalens-datakonsult.se if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#include <qpainter.h>
#include <qregion.h>
#include <qpalette.h>
#include <QPoint>
#include <q3simplerichtext.h>
#include <q3paintdevicemetrics.h>

#include <KDChartCustomBox.h>
#include <KDXMLTools.h>
#include <KDFrame.h>



KDChartCustomBox::~KDChartCustomBox()
{
    // Intentionally left blank for now.
}


void KDChartCustomBox::deepCopy( const KDChartCustomBox* source )
{
    if( !source || this == source )
        return;
    _rotation              = source->_rotation;
    _content.deepCopy( &source->_content);
    _fontSize              = source->_fontSize;
    _fontScaleGlobal       = source->_fontScaleGlobal;
    _deltaX                = source->_deltaX;
    _deltaY                = source->_deltaY;
    _width                 = source->_width;
    _height                = source->_height;
    _color                 = source->_color;
    _paper                 = source->_paper;
    _anchorArea            = source->_anchorArea;
    _anchorPos             = source->_anchorPos;
    _anchorAlign           = source->_anchorAlign;
    _dataRow               = source->_dataRow;
    _dataCol               = source->_dataCol;
    _data3rd               = source->_data3rd;
    _deltaAlign            = source->_deltaAlign;
    _deltaScaleGlobal      = source->_deltaScaleGlobal;
    _anchorBeingCalculated = source->_anchorBeingCalculated;
}

const KDChartCustomBox* KDChartCustomBox::clone() const
{
    KDChartCustomBox* newBox = new KDChartCustomBox();
    newBox->deepCopy( this );
    return newBox;
}


float KDChartCustomBox::trueFontSize( double areaWidthP1000,
                                      double areaHeightP1000,
                                      int rectHeight ) const
{
    float size;
    if( 0 > _fontSize ) {
      if( _fontScaleGlobal ) {

        size = _fontSize * qMin(areaWidthP1000, areaHeightP1000) * -1.0;//(areaWidthP1000 + areaHeightP1000) / -2.0;

      }  else {
             
            // calculate the exact size:
	float targetLineSpacing = (_fontSize * rectHeight)/ -1000;
            size = targetLineSpacing;
            // step #1  -  try to make the right font size:
            QFont font( content().font() );
            font.setPointSizeFloat( size );
            QFontMetrics fm( font );
	    //qDebug(QString("\nsize                   : ")+QString::number(size));
	    //qDebug(QString("(float)rectHeight      : ")+QString::number((float)rectHeight));
	    //qDebug(QString("(float)fm.lineSpacing(): ")+QString::number((float)fm.lineSpacing()));
            size *= targetLineSpacing / fm.lineSpacing();
	    //qDebug(QString("size                   : ")+QString::number(size));
            // step #2  -  make sure the font size is not too big:
            font.setPointSizeFloat( size );
            QFontMetrics fm2( font );
            size *= targetLineSpacing / fm2.lineSpacing();
	    //qDebug(QString("(float)rectHeight      : ")+QString::number((float)rectHeight));
	    //qDebug(QString("(float)fm.lineSpacing(): ")+QString::number((float)fm.lineSpacing()));
	    //qDebug(QString("size                   : ")+QString::number(size));
        }
    }
    else {
//qDebug(QString("\n_fontsize: ")+QString::number(_fontSize));
        if( _fontSize )
            size = _fontSize;
        else{
            size = content().font().pointSize();
            if( -1 == size )
                size = content().font().pixelSize();
            if( -1 == size )
                size = 10;
        }
    }
    return size;
}


int KDChartCustomBox::trueFontLineSpacing( double areaWidthP1000,
                                           double areaHeightP1000,
                                           int rectHeight ) const
{
    QFont font( content().font() );
    font.setPointSizeFloat( trueFontSize( areaWidthP1000,areaHeightP1000, rectHeight ) );
    QFontMetrics fm( font );
    return fm.lineSpacing();
}


void KDChartCustomBox::getTrueShift( double areaWidthP1000,
                                     double areaHeightP1000,
                                     int rectHeight,
                                     int& dX,
                                     int& dY ) const
{
    int x, y;
    if( _deltaScaleGlobal ){
        x = (0 > _deltaX) ? static_cast < int > ( -areaWidthP1000  * _deltaX ) : _deltaX;
        y = (0 > _deltaY) ? static_cast < int > ( -areaHeightP1000 * _deltaY ) : _deltaY;
    }else{
        int fontHeight = trueFontLineSpacing( areaWidthP1000, areaHeightP1000, rectHeight );
        //qDebug("\nfontHeight %i",fontHeight);
        x = (0 > _deltaX) ? static_cast < int > ( fontHeight * _deltaX / -100.0 ) : _deltaX;
        y = (0 > _deltaY) ? static_cast < int > ( fontHeight * _deltaY / -100.0 ) : _deltaY;
        //qDebug("y %i",y);
    }
    uint deltaAlign = (KDCHART_AlignAuto == _deltaAlign) ? _anchorAlign : _deltaAlign;
    if (      Qt::AlignLeft  == (Qt::AlignLeft  & deltaAlign) )
        dX = x;
    else if ( Qt::AlignRight == (Qt::AlignRight & deltaAlign) )
        dX = -x;
    else
        dX = 0; //  <--  so the _deltaX value becomes ineffective!

    if (      Qt::AlignTop    == (Qt::AlignTop    & deltaAlign) )
        dY = y;
    else if ( Qt::AlignBottom == (Qt::AlignBottom & deltaAlign) )
        dY = -y;
    else
        dY = 0; //  <--  so the _deltaY value becomes ineffective!
}

//static QPainter* pppainter=0;
//static int pdWidth =1280;
//static int pdHeight =1024;

QRect KDChartCustomBox::trueRect( QPoint anchor, double areaWidthP1000, double areaHeightP1000 ) const
{
   int w = (0 > _width ) ? static_cast < int > ( -areaWidthP1000  * _width  ) : _width;
   int h = (0 > _height) ? static_cast < int > ( -areaHeightP1000 * _height ) : _height;

   //qDebug("w: %i    h: %i", w,h );
  
    if( _fontScaleGlobal && 0 == w && 0 == h ){
      //Now calculate the size of the box based upon the content!
        QFont font( content().font() );
        if ( _fontSize ) {
            font.setPointSizeFloat(
                (0 > _fontSize)
            ? (_fontSize * qMin(areaWidthP1000, areaHeightP1000) * -1.0)//(areaWidthP1000 + areaHeightP1000) / -2.0
            : _fontSize );
	    //qDebug("\n_fontSize * qMin(areaWidthP1000, areaHeightP1000)    %i * qMin(%f, %f) text: %s", _fontSize , areaWidthP1000, areaHeightP1000, content().text().latin1());
        }
        QString txt(     content().text() );
        QString txtTest( txt.trimmed().lower() );
#if QT_VERSION >= 200 && QT_VERSION < 300
// version 2.x
        if( !(txtTest.left(4) == "<qt>") )
            txt.prepend(        "<qt>" );
        if( !(txtTest.right(5)== "</qt>"))
            txt.append(         "</qt>");
#else
        if( !txtTest.startsWith("<qt>" ) )
            txt.prepend(        "<qt>" );
        if( !txtTest.endsWith(  "</qt>") )
            txt.append(         "</qt>");
#endif

        //qDebug("\nw: %i    h: %i", w,h );
        Q3SimpleRichText tmpContent( txt, font );
//        tmpContent.setWidth(pdWidth);
//        tmpContent.setHeight(pdHeight);
//        tmpContent.adjustSize();
//        if( pppainter ){
//            tmpContent.setWidth(pppainter, 2000);
//            tmpContent.adjustSize();
//        }
        w = tmpContent.widthUsed();
        h = tmpContent.height();
	//qDebug("pdWidth: %i               w: %i  h %i",pdWidth,w,h);
        //qDebug("w: %i    h: %i", w,h );
    }

    //qDebug("\nw: %i    h: %i", w,h );
    int x,y;
    if (      Qt::AlignLeft  == (Qt::AlignLeft  & _anchorAlign) )
        x = 0;
    else if ( Qt::AlignRight == (Qt::AlignRight & _anchorAlign) )
        x = - w + 1;
    else
        x = - w / 2;

    if (      Qt::AlignTop    == (Qt::AlignTop    & _anchorAlign) )
        y = 0;
    else if ( Qt::AlignBottom == (Qt::AlignBottom & _anchorAlign) )
        y = - h + 1;
    else
        y = - h / 2;
    int dX,dY;
    getTrueShift( areaWidthP1000, areaHeightP1000, h,
                  dX, dY );
    //qDebug("trueRect:  x %i  y %i    w %i  h %i text: %s", anchor.x()+x+dX, anchor.y()+y+dY, w,h, content().text().latin1());
    
    return QRect( anchor.x()+x+dX, anchor.y()+y+dY, w, h );
}


int KDChartCustomBox::trueRectAlignX(const QRect& rect) const
{
    int ret = rect.center().x();
    if (      Qt::AlignLeft  == (Qt::AlignLeft  & _anchorAlign) )
        ret -= rect.width();
    else if ( Qt::AlignRight == (Qt::AlignRight & _anchorAlign) )
        ret += rect.width();
    return ret;
}
int KDChartCustomBox::trueRectAlignY(const QRect& rect) const
{
    int ret = rect.center().y();
    if (      Qt::AlignTop    == (Qt::AlignTop    & _anchorAlign) )
        ret -= rect.height();
    else if ( Qt::AlignBottom == (Qt::AlignBottom & _anchorAlign) )
        ret += rect.height();
    return ret;
}

void KDChartCustomBox::paint( QPainter* painter,
                              QPoint anchor,
                              double areaWidthP1000,
                              double areaHeightP1000,
                              const KDFrame* frame,
                              const QRect&   frameRect,
                              const QColor * color,
                              const QBrush * paper ) const
{
    painter->save();
    int rotDX = 0;
    int rotDY = 0;
/*
//pppainter=painter;
const QPaintDeviceMetrics metrics(painter->device());
pdWidth = metrics.width();
const int aWidthP1000  = metrics.width() /1000;
const int aHeightP1000 = metrics.height()/1000;
//pdHeight = metrics.height();
*/

    QRect myRect( trueRect( anchor, areaWidthP1000, areaHeightP1000 ) );
    
    
    /*
QPaintDeviceMetrics metrics(painter->device());
int pdWidth = metrics.width();
int dpiY = metrics.logicalDpiY();
int dpiX = metrics.logicalDpiX();
qDebug("dpiY: %i    dpiX: %i",dpiY,dpiX);
qDebug("pdWidth: %i    box myRect w: %i  h %i",pdWidth,myRect.width(),myRect.height());
    */
//myRect.setSize(myRect.size()*6);


    QRect myFrameRect( frameRect );
    if ( myRect.isValid() ) {
//qDebug("box myRect center:  x %i  y %i",myRect.center().x(),myRect.center().y());
        if( _rotation ){
            getTrueShift( areaWidthP1000, areaHeightP1000, myRect.height(),
                          rotDX, rotDY );
            myRect.moveBy( -rotDX, -rotDY );
            if( frame ) 
                myFrameRect.moveBy( -rotDX, -rotDY );
//qDebug("\nrotDelta:  x %i  y %i",rotDX,rotDY);
//qDebug("\nbox myRect center:  x %i  y %i",myRect.center().x(),myRect.center().y());
            myRect.moveCenter( QPoint( anchor.x() - trueRectAlignX(myRect),
                                       anchor.y() - trueRectAlignY(myRect) ) );
            if( frame )
                myFrameRect.moveCenter( QPoint( anchor.x() - trueRectAlignX(myFrameRect),
                                                anchor.y() - trueRectAlignY(myFrameRect) ) );
//qDebug("box myRect center:  x %i  y %i",myRect.center().x(),myRect.center().y());
            painter->translate( anchor.x(), anchor.y() );
            painter->rotate( _rotation );
            painter->translate( rotDX, rotDY );
        }
        if( frame )
            frame->paint( painter, KDFrame::PaintAll, myFrameRect );
        if ( _fontSize ) {
	  QFont font( content().font() );
          
          float trueSize = trueFontSize(areaWidthP1000,areaHeightP1000, myRect.height() );
          font.setPointSizeFloat( trueSize );  
          //adjust the area height related to the font size 
          myRect.setHeight( (int)(trueSize )+ (int)(trueSize*0.5));     
	  const KDChartTextPiece tmpTextPiece( painter, content().text(), font );
          
	  tmpTextPiece.draw( painter, myRect.x(), myRect.y(), myRect,
			     color ? *color :  _color,
			     paper ?  paper : &_paper );
        }else{
	  
	  content().draw( painter, myRect.x(), myRect.y(), myRect,
			  color ? *color :  _color,
			  paper ?  paper : &_paper );
        }
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
    KDXML::createIntNode( document, parent, "Rotation", custombox->_rotation );
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
    KDXML::createIntNode( document, parent, "DeltaAlign",
            custombox->_deltaAlign );
    KDXML::createBoolNode( document, parent, "DeltaScaleGlobal",
            custombox->_deltaScaleGlobal );
}


bool KDChartCustomBox::readCustomBoxNode( const QDomElement& element,
        KDChartCustomBox& custombox )
{
    bool ok = true;
    QString tempContentText;
    QFont tempContentFont;
    int tempRotation = 0; // must be initialized: new parameter, not present in old versions of KDChart
    int tempDeltaAlign = KDCHART_AlignAuto; // must be initialized too: new parameter
    bool tempDeltaScaleGlobal = true;   // must be initialized too: new parameter
    int tempFontSize, tempDeltaX, tempDeltaY,
    tempWidth, tempHeight, tempAnchorArea, tempAnchorAlign,
    tempDataRow, tempDataCol, tempData3rd;
    bool tempFontScaleGlobal;
    QColor tempColor;
    QBrush tempPaper;
    KDChartEnums::PositionFlag tempAnchorPos = KDChartEnums::PosTopLeft;
    QDomNode node = element.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "Rotation" ) {
                ok = ok & KDXML::readIntNode( element, tempRotation );
            } else if( tagName == "ContentText" ) {
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
            } else if( tagName == "DeltaAlign" ) {
                ok = ok & KDXML::readIntNode( element, tempDeltaAlign );
            } else if( tagName == "DeltaScaleGlobal" ) {
                ok = ok & KDXML::readBoolNode( element, tempDeltaScaleGlobal );
            } else {
                qDebug( "Unknown tag in custom box" );
            }
        }
        node = node.nextSibling();
    }

    if( ok ) {
        const KDChartTextPiece piece( 0, tempContentText, tempContentFont );
        custombox._content.deepCopy( &piece );
        custombox._rotation = tempRotation;
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
        custombox._deltaAlign       = tempDeltaAlign;
        custombox._deltaScaleGlobal = tempDeltaScaleGlobal;
    }

    return ok;
}

#include "KDChartCustomBox.moc"
