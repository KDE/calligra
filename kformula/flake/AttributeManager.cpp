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
#include <KoUnit.h>
#include <KoViewConverter.h>
#include <KoPostscriptPaintDevice.h>
#include <QFontMetricsF>
#include <QColor>

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

double AttributeManager::scriptLevelScaling( const BasicElement* element ) const
{
    double multiplier = doubleOf( "scriptsizemultiplier", element );
    if( multiplier == 0.0 )
        multiplier = 0.71;
/* 
    ElementType parentType = element->parentElement()->elementType();
    if( element->elementType() == Formula ) // Outermost element has scriptlevel 0
        return 1.0;
    else if( parentType == Fraction && displayStyle == false )
        return multiplier;
    else if( parentType == Sub || parentType == Sup || parentType == SubSup )
        return multiplier;
    else if( parentType == Under && accentunder == false )
    else if( parentType == Over && accent == false )
    else if( parentType == UnderOver && accent == false && is over )
        return multiplier;
    else if( parentType == UnderOver && accentunder == false && is under )
        return multiplier;
    else if( parentType == MultiScript )
        return multiplier ^ ;
    else if( parentType == Root && element->childElements().indexOf( element ) ==  )
        return multiplier ^ ;
    else if( parentType == Table )
        return multiplier ^ ;
    else if( element->elementType() == Style ) {
        QString tmp = element->attribute( "scriptlevel" );
        if( tmp.startsWith( "+" ) || tmp.startsWith( "-" ) )
            return multiplier^tmp.remove( 0, 1 ).toInt()
        else
            return multiplier^tmp.toInt() / element->parentElement()->scaleFactor(); 
    }
    else*/
        return 1.0;
}

double AttributeManager::layoutSpacing( const BasicElement* element ) const
{
    // return a thinmathspace which is a good value for layouting
    return parseUnit( "0.166667em", element );
}

double AttributeManager::parseUnit( const QString& value,
                                    const BasicElement* element ) const
{
    // test for value without unit
    if( value.toDouble() != 0 )
        return value.toDouble();

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
  
    return QFont();
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

