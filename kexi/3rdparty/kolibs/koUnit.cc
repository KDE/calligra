/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>
   Copyright (C) 2004, Nicolas GOUTTE <goutte@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

//#include <KoGlobal.h>
#include "KoUnit.h"
#ifndef SIMPLE_KOLIBS
# include <KoXmlWriter.h>
#endif

#include <klocale.h>
#include <kglobal.h>
#include <kdebug.h>

#include <qregexp.h>
#include <qdom.h>

QStringList KoUnit::listOfUnitName()
{
    QStringList lst;
    for ( uint i = 0 ; i <= KoUnit::LastUnit ; ++i )
    {
        KoUnit unit = static_cast<KoUnit>( i );
        lst.append( KoUnit::unitDescription( unit ) );
    }
    return lst;
}

QString KoUnit::unitDescription( Unit _unit )
{
    switch ( _unit )
    {
    case KoUnit::Millimeter:
        return i18n("Millimeters (mm)");
    case KoUnit::Centimeter:
        return i18n("Centimeters (cm)");
    case KoUnit::Decimeter:
        return i18n("Decimeters (dm)");
    case KoUnit::Inch:
        return i18n("Inches (in)");
    case KoUnit::Pica:
        return i18n("Pica (pi)");
    case KoUnit::Didot:
        return i18n("Didot (dd)");
    case KoUnit::Cicero:
        return i18n("Cicero (cc)");
    case KoUnit::Point:
        return i18n("Points (pt)" );
    default:
        return i18n("Error!");
    }
}

double KoUnit::toUserValue( double ptValue, Unit unit )
{
    switch ( unit ) {
    case Millimeter:
        return toMM( ptValue );
    case Centimeter:
        return toCM( ptValue );
    case Decimeter:
        return toDM( ptValue );
    case Inch:
        return toInch( ptValue );
    case Pica:
        return toPI( ptValue );
    case Didot:
        return toDD( ptValue );
    case Cicero:
        return toCC( ptValue );
    case Point:
    default:
        return toPoint( ptValue );
    }
}

double KoUnit::ptToUnit( const double ptValue, const Unit unit )
{
    switch ( unit )
    {
    case Millimeter:
        return POINT_TO_MM( ptValue );
    case Centimeter:
        return POINT_TO_CM( ptValue );
    case Decimeter:
        return POINT_TO_DM( ptValue );
    case Inch:
        return POINT_TO_INCH( ptValue );
    case Pica:
        return POINT_TO_PI( ptValue );
    case Didot:
        return POINT_TO_DD( ptValue );
    case Cicero:
        return POINT_TO_CC( ptValue );
    case Point:
    default:
        return ptValue;
    }
}

QString KoUnit::toUserStringValue( double ptValue, Unit unit )
{
    return KGlobal::locale()->formatNumber( toUserValue( ptValue, unit ) );
}

double KoUnit::fromUserValue( double value, Unit unit )
{
    switch ( unit ) {
    case Millimeter:
        return MM_TO_POINT( value );
    case Centimeter:
        return CM_TO_POINT( value );
    case Decimeter:
        return DM_TO_POINT( value );
    case Inch:
        return INCH_TO_POINT( value );
    case Pica:
        return PI_TO_POINT( value );
    case Didot:
        return DD_TO_POINT( value );
    case Cicero:
        return CC_TO_POINT( value );
    case Point:
    default:
        return value;
    }
}

double KoUnit::fromUserValue( const QString& value, Unit unit, bool* ok )
{
    return fromUserValue( KGlobal::locale()->readNumber( value, ok ), unit );
}

double KoUnit::parseValue( QString value, double defaultVal )
{
    value.simplified();
    value.remove( ' ' );

    if( value.isEmpty() )
        return defaultVal;

    int index = value.find( QRegExp( "[a-z]+$" ) );
    if ( index == -1 )
        return value.toDouble();

    QString unit = value.mid( index );
    value.truncate ( index );
    double val = value.toDouble();

    if ( unit == "pt" )
        return val;

    bool ok;
    Unit u = KoUnit::unit( unit, &ok );
    if( ok )
        return fromUserValue( val, u );

    if( unit == "m" )
        return fromUserValue( val * 10.0, Decimeter );
    else if( unit == "km" )
        return fromUserValue( val * 10000.0, Decimeter );
    kWarning() << "KoUnit::parseValue: Unit " << unit << " is not supported, please report." << endl;

    // TODO : add support for mi/ft ?
    return defaultVal;
}

KoUnit KoUnit::unit( const QString &_unitName, bool* ok )
{
    if ( ok )
        *ok = true;
    if ( _unitName == QString::fromLatin1( "mm" ) ) return Millimeter;
    if ( _unitName == QString::fromLatin1( "cm" ) ) return Centimeter;
    if ( _unitName == QString::fromLatin1( "dm" ) ) return Decimeter;
    if ( _unitName == QString::fromLatin1( "in" )
         || _unitName == QString::fromLatin1("inch") /*compat*/ ) return Inch;
    if ( _unitName == QString::fromLatin1( "pi" ) ) return Pica;
    if ( _unitName == QString::fromLatin1( "dd" ) ) return Didot;
    if ( _unitName == QString::fromLatin1( "cc" ) ) return Cicero;
    if ( _unitName == QString::fromLatin1( "pt" ) ) return Point;
    if ( ok )
        *ok = false;
    return Point;
}

QString KoUnit::unitName( Unit _unit )
{
    if ( _unit == Millimeter ) return QString::fromLatin1( "mm" );
    if ( _unit == Centimeter ) return QString::fromLatin1( "cm" );
    if ( _unit == Decimeter ) return QString::fromLatin1( "dm" );
    if ( _unit == Inch ) return QString::fromLatin1( "in" );
    if ( _unit == Pica ) return QString::fromLatin1( "pi" );
    if ( _unit == Didot ) return QString::fromLatin1( "dd" );
    if ( _unit == Cicero ) return QString::fromLatin1( "cc" );
    return QString::fromLatin1( "pt" );
}

#ifndef SIMPLE_KOLIBS
void KoUnit::saveOasis(KoXmlWriter* settingsWriter, Unit _unit)
{
    settingsWriter->addConfigItem( "unit", unitName(_unit) );
}
#endif
