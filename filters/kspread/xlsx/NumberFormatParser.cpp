/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Christoph Schleifenbaum christoph@kdab.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */
#include "NumberFormatParser.h"

#include <KoGenStyle.h>
#include <KoGenStyles.h>
#include <KoXmlWriter.h>

#include <QBuffer>
#include <QDebug>
#include <QLocale>
#include <QString>
#include <QStringList>

KoGenStyles* NumberFormatParser::styles = 0;
 

#define DEFINELOCALE( ID, CODE ) case ID: return QLocale( QLatin1String( CODE ) );

QLocale NumberFormatParser::locale( int langid )
{
    switch( langid && 0xffff )
    {
DEFINELOCALE( 0x0436, "af-ZA" ) //  Afrikaans   South Africa
DEFINELOCALE( 0x041c, "sq-AL" ) //  Albanian    Albania
DEFINELOCALE( 0x0484, "gsw-FR" ) //  Alsatian    France
DEFINELOCALE( 0x045e, "am-ET" ) //   Amharic     Ethiopia
DEFINELOCALE( 0x0401, "ar-SA" ) //   Arabic  Saudi Arabia
DEFINELOCALE( 0x0801, "ar-IQ" ) //   Arabic  Iraq
DEFINELOCALE( 0x0c01, "ar-EG" ) //   Arabic  Egypt
DEFINELOCALE( 0x1001, "ar-LY" ) //   Arabic  Libya
DEFINELOCALE( 0x1401, "ar-DZ" ) //   Arabic  Algeria
DEFINELOCALE( 0x1801, "ar-MA" ) //   Arabic  Morocco
DEFINELOCALE( 0x1c01, "ar-TN" ) //   Arabic  Tunisia
DEFINELOCALE( 0x2001, "ar-OM" ) //   Arabic  Oman
DEFINELOCALE( 0x2401, "ar-YE" ) //   Arabic  Yemen
DEFINELOCALE( 0x2801, "ar-SY" ) //   Arabic  Syria
DEFINELOCALE( 0x2c01, "ar-JO" ) //   Arabic  Jordan
DEFINELOCALE( 0x3001, "ar-LB" ) //   Arabic  Lebanon
DEFINELOCALE( 0x3401, "ar-KW" ) //   Arabic  Kuwait
DEFINELOCALE( 0x3801, "ar-AE" ) //   Arabic  U.A.E.
DEFINELOCALE( 0x3c01, "ar-BH" ) //   Arabic  Bahrain
DEFINELOCALE( 0x4001, "ar-QA" ) //   Arabic  Qatar
DEFINELOCALE( 0x042b, "hy-AM" ) //   Armenian    Armenia
DEFINELOCALE( 0x044d, "as-IN" ) //   Assamese    India
DEFINELOCALE( 0x082c, "az-Cyrl-AZ" ) //  Azeri (Cyrillic)    Azerbaijan
DEFINELOCALE( 0x042c, "az-Latn-AZ" ) //  Azeri (Latin)   Azerbaijan
DEFINELOCALE( 0x046d, "ba-RU" ) //   Bashkir     Russia
DEFINELOCALE( 0x042d, "eu-ES" ) //   Basque  Basque
DEFINELOCALE( 0x0423, "be-BY" ) //   Belarusian  Belarus
DEFINELOCALE( 0x0445, "bn-IN" ) //   Bengali     India
DEFINELOCALE( 0x0845, "bn-BD" ) //   Bengali     Bangladesh
DEFINELOCALE( 0x201a, "bs-Cyrl-BA" ) //  Bosnian (Cyrillic)  Bosnia and Herzegovina
DEFINELOCALE( 0x141a, "bs-Latn-BA" ) //  Bosnian (Latin)     Bosnia and Herzegovina
DEFINELOCALE( 0x047e, "br-FR" ) //   Breton  France
DEFINELOCALE( 0x0402, "bg-BG" ) //   Bulgarian   Bulgaria
DEFINELOCALE( 0x0403, "ca-ES" ) //   Catalan     Catalan
DEFINELOCALE( 0x0404, "zh-TW" ) //   Chinese     Taiwan
DEFINELOCALE( 0x0804, "zh-CN" ) //   Chinese     PRC
DEFINELOCALE( 0x0c04, "zh-HK" ) //   Chinese     Hong Kong SAR
DEFINELOCALE( 0x1004, "zh-SG" ) //   Chinese     Singapore
DEFINELOCALE( 0x1404, "zh-MO" ) //   Chinese     Macao SAR
DEFINELOCALE( 0x0483, "co-FR" ) //   Corsican    France
DEFINELOCALE( 0x041a, "hr-HR" ) //   Croatian    Croatia
DEFINELOCALE( 0x101a, "hr-BA" ) //   Croatian (Latin)    Bosnia and Herzegovina
DEFINELOCALE( 0x0405, "cs-CZ" ) //   Czech   Czech Republic
DEFINELOCALE( 0x0406, "da-DK" ) //   Danish  Denmark
DEFINELOCALE( 0x048c, "prs-AF" ) //  Dari    Afghanistan
DEFINELOCALE( 0x0465, "dv-MV" ) //   Divehi  Maldives
DEFINELOCALE( 0x0813, "nl-BE" ) //   Dutch   Belgium
DEFINELOCALE( 0x0413, "nl-NL" ) //   Dutch   Netherlands
DEFINELOCALE( 0x1009, "en-CA" ) //   English     Canada
DEFINELOCALE( 0x2009, "en-JM" ) //   English     Jamaica
DEFINELOCALE( 0x2409, "en-029" ) //  English     Caribbean
DEFINELOCALE( 0x2809, "en-BZ" ) //   English     Belize
DEFINELOCALE( 0x2c09, "en-TT" ) //   English     Trinidad
DEFINELOCALE( 0x0809, "en-GB" ) //   English     United Kingdom
DEFINELOCALE( 0x1809, "en-IE" ) //   English     Ireland
DEFINELOCALE( 0x4009, "en-IN" ) //   English     India
DEFINELOCALE( 0x1c09, "en-ZA" ) //   English     South Africa
DEFINELOCALE( 0x3009, "en-ZW" ) //   English     Zimbabwe
DEFINELOCALE( 0x0c09, "en-AU" ) //   English     Australia
DEFINELOCALE( 0x1409, "en-NZ" ) //   English     New Zealand
DEFINELOCALE( 0x3409, "en-PH" ) //   English     Philippines
DEFINELOCALE( 0x0409, "en-US" ) //   English     United States
DEFINELOCALE( 0x4409, "en-MY" ) //   English     Malaysia
DEFINELOCALE( 0x4809, "en-SG" ) //   English     Singapore
DEFINELOCALE( 0x0425, "et-EE" ) //   Estonian    Estonia
DEFINELOCALE( 0x0438, "fo-FO" ) //   Faroese     Faroe Islands
DEFINELOCALE( 0x0464, "fil-PH" ) //  Filipino    Philippines
DEFINELOCALE( 0x040b, "fi-FI" ) //   Finnish     Finland
DEFINELOCALE( 0x0c0c, "fr-CA" ) //   French  Canada
DEFINELOCALE( 0x040c, "fr-FR" ) //   French  France
DEFINELOCALE( 0x180c, "fr-MC" ) //   French  Monaco
DEFINELOCALE( 0x100c, "fr-CH" ) //   French  Switzerland
DEFINELOCALE( 0x080c, "fr-BE" ) //   French  Belgium
DEFINELOCALE( 0x140c, "fr-LU" ) //   French  Luxembourg
DEFINELOCALE( 0x0462, "fy-NL" ) //   Frisian     Netherlands
DEFINELOCALE( 0x0456, "gl-ES" ) //   Galician    Galician
DEFINELOCALE( 0x0437, "ka-GE" ) //   Georgian    Georgia
DEFINELOCALE( 0x0407, "de-DE" ) //   German  Germany
DEFINELOCALE( 0x0807, "de-CH" ) //   German  Switzerland
DEFINELOCALE( 0x0c07, "de-AT" ) //   German  Austria
DEFINELOCALE( 0x1407, "de-LI" ) //   German  Liechtenstein
DEFINELOCALE( 0x1007, "de-LU" ) //   German  Luxembourg
DEFINELOCALE( 0x0408, "el-GR" ) //   Greek   Greece
DEFINELOCALE( 0x046f, "kl-GL" ) //   Greenlandic     Greenland
DEFINELOCALE( 0x0447, "gu-IN" ) //   Gujarati    India
DEFINELOCALE( 0x0468, "ha-Latn-NG" ) //  Hausa   Nigeria
DEFINELOCALE( 0x040d, "he-IL" ) //   Hebrew  Israel
DEFINELOCALE( 0x0439, "hi-IN" ) //   Hindi   India
DEFINELOCALE( 0x040e, "hu-HU" ) //   Hungarian   Hungary
DEFINELOCALE( 0x040f, "is-IS" ) //   Icelandic   Iceland
DEFINELOCALE( 0x0470, "ig-NG" ) //   Igbo    Nigeria
DEFINELOCALE( 0x0421, "id-ID" ) //   Indonesian  Indonesia
DEFINELOCALE( 0x045d, "iu-Cans-CA" ) //  Inukitut (Syllabics)    Canada
DEFINELOCALE( 0x085d, "iu-Latn-CA" ) //  Inuktitut (Latin)   Canada
DEFINELOCALE( 0x083c, "ga-IE" ) //   Irish   Ireland
DEFINELOCALE( 0x0434, "xh-ZA" ) //   isiXhosa    South Africa
DEFINELOCALE( 0x0435, "zu-ZA" ) //   isiZulu     South Africa
DEFINELOCALE( 0x0410, "it-IT" ) //   Italian     Italy
DEFINELOCALE( 0x0810, "it-CH" ) //   Italian     Switzerland
DEFINELOCALE( 0x0411, "ja-JP" ) //   Japanese    Japan
DEFINELOCALE( 0x044b, "kn-IN" ) //   Kannada     India
DEFINELOCALE( 0x043f, "kk-KZ" ) //   Kazakh  Kazakhstan
DEFINELOCALE( 0x0453, "km-KH" ) //   Khmer   Cambodia
DEFINELOCALE( 0x0486, "qut-GT" ) //  K'iche  Guatemala
DEFINELOCALE( 0x0487, "rw-RW" ) //   Kinyarwanda     Rwanda
DEFINELOCALE( 0x0441, "sw-KE" ) //   Kiswahili   Kenya
DEFINELOCALE( 0x0457, "kok-IN" ) //  Konkani     India
DEFINELOCALE( 0x0412, "ko-KR" ) //   Korean  Korea
DEFINELOCALE( 0x0440, "ky-KG" ) //   Kyrgyz  Kyrgyzistan
DEFINELOCALE( 0x0454, "lo-LA" ) //   Lao     Lao P.D.R.
DEFINELOCALE( 0x0426, "lv-LV" ) //   Latvian     Latvia
DEFINELOCALE( 0x0427, "lt-LT" ) //   Lithuanian  Lithuania
DEFINELOCALE( 0x082e, "dsb-DE" ) //  Lower Sorbian   Germany
DEFINELOCALE( 0x046e, "lb-LU" ) //   Luxembourgish   Luxembourg
DEFINELOCALE( 0x042f, "mk-MK" ) //   Macedonian (FYROM)  Macedonia (FYROM)
DEFINELOCALE( 0x043e, "ms-MY" ) //   Malay   Malaysia
DEFINELOCALE( 0x083e, "ms-BN" ) //   Malay   Brunei Darussalam
DEFINELOCALE( 0x044c, "ml-IN" ) //   Malayalam   India
DEFINELOCALE( 0x043a, "mt-MT" ) //   Maltese     Malta
DEFINELOCALE( 0x0481, "mi-NZ" ) //   Maori   New Zealand
DEFINELOCALE( 0x047a, "arn-CL" ) //  Mapudungun  Chile
DEFINELOCALE( 0x044e, "mr-IN" ) //   Marathi     India
DEFINELOCALE( 0x047c, "moh-CA" ) //  Mohawk  Mohawk
DEFINELOCALE( 0x0450, "mn-MN" ) //   Mongolian (Cyrillic)    Mongolia
DEFINELOCALE( 0x0850, "mn-Mong-CN" ) //  Mongolian (Mongolian)   PRC
DEFINELOCALE( 0x0461, "ne-NP" ) //   Nepali  Nepal
DEFINELOCALE( 0x0414, "nb-NO" ) //   Norwegian (Bokmål)  Norway
DEFINELOCALE( 0x0814, "nn-NO" ) //   Norwegian (Nynorsk)     Norway
DEFINELOCALE( 0x0482, "oc-FR" ) //   Occitan     France
DEFINELOCALE( 0x0448, "or-IN" ) //   Oriya   India
DEFINELOCALE( 0x0463, "ps-AF" ) //   Pashto  Afghanistan
DEFINELOCALE( 0x0429, "fa-IR" ) //   Persian     Iran
DEFINELOCALE( 0x0415, "pl-PL" ) //   Polish  Poland
DEFINELOCALE( 0x0416, "pt-BR" ) //   Portuguese  Brazil
DEFINELOCALE( 0x0816, "pt-PT" ) //   Portuguese  Portugal
DEFINELOCALE( 0x0446, "pa-IN" ) //   Punjabi (Gurmukhi)  India
DEFINELOCALE( 0x046b, "quz-BO" ) //  Quechua     Bolivia
DEFINELOCALE( 0x086b, "quz-EC" ) //  Quechua     Ecuador
DEFINELOCALE( 0x0c6b, "quz-PE" ) //  Quechua     Peru
DEFINELOCALE( 0x0418, "ro-RO" ) //   Romanian    Romania
DEFINELOCALE( 0x0417, "rm-CH" ) //   Romansh     Switzerland
DEFINELOCALE( 0x0419, "ru-RU" ) //   Russian     Russia
DEFINELOCALE( 0x243b, "smn-FI" ) //  Sami, Inari     Finland
DEFINELOCALE( 0x143b, "smj-SE" ) //  Sami, Lule  Sweden
DEFINELOCALE( 0x103b, "smj-NO" ) //  Sami, Lule  Norway
DEFINELOCALE( 0x043b, "se-NO" ) //   Sami, Northern  Norway
DEFINELOCALE( 0x083b, "se-SE" ) //   Sami, Northern  Sweden
DEFINELOCALE( 0x0c3b, "se-FI" ) //   Sami, Northern  Finland
DEFINELOCALE( 0x203b, "sms-FI" ) //  Sami, Skolt     Finland
DEFINELOCALE( 0x183b, "sma-NO" ) //  Sami, Southern  Norway
DEFINELOCALE( 0x1c3b, "sma-SE" ) //  Sami, Southern  Sweden
DEFINELOCALE( 0x044f, "sa-IN" ) //   Sanskrit    India
DEFINELOCALE( 0x0c1a, "sr-Cyrl-CS" ) //  Serbian (Cyrillic)  Serbia
DEFINELOCALE( 0x1c1a, "sr-Cyrl-BA" ) //  Serbian (Cyrillic)  Bosnia and Herzegovina
DEFINELOCALE( 0x081a, "sr-Latn-CS" ) //  Serbian (Latin)     Serbia
DEFINELOCALE( 0x181a, "sr-Latn-BA" ) //  Serbian (Latin)     Bosnia and Herzegovina
DEFINELOCALE( 0x046c, "nso-ZA" ) //  Sesotho sa Leboa    South Africa
DEFINELOCALE( 0x0432, "tn-ZA" ) //   Setswana    South Africa
DEFINELOCALE( 0x045b, "si-LK" ) //   Sinhala     Sri Lanka
DEFINELOCALE( 0x041b, "sk-SK" ) //   Slovak  Slovakia
DEFINELOCALE( 0x0424, "sl-SI" ) //   Slovenian   Slovenia
DEFINELOCALE( 0x080a, "es-MX" ) //   Spanish     Mexico
DEFINELOCALE( 0x100a, "es-GT" ) //   Spanish     Guatemala
DEFINELOCALE( 0x140a, "es-CR" ) //   Spanish     Costa Rica
DEFINELOCALE( 0x180a, "es-PA" ) //   Spanish     Panama
DEFINELOCALE( 0x1c0a, "es-DO" ) //   Spanish     Dominican Republic
DEFINELOCALE( 0x200a, "es-VE" ) //   Spanish     Venezuela
DEFINELOCALE( 0x240a, "es-CO" ) //   Spanish     Colombia
DEFINELOCALE( 0x280a, "es-PE" ) //   Spanish     Peru
DEFINELOCALE( 0x2c0a, "es-AR" ) //   Spanish     Argentina
DEFINELOCALE( 0x300a, "es-EC" ) //   Spanish     Ecuador
DEFINELOCALE( 0x340a, "es-CL" ) //   Spanish     Chile
DEFINELOCALE( 0x3c0a, "es-PY" ) //   Spanish     Paraguay
DEFINELOCALE( 0x400a, "es-BO" ) //   Spanish     Bolivia
DEFINELOCALE( 0x440a, "es-SV" ) //   Spanish     El Salvador
DEFINELOCALE( 0x480a, "es-HN" ) //   Spanish     Honduras
DEFINELOCALE( 0x4c0a, "es-NI" ) //   Spanish     Nicaragua
DEFINELOCALE( 0x500a, "es-PR" ) //   Spanish     Puerto Rico
DEFINELOCALE( 0x540a, "es-US" ) //   Spanish     United States
DEFINELOCALE( 0x380a, "es-UY" ) //   Spanish     Uruguay
DEFINELOCALE( 0x0c0a, "es-ES" ) //   Spanish (International Sort)    Spain
DEFINELOCALE( 0x040a, "es-ES_tradnl" ) //    Spanish (Traditional Sort)  Spain
DEFINELOCALE( 0x041d, "sv-SE" ) //   Swedish     Sweden
DEFINELOCALE( 0x081d, "sv-FI" ) //   Swedish     Finland
DEFINELOCALE( 0x045a, "syr-SY" ) //  Syriac  Syria
DEFINELOCALE( 0x0428, "tg-Cyrl-TJ" ) //  Tajik   Tajikistan
DEFINELOCALE( 0x085f, "tzm-Latn-DZ" ) //     Tamazight (Latin)   Algeria
DEFINELOCALE( 0x0449, "ta-IN" ) //   Tamil   India
DEFINELOCALE( 0x0444, "tt-RU" ) //   Tatar   Russia
DEFINELOCALE( 0x044a, "te-IN" ) //   Telugu  India
DEFINELOCALE( 0x041e, "th-TH" ) //   Thai    Thailand
DEFINELOCALE( 0x0451, "bo-CN" ) //   Tibetan     PRC
DEFINELOCALE( 0x041f, "tr-TR" ) //   Turkish     Turkey
DEFINELOCALE( 0x0442, "tk-TM" ) //   Turkmen     Turkmenistan
DEFINELOCALE( 0x0480, "ug-CN" ) //   Uighur  PRC
DEFINELOCALE( 0x0422, "uk-UA" ) //   Ukrainian   Ukraine
DEFINELOCALE( 0x042e, "wen-DE" ) //  Upper Sorbian   Germany
DEFINELOCALE( 0x0420, "ur-PK" ) //   Urdu    Pakistan
DEFINELOCALE( 0x0843, "uz-Cyrl-UZ" ) //  Uzbek (Cyrillic)    Uzbekistan
DEFINELOCALE( 0x0443, "uz-Latn-UZ" ) //  Uzbek (Latin)   Uzbekistan
DEFINELOCALE( 0x042a, "vi-VN" ) //   Vietnamese  Vietnam
DEFINELOCALE( 0x0452, "cy-GB" ) //   Welsh   United Kingdom
DEFINELOCALE( 0x0488, "wo-SN" ) //   Wolof   Senegal
DEFINELOCALE( 0x0485, "sah-RU" ) //  Yakut   Russia
DEFINELOCALE( 0x0478, "ii-CN" ) //   Yi  PRC
DEFINELOCALE( 0x046a, "yo-NG" ) //   Yoruba  Nigeria

    }
    return QLocale(); 
}
    
void NumberFormatParser::setStyles( KoGenStyles* styles )
{
    NumberFormatParser::styles = styles;
}

#define SET_TYPE_OR_RETURN( TYPE ) { \
if( type == KoGenStyle::StyleNumericDate && TYPE == KoGenStyle::StyleNumericTime )               \
{                                                                                                \
}                                                                                                \
else if( type == KoGenStyle::StyleNumericTime && TYPE == KoGenStyle::StyleNumericDate )          \
{                                                                                                \
    type = TYPE;                                                                                 \
}                                                                                                \
else if( type == KoGenStyle::StyleNumericPercentage && TYPE == KoGenStyle::StyleNumericNumber )  \
{                                                                                                \
}                                                                                                \
else if( type == KoGenStyle::StyleNumericNumber && TYPE == KoGenStyle::StyleNumericPercentage )  \
{                                                                                                \
    type = TYPE;                                                                                 \
}                                                                                                \
else if( type == KoGenStyle::StyleNumericCurrency && TYPE == KoGenStyle::StyleNumericNumber )    \
{                                                                                                \
}                                                                                                \
else if( type == KoGenStyle::StyleNumericNumber && TYPE == KoGenStyle::StyleNumericCurrency )    \
{                                                                                                \
    type = TYPE;                                                                                 \
}                                                                                                \
else if( type == KoGenStyle::StyleNumericFraction && TYPE == KoGenStyle::StyleNumericNumber )    \
{                                                                                                \
}                                                                                                \
else if( type == KoGenStyle::StyleNumericNumber && TYPE == KoGenStyle::StyleNumericFraction )    \
{                                                                                                \
    type = TYPE;                                                                                 \
}                                                                                                \
else if( type == KoGenStyle::StyleNumericScientific && TYPE == KoGenStyle::StyleNumericNumber )  \
{                                                                                                \
}                                                                                                \
else if( type == KoGenStyle::StyleNumericNumber && TYPE == KoGenStyle::StyleNumericScientific )  \
{                                                                                                \
    type = TYPE;                                                                                 \
}                                                                                                \
else if( type != KoGenStyle::StyleAuto && type != TYPE )                                         \
{                                                                                                \
    return KoGenStyle( KoGenStyle::StyleAuto );                                                  \
}                                                                                                \
else                                                                                             \
{                                                                                                \
    type = TYPE;                                                                                 \
}                                                                                                \
}

#define FINISH_PLAIN_TEXT_PART {             \
if( !plainText.isEmpty() )                   \
{                                            \
    hadPlainText = true;                     \
    xmlWriter.startElement( "number:text" ); \
    xmlWriter.addTextNode( plainText );      \
    xmlWriter.endElement();                  \
    plainText.clear();                       \
}                                            \
}

static KoGenStyle styleFromTypeAndBuffer( KoGenStyle::Type type, const QBuffer& buffer )
{
    KoGenStyle result( type );
    
    const QString elementContents = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
    result.addChildElement( "number", elementContents );

    return result;
}

KoGenStyle NumberFormatParser::parse( const QString& numberFormat )
{
    QBuffer buffer;
    buffer.open( QIODevice::WriteOnly );
    KoXmlWriter xmlWriter( &buffer );

    KoGenStyle::Type type = KoGenStyle::StyleAuto;

    QString plainText;

    QMap< QString, QString > conditions;

    QString condition;

    // this is for the month vs. minutes-context
    bool justHadHours = false;

    bool hadPlainText = false;

    for( int i = 0; i < numberFormat.length(); ++i )
    {
        const char c = numberFormat[ i ].toLatin1();
                
        const bool isLong = numberFormat[ i + 1 ] == c                         && i < numberFormat.length() - 1;
        const bool isLonger = isLong && numberFormat[ i + 2 ] == c             && i < numberFormat.length() - 2;
        const bool isLongest = isLonger && numberFormat[ i + 3 ] == c          && i < numberFormat.length() - 3;
        const bool isWayTooLong = isWayTooLong && numberFormat[ i + 4 ] == c   && i < numberFormat.length() - 4;


        switch( c )
        {
        // condition or color or locale...
        case '[':
            {
                const char ch = i < numberFormat.length() - 1 ? numberFormat[ ++i ].toLatin1() : ']';
                if( ( ch >= 'a' && ch <= 'z' ) || ( ch >= 'A' && ch <= 'Z' ) )
                {
                    // color code
                    QString colorName;
                    while( i < numberFormat.length() && numberFormat[ i ] != QLatin1Char( ']' ) )
                        colorName += numberFormat[ i++ ];
                    
                }
                else if( ch == '$' && i < numberFormat.length() - 2 && numberFormat[ i + 1 ].toLatin1() != '-' )
                {
                    SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericCurrency );
                    ++i;
                    // currency code
                    QString currency;
                    while( i < numberFormat.length() && numberFormat[ i ] != QLatin1Char( ']' ) && numberFormat[ i ] != QLatin1Char( '-' ) )
                        currency += numberFormat[ i++ ];
                   
                    QString language;
                    QString country;

                    if( numberFormat[ i ] == QLatin1Char( '-' ) )
                    {
                        ++i;
                        QString localeId;
                        while( i < numberFormat.length() && numberFormat[ i ] != QLatin1Char( ']' ) )
                            localeId += numberFormat[ i++ ];
                        const QLocale locale = NumberFormatParser::locale( localeId.toInt( 0, 16 ) );
                        language = locale.name();
                        language = language.left( language.indexOf( QLatin1String( "_" ) ) );
                        country = locale.name();
                        country = country.mid( country.indexOf( QLatin1String( "_" ) ) + 1 );
                    }
 
                    xmlWriter.startElement( "number:currency-symbol" );
                    if( !language.isEmpty() )
                        xmlWriter.addAttribute( "number:language", language );
                    if( !country.isEmpty() )
                        xmlWriter.addAttribute( "number:country", country );

                    xmlWriter.addTextSpan( currency );
                    xmlWriter.endElement();
                }
                else
                {
                    // unknown - no idea, skip
                    while( i < numberFormat.length() && numberFormat[ i ] != QLatin1Char( ']' ) )
                        ++i;
                }
            }
            break;


        // underscore: ignore the next char
        case '_':
            plainText += QLatin1Char( ' ' );
            ++i;
            break;


        // asterisk: ignore
        case '*':
            ++i;
            break;

        // percentage
        case '%':
            SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericPercentage );
            FINISH_PLAIN_TEXT_PART
            xmlWriter.startElement( "number:text" );
            xmlWriter.addTextNode( "%" );
            xmlWriter.endElement();
            break;

        // a number
        case '.':
        case ',':
        case '#':
        case '0':
        case '?':
            SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericNumber )
            FINISH_PLAIN_TEXT_PART
            {
                bool grouping = false;
                bool gotDot = false;
                bool gotE = false;
                int decimalPlaces = 0;
                int integerDigits = 0;
                int exponentDigits = 0;

                char ch = numberFormat[ i ].toLatin1();
                do
                {
                    if( ch == '.' )
                        gotDot = true;
                    else if( ch == ',' )
                        grouping = true;
                    else if( ch == 'E' || ch == 'e' )
                    {
                        SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericScientific );
                        const char chN = numberFormat[ i + 1 ].toLatin1();
                        if( chN == '-' || chN == '+' )
                        {
                            gotE = true;
                            ++i;
                        }
                    }
                    else if( ch == '0' && gotE )
                        ++exponentDigits;
                    else if( ch == '0' && !gotDot )
                        ++integerDigits;
                    else if( ch == '0' && gotDot )
                        ++decimalPlaces;
                    else if( ch == '?' )
                    { /* ignore */ }
                    else if( ch == '/' )
                    {
                        SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericFraction );
                        if( gotDot || grouping )
                            return KoGenStyle();
                    }

                    ch = numberFormat[ ++i ].toLatin1();
                }
                while( i < numberFormat.length() && ( ch == '.' || ch == ',' || ch == '#' || ch == '0' || ch == 'E' || ch == 'e' || ch == '?' ) );
               
                if( !( ch == '.' || ch == ',' || ch == '#' || ch == '0' || ch == 'E' || ch == 'e' || ch == '?' ) )
                    --i;

                if( exponentDigits > 0 )
                    xmlWriter.startElement( "number:scientific-number" );
                else
                    xmlWriter.startElement( "number:number" );
                if( gotDot )
                    xmlWriter.addAttribute( "number:decimal-places", decimalPlaces );
                xmlWriter.addAttribute( "number:min-integer-digits", integerDigits );
                if( exponentDigits > 0 )
                    xmlWriter.addAttribute( "number:min-exponent-digits", exponentDigits );
                if( grouping )
                    xmlWriter.addAttribute( "number:grouping", grouping ? "true" : "false" );
                xmlWriter.endElement();
            }
            break;


        // Everything related to date/time
        // AM/PM
        case 'A':
        case 'a':
            if( numberFormat.mid( i, 5 ).toLower() == QLatin1String( "am/pm" ) ||
                numberFormat.mid( i, 3 ).toLower() == QLatin1String( "a/p" ) )
            {
                SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericTime )
                FINISH_PLAIN_TEXT_PART;
                xmlWriter.startElement( "number:am-pm" );
                xmlWriter.endElement();
                if( numberFormat.mid( i, 5 ).toLower() == QLatin1String( "am/pm" ) )
                    i += 2;
                i += 2;
            }
            break;

        
        // hours, long or short
        case 'H':
        case 'h':
            SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericTime )
            FINISH_PLAIN_TEXT_PART;
            xmlWriter.startElement( "number:hours" );
            if( isLong )
            {
                xmlWriter.addAttribute( "number:style", "long" );
                ++i;
            }
            xmlWriter.endElement();
            break;


        // minutes or months, depending on context
        case 'M':
        case 'm':
            // must be month, then, at least three M
            if( isLonger )
            {
                SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericDate )
                FINISH_PLAIN_TEXT_PART;
                xmlWriter.startElement( "number:month" );
                if( isLongest )
                    xmlWriter.addAttribute( "number:style", "long" );
                xmlWriter.addAttribute( "number:textual", "true" );
                xmlWriter.endElement();
                i += isLongest ? ( isWayTooLong ? 4 : 3 ) : 2;
            }
            // depends on the context. After hours and before seconds, it's minutes
            // otherwise it's the month
            else
            {
                if( justHadHours )
                {
                    SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericTime )
                    FINISH_PLAIN_TEXT_PART;
                    xmlWriter.startElement( "number:minutes" );
                    if( isLong )
                        xmlWriter.addAttribute( "number:style", "long" );
                    xmlWriter.endElement();
                }
                else
                {
                    // on the next iteration, we might see wheter there're seconds or something else
                    bool minutes = true; // let's just default to minutes, if there's nothing more...
                    // so let's look ahead:
                    for( int j = i + 1; j < numberFormat.length(); ++j )
                    {
                        const char ch = numberFormat[ i ].toLatin1();
                        if( ch == 's' || ch == 'S' )  // minutes
                            break;
                        if( !(ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' ) ) // months
                            continue;
                        minutes = false;
                        break;
                    }
                    if( minutes )
                        SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericTime )
                    else
                        SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericDate )
                    FINISH_PLAIN_TEXT_PART;
                    if( minutes )
                        xmlWriter.startElement( "number:minutes" );
                    else
                        xmlWriter.startElement( "number:month" );
                    if( isLong )
                        xmlWriter.addAttribute( "number:style", "long" );
                    xmlWriter.endElement();
                }
                if( isLong )
                    ++i;
            }
            break;


        // day (of week)
        case 'D':
        case 'd':
            SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericDate )
            FINISH_PLAIN_TEXT_PART;
            if( !isLonger )
            {
                xmlWriter.startElement( "number:day" );
                if( isLong )
                    xmlWriter.addAttribute( "number:style", "long" );
                xmlWriter.endElement();
            }
            else
            {
                xmlWriter.startElement( "number:day-of-week" );
                if( isLongest )
                    xmlWriter.addAttribute( "number:style", "long" );
                xmlWriter.endElement();
            }
            if( isLong )
                ++i;
            if( isLonger )
                ++i;
            if( isLongest )
                ++i;
            break;

        // seconds, long or short
        case 'S':
        case 's':
            SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericTime )
            FINISH_PLAIN_TEXT_PART;
            xmlWriter.startElement( "number:seconds" );
            if( isLong )
            {
                xmlWriter.addAttribute( "number:style", "long" );
                ++i;
            }
            xmlWriter.endElement();
            break;

        // year, long or short
        case 'Y':
        case 'y':
            SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericDate )
            FINISH_PLAIN_TEXT_PART;
            xmlWriter.startElement( "number:year" );
            if( isLongest )
            {
                xmlWriter.addAttribute( "number:style", "long" );
                i += 2;
            }
            else if( isLong )
            {
                ++i;
            }
            xmlWriter.endElement();
            break; 

        // now it's getting really scarry: semi-colon:
        case ';':
            {
                FINISH_PLAIN_TEXT_PART;
                buffer.close();

                // conditional style with the current format
                KoGenStyle result = styleFromTypeAndBuffer( type, buffer );
                result.addAttribute( "style:volatile", "true" );
                const QString styleName = NumberFormatParser::styles->lookup( result, "N" );
                qDebug() << condition;
                qDebug() << buffer.data();
                // start a new style
                buffer.setData( QByteArray() );
                buffer.open( QIODevice::WriteOnly );
                conditions.insertMulti( condition, styleName );
                condition.clear();
            }
            break;

        // text-content
        case '@':
            FINISH_PLAIN_TEXT_PART;
            xmlWriter.startElement( "number:text-content" );
            xmlWriter.endElement();
            break;

        // quote - plain text block
        case '"':
            while( i < numberFormat.length() - 1 && numberFormat[ ++i ] != QLatin1Char( '"' ) )
                plainText += numberFormat[ i ];
            break; 

        // backslash escapes the next char
        case '\\':
            if( i < numberFormat.length() - 1 )
                plainText += numberFormat[ ++i ];
            break;

        // every other char is just passed
        default:
            plainText += c;
            break;
        }
    

        // for the context-sensitive 'M'
        if( plainText.isEmpty() )
            justHadHours = ( c == 'h' || c == 'H' );
        
    }
            
    FINISH_PLAIN_TEXT_PART;

    if( type == KoGenStyle::StyleAuto && hadPlainText )
        SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericText )

    if( !condition.isEmpty() )
    {
        // conditional style with the current format
        KoGenStyle result = styleFromTypeAndBuffer( type, buffer );
        result.addAttribute( "style:volatile", "true" );
        const QString styleName = NumberFormatParser::styles->lookup( result, "N" );
        qDebug() << condition;
        qDebug() << buffer.data();
        // start a new style
        buffer.setData( QByteArray() );
        buffer.open( QIODevice::WriteOnly );
        conditions.insertMulti( condition, styleName );
        condition.clear();
    }

    // if conditions w/o explicit expressions where added, we create the expressions
    QStringList autoConditions;
    if( conditions.count( QString() ) == 1 )
    {
        autoConditions.push_back( QLatin1String( "value()>=0" ) );
    }
    else
    {
        autoConditions.push_back( QLatin1String( "value()>0" ) );
        autoConditions.push_back( QLatin1String( "value()<0" ) );
        autoConditions.push_back( QLatin1String( "value()=0" ) );
    }

    // add conditional styles:
    for( QMap< QString, QString >::const_iterator it = conditions.constBegin(); it != conditions.constEnd(); ++it )
    {
        xmlWriter.startElement( "style:map" );
        xmlWriter.addAttribute( "style:condition", it.key().isEmpty() ? autoConditions.takeLast() : it.key() );
        xmlWriter.addAttribute( "style:apply-style-name", it.value() );
        xmlWriter.endElement();
    }

    buffer.close();
    
    // conditional style with the current format
    qDebug() << buffer.data();

    return styleFromTypeAndBuffer( type, buffer );
}
