/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "conversion.h"

#include <word97_generated.h>
#include <functordata.h>

#include <kdebug.h>
#include <qregexp.h>
#include <qdom.h>
#include <klocale.h>

QColor Conversion::color(int number, int defaultcolor, bool defaultWhite)
{
    switch(number)
    {
	case 0:
	    if(defaultWhite)
		return QColor("white");
	case 1:
	    return QColor("black");
	case 2:
	    return QColor("blue");
	case 3:
	    return QColor("cyan");
	case 4:
	    return QColor("green");
	case 5:
	    return QColor("magenta");
	case 6:
	    return QColor("red");
	case 7:
	    return QColor("yellow");
	case 8:
	    return QColor("white");
	case 9:
	    return QColor("darkBlue");
	case 10:
	    return QColor("darkCyan");
	case 11:
	    return QColor("darkGreen");
	case 12:
	    return QColor("darkMagenta");
	case 13:
	    return QColor("darkRed");
	case 14:
	    return QColor("darkYellow");
	case 15:
	    return QColor("darkGray");
	case 16:
	    return QColor("lightGray");

	default:
	    if(defaultcolor == -1)
		return QColor("black");
	    else
		return color(defaultcolor, -1);
    }
}

QString Conversion::alignment( int jc ) {
    QString value( "left" );
    if ( jc == 1 )
        value = "center";
    else if ( jc == 2 )
        value = "right";
    else if ( jc == 3 )
        value = "justify";
    return value;
}

QString Conversion::lineSpacing( const wvWare::Word97::LSPD& lspd )
{
    QString value( "0" );
    if ( lspd.fMultLinespace == 1 )
    {
        // This will be e.g. 1.5 for a 1.5 linespacing.
        float proportionalLineSpacing = (float)lspd.dyaLine / 240.0;
        if ( QABS(proportionalLineSpacing - 1.5) <= 0.25 ) // close to 1.5?
            value = "oneandhalf";
        else if ( proportionalLineSpacing > 1.75) // close to 2.0, or more?
            value = "double";
    }
    else if ( lspd.fMultLinespace == 0 )
    {
        // see sprmPDyaLine in generator_wword8.htm
        //float value = QABS((float)lspd.dyaLine / 20.0); // twip -> pt
        // lspd.dyaLine > 0 means "at least", < 0 means "exactly"
        // "at least" is not possible in kword yet.
        // What's more, it's the size of the whole line, not the spacing between
        // the line (!)
        // To convert between the two, we'd need to find out the height of the
        // highest character in the line, and substract it from the value..... TODO.
    }
    else
        kdWarning() << "Unhandled LSPD::fMultLinespace value: " << lspd.fMultLinespace << endl;
    return value;
}

void Conversion::setBorderAttributes( QDomElement& borderElement, const wvWare::Word97::BRC& brc )
{
    QColor color = Conversion::color( brc.ico, -1 );
    borderElement.setAttribute( "red", color.red() );
    borderElement.setAttribute( "blue", color.blue() );
    borderElement.setAttribute( "green", color.green() );
    borderElement.setAttribute( "width", (double)brc.dptLineWidth / 8.0 );

    QString style = "0"; // KWord: solid
    switch ( brc.brcType ) {
    case 0: // none
        Q_ASSERT( brc.dptLineWidth == 0 ); // otherwise kword will show a border!
        break;
    case 7: // dash large gap
    case 22: // dash small gap
        style = "1"; // KWord: dashes
        break;
    case 6: // dot
        style = "2";
        break;
    case 8: // dot dash
        style = "3";
        break;
    case 9: // dot dot dash
        style = "4";
        break;
    case 3: // double
        style = "5";
        break;
    case 1: // single
    default:
        // if a fancy unsupported border is specified -> better a normal border than none
        // (so we keep the default value, "0", for "solid single line".
        break;
    }
    borderElement.setAttribute( "style", style );
    // We ignore brc.dptSpace (spacing), brc.fShadow (shadow), and brc.fFrame (?)
}

int Conversion::numberFormatCode( int nfc )
{
    switch ( nfc )
    {
    case 1: // upper case roman
        return 5;
    case 2: // lower case roman
        return 4;
    case 3: // upper case letter
        return 3;
    case 4: // lower case letter
        return 2;
    case 5: // arabic with a trailing dot (added by writeCounter)
    case 6: // numbered (one, two, three) - not supported by KWord
    case 7: // ordinal (first, second, third) - not supported by KWord
    case 22: // leading zero (01-09, 10-99, 100-...) - not supported by KWord
    case 0: // arabic
        return 1;
    }
    kdWarning() << k_funcinfo << "Unknown NFC: " << nfc << endl;
    return 1;
}

int Conversion::headerTypeToFrameInfo( unsigned char type )
{
    switch (type) {
    case wvWare::HeaderData::HeaderEven:
        return 3;
    case wvWare::HeaderData::HeaderOdd:
        return 2;
    case wvWare::HeaderData::FooterEven:
        return 6;
    case wvWare::HeaderData::FooterOdd:
        return 5;
    case wvWare::HeaderData::HeaderFirst:
        return 1;
    case wvWare::HeaderData::FooterFirst:
        return 4;
    }
    return 0;
}

QString Conversion::headerTypeToFramesetName( unsigned char type )
{
    switch (type) {
    case wvWare::HeaderData::HeaderEven:
        return i18n("Even Pages Header");
    case wvWare::HeaderData::HeaderOdd:
        return i18n("Odd Pages Header");
    case wvWare::HeaderData::FooterEven:
        return i18n("Even Pages Footer");
    case wvWare::HeaderData::FooterOdd:
        return i18n("Odd Pages Footer");
    case wvWare::HeaderData::HeaderFirst:
        return i18n("First Page Header");
    case wvWare::HeaderData::FooterFirst:
        return i18n("First Page Footer");
    }
    return QString::null;
}

bool Conversion::isHeader( unsigned char type )
{
    switch (type) {
    case wvWare::HeaderData::HeaderEven:
    case wvWare::HeaderData::HeaderOdd:
    case wvWare::HeaderData::HeaderFirst:
        return true;
    }
    return false;
}
