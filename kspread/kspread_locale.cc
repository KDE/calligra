#include "kspread_locale.h"

#include <qdom.h>

KSpreadLocale::KSpreadLocale()
	: KLocale("kspread")
{
	insertCatalogue("koffice");
}

void KSpreadLocale::load( const QDomElement& element )
{
    if ( element.hasAttribute( "weekStartsMonday" ) )
    {
	QString c = element.attribute( "weekStartsMonday" );
	if ( c == "False" )
	    m_weekStartsMonday = FALSE;
	else
	    m_weekStartsMonday = TRUE;
    }
    if ( element.hasAttribute( "decimalSymbol" ) )
	_decimalSymbol = element.attribute( "decimalSymbol" );
    if ( element.hasAttribute( "thousandsSeparator" ) )
	_thousandsSeparator = element.attribute( "thousandsSeparator" );
    if ( element.hasAttribute( "currencySymbol" ) )
	_currencySymbol = element.attribute( "currencySymbol" );
    if ( element.hasAttribute( "monetaryDecimalSymbol" ) )
	_monetaryDecimalSymbol = element.attribute( "monetaryDecimalSymbol" );
    if ( element.hasAttribute( "monetaryThousandsSeparator" ) )
	_monetaryThousandsSeparator = element.attribute( "monetaryThousandsSeparator" );
    if ( element.hasAttribute( "positiveSign" ) )
	_positiveSign = element.attribute( "positiveSign" );
    if ( element.hasAttribute( "negativeSign" ) )
	_negativeSign = element.attribute( "negativeSign" );
    if ( element.hasAttribute( "fracDigits" ) )
	_fracDigits = element.attribute( "fracDigits" ).toInt();
    if ( element.hasAttribute( "positivePrefixCurrencySymbol" ) )
    {
	QString c = element.attribute( "positivePrefixCurrencySymbol" );
	if ( c == "True" )
	    _positivePrefixCurrencySymbol = TRUE;
	else if ( c == "False" )
	    _positivePrefixCurrencySymbol = FALSE;
    }
    if ( element.hasAttribute( "negativePrefixCurrencySymbol" ) )
    {
	QString c = element.attribute( "negativePrefixCurrencySymbol" );
	if ( c == "True" )
	    _negativePrefixCurrencySymbol = TRUE;
	else if ( c == "False" )
	    _negativePrefixCurrencySymbol = FALSE;
    }
    if ( element.hasAttribute( "positiveMonetarySignPosition" ) )
	_positiveMonetarySignPosition = (SignPosition)element.attribute( "positiveMonetarySignPosition" ).toInt();
    if ( element.hasAttribute( "negativeMonetarySignPosition" ) )
	_negativeMonetarySignPosition = (SignPosition)element.attribute( "negativeMonetarySignPosition" ).toInt();
    if ( element.hasAttribute( "timeFormat" ) )
	_timefmt = element.attribute( "timeFormat" );
    if ( element.hasAttribute( "dateFormat" ) )
	_datefmt = element.attribute( "dateFormat" );
    if ( element.hasAttribute( "dateFormatShort" ) )
	_datefmtshort = element.attribute( "dateFormatShort" );
}

QDomElement KSpreadLocale::save( QDomDocument& doc ) const
{
    QDomElement element = doc.createElement( "locale" );

    if( m_weekStartsMonday )
	element.setAttribute( "weekStartsMonday", "True" );
    else
	element.setAttribute( "weekStartsMonday", "False" );
    element.setAttribute( "decimalSymbol", _decimalSymbol );
    element.setAttribute( "thousandsSeparator", _thousandsSeparator );
    element.setAttribute( "currencySymbol", _currencySymbol );
    element.setAttribute( "monetaryDecimalSymbol", _monetaryDecimalSymbol );
    element.setAttribute( "monetaryThousandsSeparator", _monetaryThousandsSeparator );
    element.setAttribute( "positiveSign", _positiveSign );
    element.setAttribute( "negativeSign", _negativeSign );
    element.setAttribute( "fracDigits", _fracDigits );
    if ( _positivePrefixCurrencySymbol )
	element.setAttribute( "positivePrefixCurrencySymbol", "True" );
    else
	element.setAttribute( "positivePrefixCurrencySymbol", "False" );
    if ( _negativePrefixCurrencySymbol )
	element.setAttribute( "negativePrefixCurrencySymbol", "True" );
    else
	element.setAttribute( "negativePrefixCurrencySymbol", "False" );
    element.setAttribute( "positiveMonetarySignPosition", (int)_positiveMonetarySignPosition );
    element.setAttribute( "negativeMonetarySignPosition", (int)_negativeMonetarySignPosition );
    element.setAttribute( "timeFormat", _timefmt );
    element.setAttribute( "dateFormat", _datefmt );
    element.setAttribute( "dateFormatShort", _datefmtshort );

    return element;
}
