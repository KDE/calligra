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

bool AttributeManager::boolOf( const QString& attribute, const BasicElement* element ) const
{
    return findValue( attribute, element ) == "true";
}

double AttributeManager::doubleOf( const QString& attribute,
                                   const BasicElement* element ) const
{
    // lookup value
    QString tmpValue = findValue( attribute, element );

    // test for value without unit
    if( tmpValue.toDouble() != 0 )
        return tmpValue.toDouble();

    // process values with units
    QString unit = tmpValue.right( 2 );
    tmpValue.endsWith( "%" ) ? tmpValue.chop( 1 ) : tmpValue.chop( 2 );

    if( unit == "in" || unit == "cm" || unit == "pc" || unit == "mm" || unit == "pt" )
        return KoUnit::parseValue( tmpValue + unit );
//    else if( tmpValue.endsWith( '%' ) )
//        return defaultValueOf( m_attribute ) * ( tmpValue.toDouble()/100 );
    else if( unit == "em" )
        return calculateEmEx( font( element ), tmpValue.toDouble(), true );
    else if( unit == "ex" )
        return calculateEmEx( font( element ), tmpValue.toDouble(), false );
    else if( unit == "px" )
        return m_viewConverter->viewToDocumentX( tmpValue.toInt() );
    else
        return 0.0;   // actually a value should never be 0.0
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

QFont AttributeManager::font( const BasicElement* element ) const
{
    
    // TODO process the mathvariant values partly
    // normal -> do nothing.
    // if contains bold -> font.setBold( true )
    // if contains italic -> font.setItalic( true )
    // if contains sans-serif setStyleHint( SansSerif ) --> Helvetica
  
    return QFont();
}

double AttributeManager::layoutSpacing( const BasicElement* element ) const
{
    // return a thinmathspace which is a good value for layouting
    return calculateEmEx( font( element ), 0.166667, true );
}

double AttributeManager::calculateEmEx( QFont font, double value, bool isEm ) const
{
    // use a postscript paint device so that font metrics returns postscript points
    KoPostscriptPaintDevice paintDevice;
    QFontMetricsF fm( font, &paintDevice );
    return isEm ? value*fm.width( 'm' ) : value*fm.xHeight();
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

void AttributeManager::setViewConverter( KoViewConverter* converter )
{
    m_viewConverter = converter;
}




double AttributeManager::mathSize( BasicElement* element )
{
    QString value = findValue( "mathsize", element );
/*    if( value == "small" )
    else if( value == "normal" )
    else if( value == "big" )
     | number v-unit*/
    return 0.0;
}

QString AttributeManager::stringOf( const QString& attribute, BasicElement* element ) const
{
    return findValue( attribute, element );
}

QStringList AttributeManager::stringListOf( const QString& attribute,
                                            BasicElement* element ) const
{
    // TODO implement parsing
    return QStringList();
}





int AttributeManager::intOf( const QString& attribute, BasicElement* element ) const
{
    return findValue( attribute, element ).toInt();
}

