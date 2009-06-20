/* This file is part of the KDE project
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>

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

#include "AttributeManager.h"
#include "BasicElement.h"
#include "ElementFactory.h"
#include <KoUnit.h>
#include <KoViewConverter.h>
#include <KoPostscriptPaintDevice.h>
#include <QFontMetricsF>
#include <QColor>
#include <kdebug.h>

AttributeManager::AttributeManager()
{
    m_viewConverter = 0;
}

AttributeManager::~AttributeManager()
{}

QString AttributeManager::findValue( const QString& attribute, const BasicElement* element ) const
{
    // check if the current element has a value assigned
    QString value = element->attribute( attribute );
    if( !value.isEmpty() )
        return value;

    // if not, check if any of the parent elements inherits a value
    BasicElement* tmpParent = element->parentElement();
    while( tmpParent )
    {
        value = tmpParent->inheritsAttribute( attribute );
        if( !value.isEmpty() )
            return value;
        else
            tmpParent = tmpParent->parentElement();
    }

    // if not, return the default value of the attribute
    return element->attributesDefaultValue( attribute );
}

bool AttributeManager::boolOf( const QString& attribute,
                               const BasicElement* element ) const
{
    return findValue( attribute, element ) == "true";
}

double AttributeManager::doubleOf( const QString& attribute,
                                   const BasicElement* element ) const
{
    return parseUnit( findValue( attribute, element ), element );
}

QList<double> AttributeManager::doubleListOf( const QString& attribute,
                                              const BasicElement* element ) const
{
    QList<double> doubleList;
    QStringList tmp = findValue( attribute, element ).split( " " );
    foreach( QString doubleValue, tmp )
        doubleList << parseUnit( doubleValue, element );

    return doubleList;
}

QString AttributeManager::stringOf( const QString& attribute, BasicElement* element ) const
{
    return findValue( attribute, element );
}

QColor AttributeManager::colorOf( const QString& attribute, BasicElement* element ) const
{
    QString tmpColor = findValue( attribute, element );
    if( attribute == "mathbackground" && tmpColor.isEmpty() )
        return Qt::transparent;

    return QColor( tmpColor );
}

Align AttributeManager::alignOf( const QString& attribute, BasicElement* element ) const
{
    return parseAlign( findValue( attribute, element ) );
}

QList<Align> AttributeManager::alignListOf( const QString& attribute,
                                            BasicElement* element ) const
{
    QList<Align> alignList;
    QStringList tmpList = findValue( attribute, element ).split( " " );

    foreach( QString tmp, tmpList )
        alignList << parseAlign( tmp );

    return alignList;
}

Qt::PenStyle AttributeManager::penStyleOf( const QString& attribute,
                                           BasicElement* element ) const
{
    return parsePenStyle( findValue( attribute, element ) );
}

QList<Qt::PenStyle> AttributeManager::penStyleListOf( const QString& attribute,
                                                      BasicElement* element ) const
{
    QList<Qt::PenStyle> penStyleList;
    QStringList tmpList = findValue( attribute, element ).split( " " );

    foreach( QString tmp, tmpList )
        penStyleList << parsePenStyle( tmp );

    return penStyleList;
}

int AttributeManager::scriptLevel( const BasicElement* parent, int index ) const
{
    ElementType parentType = parent->elementType();
    int current_scaleLevel = parent->scaleLevel();

    /** First check for types where all children are scaled */
    switch(parentType) {
        case Fraction:
            if( parent->displayStyle() == false )
                return current_scaleLevel+1;
	    else 
		return current_scaleLevel;
	case Style: {
            QString tmp = parent->attribute( "scriptlevel" );
            if( tmp.startsWith( '+' ) )
		    return current_scaleLevel + tmp.remove(0,1).toInt();
	    if( tmp.startsWith( '-' ) )
		    return current_scaleLevel - tmp.remove(0,1).toInt();
            return tmp.toInt(); 
	}
	case MultiScript:
	    return current_scaleLevel + 1;
	case Table:
	    return current_scaleLevel + 1;
	default:
	    break;
    }
    if( index == 0) return current_scaleLevel;
    /** Now check for types where the first child isn't scaled, but the rest are */
    switch(parentType) {
	    case SubScript:
	    case SupScript:
	    case SubSupScript:
        	return current_scaleLevel + 1;
	    case Under:
		if( boolOf("accentunder", parent) )
	    	    return current_scaleLevel + 1;
		else
	    	    return current_scaleLevel;
	    case Over:
		if( boolOf("accent", parent) )
	    	    return current_scaleLevel + 1;
		else
	    	    return current_scaleLevel;
	    case UnderOver:
		if( (index == 1 && boolOf("accentunder", parent)) || (index == 2 && boolOf("accent", parent)) ) 
	    	    return current_scaleLevel + 1;
		else
	    	    return current_scaleLevel;
	    case Root:
		/* second argument to root is the base */
	        return current_scaleLevel + 1;
            default:
	    	return current_scaleLevel;
    }
}

double AttributeManager::layoutSpacing( const BasicElement* element ) const
{
    // return a thinmathspace which is a good value for layouting
    return parseUnit( "0.166667em", element );
}

double AttributeManager::parseUnit( const QString& value,
                                    const BasicElement* element ) const
{
    if (value.isEmpty())
        return 0;
    QRegExp re("(-?[\\d\\.]*)(px|em|ex|in|cm|pc|mm|pt)?", Qt::CaseInsensitive);
    if (re.indexIn(value) == -1)
        return 0;
    QString real = re.cap(1);
    QString unit = re.cap(2);

    bool ok;
    qreal number = real.toDouble(&ok);
    if (!ok)
        return 0;
    if(!unit.isEmpty()) {
        if (unit.compare("em", Qt::CaseInsensitive) == 0) {
            QFontMetrics fm(font(element));
            return fm.height() * number;
        }
        else if (unit.compare("ex", Qt::CaseInsensitive) == 0) {
            QFontMetrics fm(font(element));
            return fm.xHeight() * number;
        }
    }
    
    return number;
    
    //FIXME - parse the other units - in, cm etc


    /*
    // process values with units
    QString unit = value.right( value.endsWith( '%' ) ? 1 : 2 );
    double v = value.left( value.length() - unit.length() ).toDouble();

    if( unit == "in" || unit == "cm" || unit == "pc" || unit == "mm" || unit == "pt" )
        return KoUnit::parseValue( QString::number( v ) + unit );
    else if( unit == "em" || unit == "ex" ) {
        // use a postscript paint device so that font metrics returns postscript points
        KoPostscriptPaintDevice paintDevice;
        QFontMetricsF fm( font( element ), &paintDevice );
        return ( unit == "em" ) ? v*fm.width( 'm' ) : v*fm.xHeight();
    }
//    else if( unit == "px" )
//        return m_viewConverter->viewToDocumentX( v.toInt() );
//    else if( tmpValue.endsWith( '%' ) )
//        return defaultValueOf( m_attribute ) * ( tmpValue.toDouble()/100 ); 
    else
        return 0.0;   // actually a value should never be 0.0
    */
}

Align AttributeManager::parseAlign( const QString& value ) const
{
    if( value == "right" )
        return Right;
    else if( value == "left" )
        return Left;
    else if( value == "center" )
        return Center;
    else if( value == "top" )
        return Top;
    else if( value == "bottom" )
        return Bottom;
    else if( value == "baseline" )
        return BaseLine;
    else if( value == "axis" )
        return Axis;
    else
        return InvalidAlign;
}

Qt::PenStyle AttributeManager::parsePenStyle( const QString& value ) const
{
    if( value == "solid" )
        return Qt::SolidLine;
    else if( value == "dashed" )
        return Qt::DashLine;
    else
        return Qt::NoPen;
}

QFont AttributeManager::font( const BasicElement* element ) const
{
    
    // TODO process the mathvariant values partly
    // normal -> do nothing.
    // if contains bold -> font.setBold( true )
    // if contains italic -> font.setItalic( true )
    // if contains sans-serif setStyleHint( SansSerif ) --> Helvetica
  
    QFont font;
     if(font.pointSizeF() != -1)
         font.setPointSizeF(font.pointSizeF() * element->scaleFactor());
     else
         font.setPixelSize( font.pixelSize() * element->scaleFactor() );
     return font;
}

void AttributeManager::setViewConverter( KoViewConverter* converter )
{
    m_viewConverter = converter;
}

double AttributeManager::maxHeightOfChildren( BasicElement* element ) const
{
    double maxHeight = 0.0;
    foreach( BasicElement* tmp, element->childElements() )
        maxHeight = qMax( maxHeight, tmp->height() );

    return maxHeight; 
}

double AttributeManager::maxWidthOfChildren( BasicElement* element ) const
{
    double maxWidth = 0.0;
    foreach( BasicElement* tmp, element->childElements() )
        maxWidth = qMax( maxWidth, tmp->width() );

    return maxWidth; 
}
double AttributeManager::parseMathSpace( const QString& value, BasicElement *element )  const
{
    QFontMetrics fm(font(element));
    qreal conversionEmToPixels = fm.xHeight();

    if( value == "negativeveryverythinmathspace" )
        return -1*conversionEmToPixels*0.055556;
    else if( value == "negativeverythinmathspace" )
        return -1*conversionEmToPixels*0.111111;
    else if( value == "negativethinmathspace" )
        return -1*conversionEmToPixels*0.166667;
    else if( value == "negativemediummathspace" )
        return -1*conversionEmToPixels*0.222222;
    else if( value == "negativethickmathspace" )
        return -1*conversionEmToPixels*0.277778;
    else if( value == "negativeverythickmathspace" )
        return -1*conversionEmToPixels*0.333333;
    else if( value == "negativeveryverythickmathspace" )
        return -1*conversionEmToPixels*0.388889;
    else if( value == "veryverythinmathspace" )
        return conversionEmToPixels*0.055556;
    else if( value == "verythinmathspace" )
        return conversionEmToPixels*0.111111;
    else if( value == "thinmathspace" )
        return conversionEmToPixels*0.166667;
    else if( value == "mediummathspace" )
        return conversionEmToPixels*0.222222;
    else if( value == "thickmathspace" )
        return conversionEmToPixels*0.277778;
    else if( value == "verythickmathspace" )
        return conversionEmToPixels*0.333333;
    else if( value == "veryverythickmathspace" )
        return conversionEmToPixels*0.388889;
    else
        return 0;
}

