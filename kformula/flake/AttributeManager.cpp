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
    m_currentElement = 0;
}

AttributeManager::~AttributeManager()
{}

QString AttributeManager::findValue( const QString& attribute, const BasicElement* element ) const
{
    m_currentElement = element;

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

double AttributeManager::mathSize( BasicElement* element )
{
    QString value = findValue( "mathsize", element );
/*    if( value == "small" )
    else if( value == "normal" )
    else if( value == "big" )
     | number v-unit*/
    return 0.0;
}

QColor AttributeManager::mathColor( BasicElement* element )
{
    return QColor( findValue( "mathcolor", element ) );
}

QColor AttributeManager::mathBackground( BasicElement* element )
{
    return QColor( findValue( "mathbackground", element ) );
}

MathVariant AttributeManager::mathVariant( BasicElement* element )
{
    QString value = findValue( "mathvariant", element );
    if( value == "normal" )
        return Normal;
    else if( value == "bold" )
        return Bold;
    else if( value == "italic" )
        return Italic;
    else if( value == "bold-italic" )
        return BoldItalic;
    else if( value == "double-struck" )
        return DoubleStruck;
    else if( value == "bold-fraktur" )
        return BoldFraktur;
    else if( value == "script" )
        return Script;
    else if( value == "bold-script" )
        return BoldScript;
    else if( value == "fraktur" )
        return Fraktur;
    else if( value == "sans-serif" )
        return SansSerif;
    else if( value == "bold-sans-serif" )
        return BoldSansSerif;
    else if( value == "sans-serif-italic" )
        return SansSerifItalic;
    else if( value == "sans-serif-bold-italic" )
        return SansSerifBoldItalic;
    else if( value == "monospace" )
        return Monospace;
    else
        return InvalidMathVariant;   // Invalid
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

bool AttributeManager::displayStyle( BasicElement* element ) const
{
    return true;   // TODO replace this dummy
}

int AttributeManager::scriptLevel( BasicElement* element )
{
    if( element->parentElement() == m_currentElement )  // the asking element is a child
    {
        m_currentElement = element;
        parseScriptLevel( element );     // only parse for this element
        return m_cachedScriptLevel;
    }

    BasicElement* tmpParent = element->parentElement();
    QList<BasicElement*> tmpList;
    while( tmpParent )
    {
        tmpList.prepend( tmpParent );
        tmpParent = tmpParent->parentElement();
    }

    m_cachedScriptLevel = 0;
    foreach( BasicElement* tmp, tmpList )
        parseScriptLevel( tmp );

    return m_cachedScriptLevel;
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

bool AttributeManager::boolOf( const QString& attribute, const BasicElement* element ) const
{
    return findValue( attribute, element ) == "true";
}

double AttributeManager::doubleOf( const QString& attribute, BasicElement* element ) const
{
    QString tmpValue = findValue( attribute, element );
    QString unit = tmpValue.right( 2 );

    if( unit == "in" || unit == "cm" || unit == "pc" || unit == "mm" || unit == "pt" )
        return KoUnit::parseValue( tmpValue );
    else if( tmpValue.endsWith( '%' ) )
    {
   /*     tmpValue.chop( 1 );
        return defaultValueOf( m_attribute ) * (tmpValue.toDouble()/100);  */
    }

    tmpValue.chop( 2 );
    if( unit == "em" )
        return calculateEmExUnits( tmpValue.toDouble(), true );
    else if( unit == "ex" )
        return calculateEmExUnits( tmpValue.toDouble(), false );
    else if( unit == "px" )
        return m_viewConverter->viewToDocumentX( tmpValue.toInt() );
    else
        return 0.0;   // actually a value should never be 0.0
}

int AttributeManager::intOf( const QString& attribute, BasicElement* element ) const
{
    return findValue( attribute, element ).toInt();
}

Form AttributeManager::parseForm( const QString& value ) const
{
    if( value == "prefix" )
        return Prefix;
    else if( value == "infix" )
        return Infix;
    else if( value == "postfix" )
        return Postfix;
    else
        return InvalidForm;
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

void AttributeManager::parseScriptLevel( BasicElement* element )
{
    // set the scriptlevel explicitly
    QString value = element->attribute( "scriptlevel" );
    if( !value.isEmpty() )
    {
        if( value.startsWith( '+' ) )
            m_cachedScriptLevel += value.remove( 0, 1 ).toInt();
        else if( value.startsWith( '-' ) )
            m_cachedScriptLevel -= value.remove( 0, 1 ).toInt();
        else
            m_cachedScriptLevel = value.toInt();
    }
    else if( element->elementType() == Formula ) 
        m_cachedScriptLevel = 0;

    ElementType parentType = element->parentElement()->elementType();
    if( parentType == UnderOver || parentType == Under || parentType == Over )
        m_cachedScriptLevel++;
    else if( parentType == MultiScript || parentType == SupScript ||
             parentType == SubScript || parentType == SubSupScript )
        m_cachedScriptLevel++;
    else if( parentType == Fraction && displayStyle( element ) )
        m_cachedScriptLevel++;
    else if( parentType == Root && 
             element == element->parentElement()->childElements().value( 1 ) )
        m_cachedScriptLevel += 2;     // only for roots index
}

double AttributeManager::mathSpaceValue( const QString& value )  const
{
    if( value == "negativeveryverythinmathspace" )
        return -1*calculateEmExUnits( 0.055556, true );
    else if( value == "negativeverythinmathspace" )
        return -1*calculateEmExUnits( 0.111111, true );
    else if( value == "negativethinmathspace" )
        return -1*calculateEmExUnits( 0.166667, true );
    else if( value == "negativemediummathspace" )
        return -1*calculateEmExUnits( 0.222222, true );
    else if( value == "negativethickmathspace" )
        return -1*calculateEmExUnits( 0.277778, true );
    else if( value == "negativeverythickmathspace" )
        return -1*calculateEmExUnits( 0.333333, true );
    else if( value == "negativeveryverythickmathspace" )
        return -1*calculateEmExUnits( 0.388889, true );
    else if( value == "veryverythinmathspace" )
        return calculateEmExUnits( 0.055556, true );
    else if( value == "verythinmathspace" )
        return calculateEmExUnits( 0.111111, true );
    else if( value == "thinmathspace" )
        return calculateEmExUnits( 0.166667, true );
    else if( value == "mediummathspace" )
        return calculateEmExUnits( 0.222222, true );
    else if( value == "thickmathspace" )
        return calculateEmExUnits( 0.277778, true );
    else if( value == "verythickmathspace" )
        return calculateEmExUnits( 0.333333, true );
    else if( value == "veryverythickmathspace" )
        return calculateEmExUnits( 0.388889, true );
    else
        return -1.0;
}

double AttributeManager::calculateEmExUnits( double value, bool isEm ) const
{
    // use a postscript paint device so that font metrics returns postscript points
    KoPostscriptPaintDevice paintDevice;
    QFontMetricsF fm( font( m_currentElement ), &paintDevice );
    if( isEm )
        return value* fm.width( 'm' );
    else
        return value* fm.xHeight();
}

void AttributeManager::setViewConverter( KoViewConverter* converter )
{
    m_viewConverter = converter;
}
