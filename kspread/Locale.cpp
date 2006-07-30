/* This file is part of the KDE project
   Copyright (C) 2001 Laurent Montel <montel@kde.org>
             (C) 2000 Torben Weis <weis@kde.org>

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

#include "Locale.h"

#include <qdom.h>

using namespace KSpread;

Locale::Locale()
	: KLocale("kspread")
{
	insertCatalog("koffice");
}

void Locale::load( const KoXmlElement& element )
{
    if ( element.hasAttribute( "weekStartsMonday" ) )
    {
	QString c = element.attribute( "weekStartsMonday" );
        if ( c != "False")
        {
          setWeekStartDay( 1 /*Monday*/ );
        }
    }
    if ( element.hasAttribute( "decimalSymbol" ) )
	setDecimalSymbol( element.attribute( "decimalSymbol" ) );
    if ( element.hasAttribute( "thousandsSeparator" ) )
	setThousandsSeparator( element.attribute( "thousandsSeparator" ) );
    if ( element.hasAttribute( "currencySymbol" ) )
	setCurrencySymbol( element.attribute( "currencySymbol" ) );
    if ( element.hasAttribute( "monetaryDecimalSymbol" ) )
	setMonetaryDecimalSymbol( element.attribute( "monetaryDecimalSymbol" ) );
    if ( element.hasAttribute( "monetaryThousandsSeparator" ) )
	setMonetaryThousandsSeparator( element.attribute( "monetaryThousandsSeparator" ) );
    if ( element.hasAttribute( "positiveSign" ) )
	setPositiveSign( element.attribute( "positiveSign" ) );
    if ( element.hasAttribute( "negativeSign" ) )
	setNegativeSign( element.attribute( "negativeSign" ) );
    if ( element.hasAttribute( "fracDigits" ) )
	setFracDigits( element.attribute( "fracDigits" ).toInt() );
    if ( element.hasAttribute( "positivePrefixCurrencySymbol" ) )
    {
	QString c = element.attribute( "positivePrefixCurrencySymbol" );
        setPositivePrefixCurrencySymbol( c == "True" );
    }
    if ( element.hasAttribute( "negativePrefixCurrencySymbol" ) )
    {
	QString c = element.attribute( "negativePrefixCurrencySymbol" );
	setNegativePrefixCurrencySymbol( c == "True" );
    }
    if ( element.hasAttribute( "positiveMonetarySignPosition" ) )
	setPositiveMonetarySignPosition( (SignPosition)element.attribute( "positiveMonetarySignPosition" ).toInt() );
    if ( element.hasAttribute( "negativeMonetarySignPosition" ) )
	setNegativeMonetarySignPosition( (SignPosition)element.attribute( "negativeMonetarySignPosition" ).toInt() );
    if ( element.hasAttribute( "timeFormat" ) )
	setTimeFormat( element.attribute( "timeFormat" ) );
    if ( element.hasAttribute( "dateFormat" ) )
	setDateFormat( element.attribute( "dateFormat" ) );
    if ( element.hasAttribute( "dateFormatShort" ) )
	setDateFormatShort( element.attribute( "dateFormatShort" ) );
}

QDomElement Locale::save( QDomDocument& doc ) const
{
    QDomElement element = doc.createElement( "locale" );

    element.setAttribute( "weekStartsMonday", (weekStartDay() == 1) ? "True" : "False" );
    element.setAttribute( "decimalSymbol", decimalSymbol() );
    element.setAttribute( "thousandsSeparator", thousandsSeparator() );
    element.setAttribute( "currencySymbol", currencySymbol() );
    element.setAttribute( "monetaryDecimalSymbol", monetaryDecimalSymbol() );
    element.setAttribute( "monetaryThousandsSeparator", monetaryThousandsSeparator() );
    element.setAttribute( "positiveSign", positiveSign() );
    element.setAttribute( "negativeSign", negativeSign() );
    element.setAttribute( "fracDigits", fracDigits() );
    element.setAttribute( "positivePrefixCurrencySymbol", positivePrefixCurrencySymbol() ? "True" : "False");
    element.setAttribute( "negativePrefixCurrencySymbol", negativePrefixCurrencySymbol() ? "True" : "False");
    element.setAttribute( "positiveMonetarySignPosition", (int)positiveMonetarySignPosition() );
    element.setAttribute( "negativeMonetarySignPosition", (int)negativeMonetarySignPosition() );
    element.setAttribute( "timeFormat", timeFormat() );
    element.setAttribute( "dateFormat", dateFormat() );
    element.setAttribute( "dateFormatShort", dateFormatShort() );

    return element;
}

void Locale::defaultSystemConfig( )
{
    KLocale locale("kspread");
    setWeekStartDay( locale.weekStartDay() );
    setDecimalSymbol( locale.decimalSymbol());
    setThousandsSeparator( locale.thousandsSeparator() );
    setCurrencySymbol( locale.currencySymbol() );
    setMonetaryDecimalSymbol( locale.monetaryDecimalSymbol() );
    setMonetaryThousandsSeparator( locale.monetaryThousandsSeparator());
    setPositiveSign( locale.positiveSign() );
    setNegativeSign( locale.negativeSign() );
    setFracDigits( locale.fracDigits() );
    setPositivePrefixCurrencySymbol( locale.positivePrefixCurrencySymbol() );
    setNegativePrefixCurrencySymbol( locale.negativePrefixCurrencySymbol() );
    setPositiveMonetarySignPosition( locale.positiveMonetarySignPosition() );
    setNegativeMonetarySignPosition( locale.negativeMonetarySignPosition() );
    setTimeFormat( locale.timeFormat() );
    setDateFormat( locale.dateFormat() );
    setDateFormatShort( locale.dateFormatShort() );

}

