/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <koGlobal.h>
#include <kdebug.h>
#include <klocale.h>
#include <koprinter.h>


int KoPageFormat::printerPageSize( KoFormat format )
{
    switch ( format ) {
        case PG_DIN_A3:
            return KPrinter::A3;
        case PG_DIN_A4:
            return KPrinter::A4;
        case PG_DIN_A5:
            return KPrinter::A5;
        case PG_US_LETTER:
            return KPrinter::Letter;
        case PG_US_LEGAL:
            return KPrinter::Legal;
        case PG_SCREEN:
            kdWarning() << "You use the page layout SCREEN. Printing in DIN A4 LANDSCAPE." << endl;
            return KPrinter::A4;
        case PG_CUSTOM:
            kdWarning() << "The used page layout (CUSTOM) is not supported by KPrinter. Printing in A4." << endl;
            return KPrinter::A4;
        case PG_DIN_B5:
            return KPrinter::B5;
        case PG_US_EXECUTIVE:
            return KPrinter::Executive;
        case PG_DIN_A0:
            return KPrinter::A0;
        case PG_DIN_A1:
            return KPrinter::A1;
        case PG_DIN_A2:
            return KPrinter::A2;
        case PG_DIN_A6:
            return KPrinter::A6;
        case PG_DIN_A7:
            return KPrinter::A7;
        case PG_DIN_A8:
            return KPrinter::A8;
        case PG_DIN_A9:
            return KPrinter::A9;
        case PG_DIN_B0:
            return KPrinter::B0;
        case PG_DIN_B1:
            return KPrinter::B1;
        case PG_DIN_B10:
            return KPrinter::B10;
        case PG_DIN_B2:
            return KPrinter::B2;
        case PG_DIN_B3:
            return KPrinter::B3;
        case PG_DIN_B4:
            return KPrinter::B4;
        case PG_DIN_B6:
            return KPrinter::B6;
    }
}

double KoPageFormat::width( KoFormat format, KoOrientation orientation )
{
    if ( orientation == PG_LANDSCAPE )
        return height( format, PG_PORTRAIT );
    switch( format ) {
        case PG_DIN_A3:
            return PG_A3_WIDTH;
        case PG_DIN_A4:
            return PG_A4_WIDTH;
        case PG_DIN_A5:
            return PG_A5_WIDTH;
        case PG_US_LETTER:
            return PG_US_LETTER_WIDTH;
        case PG_US_LEGAL:
            return PG_US_LEGAL_WIDTH;
        case PG_SCREEN:
            return PG_A4_HEIGHT; // since we fallback on A4 landscape
        case PG_CUSTOM:
            kdWarning() << "KoPageFormat::width called with PG_CUSTOM!" << endl;
            return 0;
        case PG_DIN_B5:
            return PG_B5_WIDTH;
        case PG_US_EXECUTIVE:
            return PG_US_EXECUTIVE_WIDTH;
        case PG_DIN_A0:
            return 841.0;
        case PG_DIN_A1:
            return 594.0;
        case PG_DIN_A2:
            return 420.0;
        case PG_DIN_A6:
            return 105.0;
        case PG_DIN_A7:
            return 74.0;
        case PG_DIN_A8:
            return 52.0;
        case PG_DIN_A9:
            return 37.0;
        case PG_DIN_B0:
            return 1030.0;
        case PG_DIN_B1:
            return 728.0;
        case PG_DIN_B10:
            return 32.0;
        case PG_DIN_B2:
            return 515.0;
        case PG_DIN_B3:
            return 364.0;
        case PG_DIN_B4:
            return 257.0;
        case PG_DIN_B6:
            return 128.0;
    }
}

double KoPageFormat::height( KoFormat format, KoOrientation orientation )
{
    if ( orientation == PG_LANDSCAPE )
        return width( format, PG_PORTRAIT );
    switch( format ) {
        case PG_DIN_A3:
            return PG_A3_HEIGHT;
        case PG_DIN_A4:
            return PG_A4_HEIGHT;
        case PG_DIN_A5:
            return PG_A5_HEIGHT;
        case PG_US_LETTER:
            return PG_US_LETTER_HEIGHT;
        case PG_US_LEGAL:
            return PG_US_LEGAL_HEIGHT;
        case PG_SCREEN:
            return PG_A4_WIDTH; // since we fallback on A4 landscape
        case PG_CUSTOM:
            kdWarning() << "KoPageFormat::height called with PG_CUSTOM!" << endl;
            return 0;
        case PG_DIN_B5:
            return PG_B5_HEIGHT;
        case PG_US_EXECUTIVE:
            return PG_US_EXECUTIVE_HEIGHT;
        case PG_DIN_A0:
            return 1189.0;
        case PG_DIN_A1:
            return 841.0;
        case PG_DIN_A2:
            return 594.0;
        case PG_DIN_A6:
            return 148.0;
        case PG_DIN_A7:
            return 105.0;
        case PG_DIN_A8:
            return 74.0;
        case PG_DIN_A9:
            return 52.0;
        case PG_DIN_B0:
            return 1456.0;
        case PG_DIN_B1:
            return 1030.0;
        case PG_DIN_B10:
            return 45.0;
        case PG_DIN_B2:
            return 728.0;
        case PG_DIN_B3:
            return 515.0;
        case PG_DIN_B4:
            return 364.0;
        case PG_DIN_B6:
            return 182.0;
    }
}

QString KoPageFormat::formatString( KoFormat format )
{
    switch( format )
    {
        case PG_DIN_A3:
            return QString::fromLatin1( "A3" );
        case PG_DIN_A4:
            return QString::fromLatin1( "A4" );
        case PG_DIN_A5:
            return QString::fromLatin1( "A5" );
        case PG_US_LETTER:
            return QString::fromLatin1( "Letter" );
        case PG_US_LEGAL:
            return QString::fromLatin1( "Legal" );
        case PG_SCREEN:
            return QString::fromLatin1( "Screen" );
        case PG_CUSTOM:
            return QString::fromLatin1( "Custom" );
        case PG_DIN_B5:
            return QString::fromLatin1( "B5" );
        case PG_US_EXECUTIVE:
            return QString::fromLatin1( "Executive" );
        case PG_DIN_A0:
            return QString::fromLatin1( "A0" );
        case PG_DIN_A1:
            return QString::fromLatin1( "A1" );
        case PG_DIN_A2:
            return QString::fromLatin1( "A2" );
        case PG_DIN_A6:
            return QString::fromLatin1( "A6" );
        case PG_DIN_A7:
            return QString::fromLatin1( "A7" );
        case PG_DIN_A8:
            return QString::fromLatin1( "A8" );
        case PG_DIN_A9:
            return QString::fromLatin1( "A9" );
        case PG_DIN_B0:
            return QString::fromLatin1( "B0" );
        case PG_DIN_B1:
            return QString::fromLatin1( "B1" );
        case PG_DIN_B10:
            return QString::fromLatin1( "B10" );
        case PG_DIN_B2:
            return QString::fromLatin1( "B2" );
        case PG_DIN_B3:
            return QString::fromLatin1( "B3" );
        case PG_DIN_B4:
            return QString::fromLatin1( "B4" );
        case PG_DIN_B6:
            return QString::fromLatin1( "B6" );
    }
}

KoFormat KoPageFormat::formatFromString( const QString & string )
{
    if ( string == "A3" )
        return PG_DIN_A3;
    if ( string == "A4" )
        return PG_DIN_A4;
    if ( string == "A5" )
        return PG_DIN_A5;
    if ( string == "Letter" )
        return PG_US_LETTER;
    if ( string == "Legal" )
        return PG_US_LEGAL;
    if ( string == "Screen" )
        return PG_SCREEN;
    if ( string == "Custom" )
        return PG_CUSTOM;
    if ( string == "B5" )
        return PG_DIN_B5;
    if ( string == "Executive" )
        return PG_US_EXECUTIVE;
    if ( string == "A0" )
        return PG_DIN_A0;
    if ( string == "A1" )
        return PG_DIN_A1;
    if ( string == "A2" )
        return PG_DIN_A2;
    if ( string == "A6" )
        return PG_DIN_A6;
    if ( string == "A7" )
        return PG_DIN_A7;
    if ( string == "A8" )
        return PG_DIN_A8;
    if ( string == "A9" )
        return PG_DIN_A9;
    if ( string == "B0" )
        return PG_DIN_B0;
    if ( string == "B1" )
        return PG_DIN_B1;
    if ( string == "B10" )
        return PG_DIN_B10;
    if ( string == "B2" )
        return PG_DIN_B2;
    if ( string == "B3" )
        return PG_DIN_B3;
    if ( string == "B4" )
        return PG_DIN_B4;
    if ( string == "B6" )
        return PG_DIN_B6;
}

QString KoPageFormat::name( KoFormat format )
{
    switch( format ) {
        case PG_DIN_A3:
            return i18n("DIN A3");
        case PG_DIN_A4:
            return i18n("DIN A4");
        case PG_DIN_A5:
            return i18n("DIN A5");
        case PG_US_LETTER:
            return i18n("US Letter");
        case PG_US_LEGAL:
            return i18n("US Legal");
        case PG_SCREEN:
            return i18n("Screen");
        case PG_CUSTOM:
            return i18n("Custom");
        case PG_DIN_B5:
            return i18n("DIN B5");
        case PG_US_EXECUTIVE:
            return i18n("US Executive");
        case PG_DIN_A0:
            return i18n( "DIN A0" );
        case PG_DIN_A1:
            return i18n( "DIN A1" );
        case PG_DIN_A2:
            return i18n( "DIN A2" );
        case PG_DIN_A6:
            return i18n( "DIN A6" );
        case PG_DIN_A7:
            return i18n( "DIN A7" );
        case PG_DIN_A8:
            return i18n( "DIN A8" );
        case PG_DIN_A9:
            return i18n( "DIN A9" );
        case PG_DIN_B0:
            return i18n( "DIN B0" );
        case PG_DIN_B1:
            return i18n( "DIN B1" );
        case PG_DIN_B10:
            return i18n( "DIN B10" );
        case PG_DIN_B2:
            return i18n( "DIN B2" );
        case PG_DIN_B3:
            return i18n( "DIN B3" );
        case PG_DIN_B4:
            return i18n( "DIN B4" );
        case PG_DIN_B6:
            return i18n( "DIN B6" );
    }
}

QStringList KoPageFormat::allFormats()
{
    QStringList lst;
    lst += i18n( "DIN A3" );
    lst += i18n( "DIN A4" );
    lst += i18n( "DIN A5" );
    lst += i18n( "US Letter" );
    lst += i18n( "US Legal" );
    lst += i18n( "Screen" );
    lst += i18n( "Custom" );
    lst += i18n( "DIN B5" );
    lst += i18n( "US Executive" );
    lst += i18n( "DIN A0" );
    lst += i18n( "DIN A1" );
    lst += i18n( "DIN A2" );
    lst += i18n( "DIN A6" );
    lst += i18n( "DIN A7" );
    lst += i18n( "DIN A8" );
    lst += i18n( "DIN A9" );
    lst += i18n( "DIN B0" );
    lst += i18n( "DIN B1" );
    lst += i18n( "DIN B10" );
    lst += i18n( "DIN B2" );
    lst += i18n( "DIN B3" );
    lst += i18n( "DIN B4" );
    lst += i18n( "DIN B6" );
    return lst;
}
