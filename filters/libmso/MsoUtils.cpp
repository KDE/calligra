/*
 * This file is part of Office 2007 Filters for Calligra
 * Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
 * Copyright (c) 2003 Lukas Tinkl <lukas@kde.org>
 * Copyright (C) 2003 David Faure <faure@kde.org>
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
 * Contact: Suresh Chande suresh.chande@nokia.com
 * Copyright (C) 2011 Matus Uzak <matus.uzak@ixonos.com>
 * Copyright 2014 Inge Wallin <inge@lysator.liu.se>
 *
 * Utils::columnName() based on Cell::columnName() from calligra/kspread/Utils.cpp:
 * Copyright 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 * Copyright 2004 Tomas Mecir <mecirt@gmail.com>
 * Copyright 1999-2002,2004 Laurent Montel <montel@kde.org>
 * Copyright 2002,2004 Ariya Hidayat <ariya@kde.org>
 * Copyright 2002-2003 Norbert Andres <nandres@web.de>
 * Copyright 2003 Stefan Hetzl <shetzl@chello.at>
 * Copyright 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
 * Copyright 2002 Harri Porten <porten@kde.org>
 * Copyright 2002 John Dailey <dailey@vt.edu>
 * Copyright 1999-2001 David Faure <faure@kde.org>
 * Copyright 2000-2001 Werner Trobin <trobin@kde.org>
 * Copyright 2000 Simon Hausmann <hausmann@kde.org
 * Copyright 1998-1999 Torben Weis <weis@kde.org>
 * Copyright 1999 Michael Reiher <michael.reiher@gmx.de>
 * Copyright 1999 Reginald Stadlbauer <reggie@kde.org>
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

// Own
#include "MsoUtils.h"

// Qt
#include <QMap>
#include <QString>
#include <QPalette>
#include <QGlobalStatic>


class DefaultIndexedColors : public QList< QColor >
{
public:
    DefaultIndexedColors()
    {
        push_back( QColor( 0, 0, 0 ) );
        push_back( QColor( 255, 255, 255 ) );
        push_back( QColor( 255, 0, 0 ) );
        push_back( QColor( 0, 255, 0 ) );
        push_back( QColor( 0, 0, 255 ) );
        push_back( QColor( 255, 255, 0 ) );
        push_back( QColor( 255, 0, 255 ) );
        push_back( QColor( 0, 255, 255 ) );
        push_back( QColor( 0, 0, 0 ) );
        push_back( QColor( 255, 255, 255 ) );
        push_back( QColor( 255, 0, 0 ) );
        push_back( QColor( 0, 255, 0 ) );
        push_back( QColor( 0, 0, 255 ) );
        push_back( QColor( 255, 255, 0 ) );
        push_back( QColor( 255, 0, 255 ) );
        push_back( QColor( 0, 255, 255 ) );
        push_back( QColor( 128, 0, 0 ) );
        push_back( QColor( 0, 128, 0 ) );
        push_back( QColor( 0, 0, 128 ) );
        push_back( QColor( 128, 128, 0 ) );
        push_back( QColor( 128, 0, 128 ) );
        push_back( QColor( 0, 128, 128 ) );
        push_back( QColor( 192, 192, 192 ) );
        push_back( QColor( 128, 128, 128 ) );
        push_back( QColor( 153, 153, 255 ) );
        push_back( QColor( 153, 51, 102 ) );
        push_back( QColor( 255, 255, 204 ) );
        push_back( QColor( 204, 255, 255 ) );
        push_back( QColor( 102, 0, 102 ) );
        push_back( QColor( 255, 128, 128 ) );
        push_back( QColor( 0, 102, 204 ) );
        push_back( QColor( 204, 204, 255 ) );
        push_back( QColor( 0, 0, 128 ) );
        push_back( QColor( 255, 0, 255 ) );
        push_back( QColor( 255, 255, 0 ) );
        push_back( QColor( 0, 255, 255 ) );
        push_back( QColor( 128, 0, 128 ) );
        push_back( QColor( 128, 0, 0 ) );
        push_back( QColor( 0, 128, 128 ) );
        push_back( QColor( 0, 0, 255 ) );
        push_back( QColor( 0, 204, 255 ) );
        push_back( QColor( 204, 255, 255 ) );
        push_back( QColor( 204, 255, 204 ) );
        push_back( QColor( 255, 255, 153 ) );
        push_back( QColor( 153, 204, 255 ) );
        push_back( QColor( 255, 153, 204 ) );
        push_back( QColor( 204, 153, 255 ) );
        push_back( QColor( 255, 204, 153 ) );
        push_back( QColor( 51, 102, 255 ) );
        push_back( QColor( 51, 204, 204 ) );
        push_back( QColor( 153, 204, 0 ) );
        push_back( QColor( 255, 204, 0 ) );
        push_back( QColor( 255, 153, 0 ) );
        push_back( QColor( 255, 102, 0 ) );
        push_back( QColor( 102, 102, 153 ) );
        push_back( QColor( 150, 150, 150 ) );
        push_back( QColor( 0, 51, 102 ) );
        push_back( QColor( 51, 102, 153 ) );
        push_back( QColor( 0, 51, 0 ) );
        push_back( QColor( 51, 51, 0 ) );
        push_back( QColor( 153, 51, 0 ) );
        push_back( QColor( 153, 51, 102 ) );
        push_back( QColor( 51, 51, 153 ) );
        push_back( QColor( 51, 51, 51 ) );
	// FIXME: The palette colors can apparently change during execution so
	//        this should be calculated on the fly instead.
        push_back( QPalette().color( QPalette::Active, QPalette::WindowText ) );
        push_back( QPalette().color( QPalette::Active, QPalette::Window ) );
     }
};

Q_GLOBAL_STATIC( DefaultIndexedColors, s_defaultIndexedColors )

QColor MSO::defaultIndexedColor( int index )
{
    if( index < 0 || s_defaultIndexedColors->count() <= index )
        return QColor();
    return s_defaultIndexedColors->at( index );
}


class LangIdToLocaleMapping : public QMap< int, QString >
{
public:
    LangIdToLocaleMapping()
    {
#define DEFINELOCALE( ID, CODE ) insert( ID, QLatin1String( CODE ) );
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

#undef DEFINELOCALE
    }
};

Q_GLOBAL_STATIC( LangIdToLocaleMapping, s_LangIdToLocaleMapping )

QLocale MSO::localeForLangId( int langid )
{
    return QLocale( s_LangIdToLocaleMapping->value( langid ) );
}
