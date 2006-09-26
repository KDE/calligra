/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "kivio_common.h"
#include "kivio_fill_style.h"
#include "kivio_line_style.h"
#include "kivio_point.h"
#include "kivio_shape_data.h"
#include "kivio_text_style.h"

#include <qpainter.h>
#include <kdebug.h>

/*
 * Struct for holding information about a shape type
 */
struct KivioShapeTypeInfo
{
    const char *name;
    KivioShapeData::KivioShapeType type;
};

/*
 * Array of shape info used for loading/saving.
 */
static const int numShapeInfo = 12;
static struct KivioShapeTypeInfo shapeInfo[] = {
    { "Arc",        KivioShapeData::kstArc                  },
    { "Pie",        KivioShapeData::kstPie                  },
    { "LineArray",  KivioShapeData::kstLineArray            },
    { "Polyline",   KivioShapeData::kstPolyline             },
    { "Polygon",    KivioShapeData::kstPolygon              },
    { "Bezier",     KivioShapeData::kstBezier               },
    { "Rectangle",  KivioShapeData::kstRectangle            },
    { "RoundRectangle",   KivioShapeData::kstRoundRectangle },
    { "Ellipse",    KivioShapeData::kstEllipse              },
    { "OpenPath",   KivioShapeData::kstOpenPath             },
    { "ClosedPath", KivioShapeData::kstClosedPath           },
    { "TextBox",    KivioShapeData::kstTextBox              }
};


/*****************
 * FIXME
 *
 * Down the road, this will be phased out because of the KivioTextStyle
 * class.  But it's here for now to keep backwards compatibility with
 * beta 2 -- even though no one should be using it for anything important!!!
 ****************/
KivioTextShapeData::KivioTextShapeData()
{
    m_text = "";
    m_textFont = QFont("Times");
    m_textColor = QColor(0,0,0);
    m_isHtml = false;
    m_hTextAlign = Qt::AlignHCenter;
    m_vTextAlign = Qt::AlignVCenter;
}


KivioShapeData::KivioShapeData()
    : m_pOriginalPointList(NULL),
      m_pFillStyle(NULL)
{
    m_pOriginalPointList = new QPtrList<KivioPoint>;
    m_pOriginalPointList->setAutoDelete(true);

    m_pFillStyle = new KivioFillStyle();
    m_pLineStyle = new KivioLineStyle();
//    m_fgColor = QColor( 0, 0, 0 );

    m_shapeType = kstNone;
    m_name = "";
//    m_lineWidth = 1.0f;

    m_pTextData = NULL;

    m_position.set( 0.0f, 0.0f );
    m_dimensions.set( 72.0f, 72.0f );
}

KivioShapeData::KivioShapeData( const KivioShapeData &source )
    : m_pOriginalPointList(NULL),
      m_pFillStyle(NULL)
{
    // Allocate a new point list
    KivioPoint *pPoint;
    m_pOriginalPointList = new QPtrList<KivioPoint>;
    m_pOriginalPointList->setAutoDelete(true);

    // Copy over the point list
    pPoint = source.m_pOriginalPointList->first();
    while( pPoint )
    {
        m_pOriginalPointList->append( new KivioPoint( *pPoint ) );
        pPoint = source.m_pOriginalPointList->next();
    }

    // Copy the fill/line styles
    m_pFillStyle = new KivioFillStyle( *(source.m_pFillStyle) );
    m_pLineStyle = new KivioLineStyle( *(source.m_pLineStyle) );

    // Copy the fg color
    //m_fgColor = source.m_fgColor;

    // Copy the rest
    m_shapeType = source.m_shapeType;
    m_name      = QString(source.m_name);
    //m_lineWidth = source.m_lineWidth;

    // Copy the position and size
    source.m_position.copyInto( &m_position );
    source.m_dimensions.copyInto( &m_dimensions );

    // If this is a text shape, allocate a text data struct and copy the info
    if( m_shapeType == kstTextBox )
    {
        m_pTextData = new KivioTextStyle();

        source.m_pTextData->copyInto( m_pTextData );
/*
        m_pTextData->m_text       = ((KivioShapeData)source).text();
        m_pTextData->m_isHtml     = ((KivioShapeData)source).isHtml();
        m_pTextData->m_hTextAlign = ((KivioShapeData)source).hTextAlign();
        m_pTextData->m_vTextAlign = ((KivioShapeData)source).vTextAlign();
        m_pTextData->m_textFont   = ((KivioShapeData)source).textFont();
        m_pTextData->m_textColor  = ((KivioShapeData)source).textColor();
*/
    }
    else
        m_pTextData = NULL;

}


KivioShapeData::~KivioShapeData()
{
    if( m_pOriginalPointList )
    {
        delete m_pOriginalPointList;
        m_pOriginalPointList = NULL;
    }

    if( m_pFillStyle )
    {
        delete m_pFillStyle;
        m_pFillStyle = NULL;
    }

    if( m_pLineStyle )
    {
        delete m_pLineStyle;
        m_pLineStyle = NULL;
    }

    if( m_pTextData )
    {
        delete m_pTextData;
        m_pTextData = NULL;
    }
}


void KivioShapeData::copyInto( KivioShapeData *pTarget ) const
{
    KivioPoint *pPoint;

    if( !pTarget )
        return;

    // Delete the old point list
    if( pTarget->m_pOriginalPointList )
    {
        delete pTarget->m_pOriginalPointList;
        pTarget->m_pOriginalPointList = NULL;
    }

    // Create a new point list and copy it over
    pTarget->m_pOriginalPointList = new QPtrList<KivioPoint>;
    pTarget->m_pOriginalPointList->setAutoDelete(true);
    pPoint = m_pOriginalPointList->first();
    while( pPoint )
    {
        pTarget->m_pOriginalPointList->append( new KivioPoint( *pPoint ) );
        pPoint = m_pOriginalPointList->next();
    }

    // Copy the fill/line styles
    m_pFillStyle->copyInto( pTarget->m_pFillStyle );
    m_pLineStyle->copyInto( pTarget->m_pLineStyle );

    // Copy the fg color
    //pTarget->m_fgColor = m_fgColor;

    // Copy the rest
    pTarget->m_shapeType = m_shapeType;
    pTarget->m_name = QString(m_name);
    //pTarget->m_lineWidth = m_lineWidth;

    m_position.copyInto( &(pTarget->m_position) );
    m_dimensions.copyInto( &(pTarget->m_dimensions) );


    // If this is a textbox, allocate & copy
    if( m_shapeType == kstTextBox )
    {
        if( !pTarget->m_pTextData )
        {
            pTarget->m_pTextData = new KivioTextStyle();
        }

        if( m_pTextData )
        {
            m_pTextData->copyInto( pTarget->m_pTextData );
        }
        else
        {
	   kWarning(43000) << "KivioShapeData::copyInto() - Shape is of type text-box, but our text data doesn't exist." << endl;
            pTarget->m_pTextData->setText("");
            pTarget->m_pTextData->setIsHtml(false);
            pTarget->m_pTextData->setHTextAlign(Qt::AlignHCenter);
            pTarget->m_pTextData->setVTextAlign(Qt::AlignVCenter);
            pTarget->m_pTextData->setFont( QFont("Times",12) );
            pTarget->m_pTextData->setColor( QColor(0,0,0) );
        }
    }
    else
    {
        if( pTarget->m_pTextData )
        {
            delete pTarget->m_pTextData;
            pTarget->m_pTextData = NULL;
        }
    }
}


/**
 * Load this object from an XML element
 *
 */
bool KivioShapeData::loadXML( const QDomElement &e )
{
    QDomNode node;
    QDomElement ele;

    // Maintain backwards compatibility with the eariler betas. They saved
    // fg color and line style in this node.
    m_pLineStyle->setColor( XmlReadColor( e, "fgColor", QColor(0,0,0) ) );
    m_pLineStyle->setWidth( XmlReadFloat( e, "lineWidth", 1.0f ) );


    node = e.firstChild();
    while( !node.isNull() )
    {
        QString nodeName = node.nodeName();
        ele = node.toElement();

        if( nodeName == "KivioLineStyle" )
        {
            m_pLineStyle->loadXML( ele );
        }
        else if( nodeName == "KivioFillStyle" )
        {
            m_pFillStyle->loadXML( ele );
        }
        else if( nodeName == "KivioTextStyle" )
        {

            // First make sure we are a text box
            if( m_shapeType == kstTextBox )
            {
                // If we don't have text data, allocate it
                if( !m_pTextData )
                    m_pTextData = new KivioTextStyle();

                m_pTextData->loadXML( ele );

            } // end if m_shapeType==kstTextBox
        }
        else if( nodeName == "KivioText" )
        {
            // First make sure we are a text box
            if( m_shapeType == kstTextBox )
            {
                KivioTextShapeData *pData = new KivioTextShapeData;

                pData->m_text = XmlReadString( ele, "text", "" );
                pData->m_isHtml = (bool)XmlReadInt( ele, "isHtml", (int)false );

                pData->m_hTextAlign = XmlReadInt( ele, "hTextAlign", Qt::AlignHCenter );
                pData->m_vTextAlign = XmlReadInt( ele, "vTextAlign", Qt::AlignVCenter );

                // Search for the font
                QDomNode innerNode = ele.firstChild();
                while( !innerNode.isNull() )
                {
                    QString innerName = innerNode.nodeName();
                    QDomElement innerE = innerNode.toElement();

                    if( innerName == "TextFont" )
                    {
                        pData->m_textFont.setFamily( XmlReadString(innerE, "family", "times") );
                        pData->m_textFont.setPointSize( XmlReadInt(innerE, "size", 12 ) );
                        pData->m_textFont.setBold( (bool)XmlReadInt( innerE, "bold", 12 ) );
                        pData->m_textFont.setItalic( (bool)XmlReadInt( innerE, "italic", 12 ) );
                        pData->m_textFont.setUnderline( (bool)XmlReadInt( innerE, "underline", 12 ) );
                        pData->m_textFont.setStrikeOut( (bool)XmlReadInt( innerE, "strikeOut", 12 ) );
                        pData->m_textFont.setFixedPitch( (bool)XmlReadInt( innerE, "fixedPitch", false ) );
                        pData->m_textColor = XmlReadColor( innerE, "color", QColor(0,0,0) );
                    }

                    innerNode = innerNode.nextSibling();
                }

                // Now convert it to a KivioTextStyle
                if( !m_pTextData )
                    m_pTextData = new KivioTextStyle();

                m_pTextData->setText( pData->m_text );
                m_pTextData->setHTextAlign( pData->m_hTextAlign );
                m_pTextData->setVTextAlign( pData->m_vTextAlign );
                m_pTextData->setFont( pData->m_textFont );
                m_pTextData->setColor( pData->m_textColor );

            } // end if m_shapeType==kstTextBox
            else
            {
	       kDebug(43000) << "KivioShapeData::loadXML() - Loading KivioText, but this is not a textbox!" << endl;
            }
        }

        node = node.nextSibling();
    }

    return true;
}

/**
 * Save this object to an XML element
 *
 */
QDomElement KivioShapeData::saveXML( QDomDocument &doc )
{
    QDomElement e = doc.createElement("KivioShapeData");

    // FIXME: Do we need to save m_pOriginalPointList

    // We save all this, but we won't necessarily load it back.

    // The positions
    QDomElement posE = doc.createElement("Position");
    XmlWriteFloat( posE, "x", m_position.x() );
    XmlWriteFloat( posE, "y", m_position.y() );
    e.appendChild( posE );

    // The dimensions
    QDomElement dimE = doc.createElement("Dimension");
    XmlWriteFloat( dimE, "w", m_dimensions.x() );
    XmlWriteFloat( dimE, "h", m_dimensions.y() );
    e.appendChild( dimE );

    // The FGColor
//    QDomElement foreE = doc.createElement("Foreground");
//    XmlWriteUInt( foreE, "color", m_fgColor.rgb() );
//    XmlWriteFloat( foreE, "lineWidth", m_lineWidth );
//    e.appendChild( foreE );
    QDomElement foreE = m_pLineStyle->saveXML( doc );
    e.appendChild( foreE );


    // Save if we are a text box etc...
    if( m_shapeType == kstTextBox )
    {
        if( m_pTextData )
        {
            e.appendChild( m_pTextData->saveXML(doc) );
        /*
            // The text & formatting
            QDomElement textE = doc.createElement("KivioText");
            XmlWriteString( textE, "text", m_pTextData->m_text );
            XmlWriteInt( textE, "isHtml", m_pTextData->m_isHtml );
            XmlWriteInt( textE, "hTextAlign", m_pTextData->m_hTextAlign );
            XmlWriteInt( textE, "vTextAlign", m_pTextData->m_vTextAlign );

            // Text font & color
            QDomElement innerTextE = doc.createElement("TextFont");
            XmlWriteString( innerTextE, "family", m_pTextData->m_textFont.family() );
            XmlWriteColor( innerTextE, "color", m_pTextData->m_textColor );
            XmlWriteInt( innerTextE, "size", m_pTextData->m_textFont.pointSize() );
            XmlWriteInt( innerTextE, "bold", m_pTextData->m_textFont.bold() );
            XmlWriteInt( innerTextE, "italic", m_pTextData->m_textFont.italic() );
            XmlWriteInt( innerTextE, "underline", m_pTextData->m_textFont.underline() );
            XmlWriteInt( innerTextE, "strikeOut", m_pTextData->m_textFont.strikeOut() );
            XmlWriteInt( innerTextE, "fixedPitch", m_pTextData->m_textFont.fixedPitch() );

            textE.appendChild( innerTextE );
            e.appendChild( textE );
        */
        }
    }



    // The BGFillStyle
    e.appendChild( m_pFillStyle->saveXML( doc ) );

    // Shape type & name are stored in the shape node
    //XmlWriteInt( e, "shapeType", m_shapeType );
    //XmlWriteString( e, "name", m_name );

    return e;
}


KivioShapeData::KivioShapeType KivioShapeData::shapeTypeFromString( const QString &str )
{

    for( int i=0; i<numShapeInfo; i++ )
    {
        if( str.compare( shapeInfo[i].name )==0 )
            return shapeInfo[i].type;
    }

    return kstNone;
}

void KivioShapeData::setShapeType( KivioShapeType st )
{
    m_shapeType = st;

    // If it is a text box, make sure we have text data
    if( st == kstTextBox )
    {
        if( !m_pTextData )
            m_pTextData = new KivioTextStyle();
    }
    else    // Otherwise, make sure we DON'T have it.
    {
        if( m_pTextData )
        {
            delete m_pTextData;
            m_pTextData = NULL;
        }
    }
}


QString KivioShapeData::text()
{
    if( m_pTextData )
        return m_pTextData->text();

    return QString("");
}

void KivioShapeData::setText( const QString &newText )
{
    if( m_pTextData )
    {
        m_pTextData->setText(newText);
    }
}

bool KivioShapeData::isHtml() const
{
    if( m_pTextData )
        return m_pTextData->isHtml();

    return false;
}

void KivioShapeData::setIsHtml( bool b )
{
    if( m_pTextData )
        m_pTextData->setIsHtml(b);
}

int KivioShapeData::hTextAlign() const
{
    if( m_pTextData )
        return m_pTextData->hTextAlign();

    return Qt::AlignHCenter;
}

void KivioShapeData::setHTextAlign( int i )
{
    if( m_pTextData )
        m_pTextData->setHTextAlign(i);
}

int KivioShapeData::vTextAlign() const
{
    if( m_pTextData )
        return m_pTextData->vTextAlign();

    return Qt::AlignVCenter;
}

void KivioShapeData::setVTextAlign( int i )
{
    if( m_pTextData )
        m_pTextData->setVTextAlign(i);
}

QFont KivioShapeData::textFont()
{
    if( m_pTextData )
        return m_pTextData->font();

    return QFont("Times");
}

void KivioShapeData::setTextFont( const QFont &f )
{
    if( m_pTextData )
        m_pTextData->setFont(f);
}

QColor KivioShapeData::textColor()
{
    if( m_pTextData )
        return m_pTextData->color();

    return QColor(0,0,0);
}

void KivioShapeData::setTextColor( QColor c )
{
    if( m_pTextData )
        m_pTextData->setColor(c);
}

void KivioShapeData::setTextStyle( KivioTextStyle *pStyle )
{
   if( m_pTextData )
   {
      pStyle->copyInto( m_pTextData );
   }
}

void KivioShapeData::setLineStyle(KivioLineStyle ls)
{
    if(m_pLineStyle) {
      ls.copyInto(m_pLineStyle);
    }
}
