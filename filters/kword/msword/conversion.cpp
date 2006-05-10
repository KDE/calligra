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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "conversion.h"

#include <wv2/word97_generated.h>
#include <wv2/functordata.h>
#include <wv2/fields.h>

#include <kdebug.h>
#include <QRegExp>
#include <qdom.h>
#include <klocale.h>

QColor Conversion::color(int number, int defaultcolor, bool defaultWhite)
{
    switch(number)
    {
	case 0:
	    if(defaultWhite)
		return Qt::white;
	case 1:
	    return Qt::black;
	case 2:
	    return Qt::blue;
	case 3:
	    return Qt::cyan;
	case 4:
	    return Qt::green;
	case 5:
	    return Qt::magenta;
	case 6:
	    return Qt::red;
	case 7:
	    return Qt::yellow;
	case 8:
	    return Qt::white;
	case 9:
	    return Qt::darkBlue;
	case 10:
	    return Qt::darkCyan;
	case 11:
	    return Qt::darkGreen;
	case 12:
	    return Qt::darkMagenta;
	case 13:
	    return Qt::darkRed;
	case 14:
	    return Qt::darkYellow;
	case 15:
	    return Qt::darkGray;
	case 16:
	    return Qt::lightGray;

	default:
            kDebug(30513) << "Conversion::color: unknown color: " << number << endl;
	    if(defaultcolor == -1)
		return QColor("black");
	    else
		return color(defaultcolor, -1);
    }
}

int Conversion::fillPatternStyle( int ipat )
{
    // See $QTDIR/doc/html/qbrush.html#setStyle
    switch( ipat )  {
    case 0: // Automatic (Apparently it means Solid from background color instead of foreground)
    case 1: // Solid
        return Qt::SolidPattern;
    case 2: // 5%
    case 35: // 2.5 Percent
    case 36: // 7.5 Percent
        return Qt::Dense7Pattern;
    case 3: // 10%
    case 37: // 12.5 Percent
    case 38: // 15 Percent
    case 39: // 17.5 Percent
    case 4: // 20%
        return Qt::Dense6Pattern;
    case 40: // 22.5 Percent
    case 5: // 25%
    case 41: // 27.5 Percent
    case 6: // 30%
    case 42: // 32.5 Percent
    case 43: // 35 Percent
    case 44: // 37.5 Percent
    case 7: // 40%
        return Qt::Dense5Pattern;
    case 45: // 42.5 Percent
    case 46: // 45 Percent
    case 47: // 47.5 Percent
    case 8: // 50%
    case 48: // 52.5 Percent
    case 49: // 55 Percent
        return Qt::Dense4Pattern;
    case 50: // 57.5 Percent
    case 9: // 60%
    case 51: // 62.5 Percent
    case 52: // 65 Percent
    case 53: // 67.5 Percent
    case 10: // 70%
    case 54: // 72.5 Percent
        return Qt::Dense3Pattern;
    case 11: // 75%
    case 55: // 77.5 Percent
    case 12: // 80%
    case 56: // 82.5 Percent
    case 57: // 85 Percent
    case 58: // 87.5 Percent
    case 13: // 90%
        return Qt::Dense2Pattern;
    case 59: // 92.5 Percent
    case 60: // 95 Percent
    case 61: // 97.5 Percent
    case 62: // 97 Percent
        return Qt::Dense1Pattern;
    case 14: // Dark Horizontal
    case 20: // Horizontal
        return Qt::HorPattern;
    case 15: // Dark Vertical
    case 21: // Vertical
        return Qt::VerPattern;
    case 16: // Dark Forward Diagonal
    case 22: // Forward Diagonal
        return Qt::FDiagPattern;
    case 17: // Dark Backward Diagonal
    case 23: // Backward Diagonal
        return Qt::BDiagPattern;
    case 18: // Dark Cross
    case 24: // Cross
        return Qt::CrossPattern;
    case 19: // Dark Diagonal Cross
    case 25: // Diagonal Cross
        return Qt::DiagCrossPattern;
    default:
        kWarning(30513) << "Unhandled undocumented SHD ipat value: " << ipat << endl;
        return Qt::NoBrush;
    }
}


int Conversion::ditheringToGray( int ipat, bool* ok )
{
    *ok = true; // optimistic ;)
    switch( ipat )  {
    case 2: // 5%
        return 255 - qRound(0.05 * 255);
    case 35: // 2.5 Percent
        return 255 - qRound(0.025 * 255);
    case 36: // 7.5 Percent
        return 255 - qRound(0.075 * 255);
    case 3: // 10%
        return 255 - qRound(0.1 * 255);
    case 37: // 12.5 Percent
        return 255 - qRound(0.125 * 255);
    case 38: // 15 Percent
        return 255 - qRound(0.15 * 255);
    case 39: // 17.5 Percent
        return 255 - qRound(0.175 * 255);
    case 4: // 20%
        return 255 - qRound(0.2 * 255);
    case 40: // 22.5 Percent
        return 255 - qRound(0.225 * 255);
    case 5: // 25%
        return 255 - qRound(0.25 * 255);
    case 41: // 27.5 Percent
        return 255 - qRound(0.275 * 255);
    case 6: // 30%
        return 255 - qRound(0.3 * 255);
    case 42: // 32.5 Percent
        return 255 - qRound(0.325 * 255);
    case 43: // 35 Percent
        return 255 - qRound(0.35 * 255);
    case 44: // 37.5 Percent
        return 255 - qRound(0.375 * 255);
    case 7: // 40%
        return 255 - qRound(0.4 * 255);
    case 45: // 42.5 Percent
        return 255 - qRound(0.425 * 255);
    case 46: // 45 Percent
        return 255 - qRound(0.45 * 255);
    case 47: // 47.5 Percent
        return 255 - qRound(0.475 * 255);
    case 8: // 50%
        return 255 - qRound(0.5 * 255);
    case 48: // 52.5 Percent
        return 255 - qRound(0.525 * 255);
    case 49: // 55 Percent
        return 255 - qRound(0.55 * 255);
    case 50: // 57.5 Percent
        return 255 - qRound(0.575 * 255);
    case 9: // 60%
        return 255 - qRound(0.6 * 255);
    case 51: // 62.5 Percent
        return 255 - qRound(0.625 * 255);
    case 52: // 65 Percent
        return 255 - qRound(0.65 * 255);
    case 53: // 67.5 Percent
        return 255 - qRound(0.675 * 255);
    case 10: // 70%
        return 255 - qRound(0.7 * 255);
    case 54: // 72.5 Percent
        return 255 - qRound(0.725 * 255);
    case 11: // 75%
        return 255 - qRound(0.75 * 255);
    case 55: // 77.5 Percent
        return 255 - qRound(0.775 * 255);
    case 12: // 80%
        return 255 - qRound(0.8 * 255);
    case 56: // 82.5 Percent
        return 255 - qRound(0.825 * 255);
    case 57: // 85 Percent
        return 255 - qRound(0.85 * 255);
    case 58: // 87.5 Percent
        return 255 - qRound(0.875 * 255);
    case 13: // 90%
        return 255 - qRound(0.9 * 255);
    case 59: // 92.5 Percent
        return 255 - qRound(0.925 * 255);
    case 60: // 95 Percent
        return 255 - qRound(0.95 * 255);
    case 61: // 97.5 Percent
        return 255 - qRound(0.975 * 255);
    case 62: // 97 Percent
        return 255 - qRound(0.97 * 255);
    default:
        *ok = false;
        return 0;
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
        // "at least" is now possible in kword, but here it's the size of the whole
        // line, not the spacing between the line (!)
        // To convert between the two, we'd need to find out the height of the
        // highest character in the line, and substract it from the value..... Ouch.
        // Better implement line-height-at-least like OOo has.
    }
    else
        kWarning(30513) << "Unhandled LSPD::fMultLinespace value: " << lspd.fMultLinespace << endl;
    return value;
}

void Conversion::setColorAttributes( QDomElement& element, int ico, const QString& prefix, bool defaultWhite )
{
    QColor color = Conversion::color( ico, -1, defaultWhite );
    element.setAttribute( prefix.isNull() ? "red" : prefix+"Red", color.red() );
    element.setAttribute( prefix.isNull() ? "blue" : prefix+"Blue", color.blue() );
    element.setAttribute( prefix.isNull() ? "green" : prefix+"Green", color.green() );
}

void Conversion::setBorderAttributes( QDomElement& borderElement, const wvWare::Word97::BRC& brc, const QString& prefix )
{
    setColorAttributes( borderElement, brc.ico, prefix, false );

    borderElement.setAttribute( prefix.isNull() ? "width" : prefix+"Width",
                                (double)brc.dptLineWidth / 8.0 );

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
    borderElement.setAttribute( prefix.isNull() ? "style" : prefix+"Style", style );
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
    kWarning(30513) << k_funcinfo << "Unknown NFC: " << nfc << endl;
    return 1;
}

int Conversion::headerTypeToFrameInfo( unsigned char type )
{
    switch (type) {
    case wvWare::HeaderData::HeaderEven:
        return 2;
    case wvWare::HeaderData::HeaderOdd:
        return 3;
    case wvWare::HeaderData::FooterEven:
        return 5;
    case wvWare::HeaderData::FooterOdd:
        return 6;
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

int Conversion::headerMaskToHType( unsigned char mask )
{
    bool hasFirst = ( mask & wvWare::HeaderData::HeaderFirst );
    // Odd is always there. We have even!=odd only if Even is there too.
    bool hasEvenOdd = ( mask & wvWare::HeaderData::HeaderEven );
    //kDebug(30513) << k_funcinfo << " hasEvenOdd=" << hasEvenOdd << endl;
    if ( hasFirst )
        return hasEvenOdd ? 1 : 2;
    return hasEvenOdd ? 3 : 0;
}

int Conversion::headerMaskToFType( unsigned char mask )
{
    bool hasFirst = ( mask & wvWare::HeaderData::FooterFirst );
    bool hasEvenOdd = ( mask & wvWare::HeaderData::FooterEven );
    // Odd is always there. We have even!=odd only if Even is there too.
    kDebug(30513) << k_funcinfo << " hasEvenOdd=" << hasEvenOdd << endl;
    if ( hasFirst )
        return hasEvenOdd ? 1 : 2;
    return hasEvenOdd ? 3 : 0;
}

int Conversion::fldToFieldType( const wvWare::FLD* fld )
{
    // assume unhandled
    int m_fieldType = -1;

    // sanity check
    if( !fld ) return -1;

    switch( fld->flt )
    {
    case 15:    m_fieldType = 10; break;  // title
    case 17:    m_fieldType =  2; break;  // author
    case 18:    m_fieldType = -1; break;  // keywords (unhandled)
    case 19:    m_fieldType = 11; break;  // comments (unhandled)
    case 21:    m_fieldType = -1; break;  // createdate (unhandled)
    case 22:    m_fieldType = -1; break;  // savedate (unhandled)
    case 23:    m_fieldType = -1; break;  // printdate (unhandled)
    case 25:    m_fieldType = -1; break;  // edittime (unhandled)
    case 29:    m_fieldType =  0; break;  // filename (unhandled)
    case 32:    m_fieldType = -1; break;  // time (unhandled)
    case 60:    m_fieldType =  2; break;  // username <-> KWord's author name
    case 61:    m_fieldType = 16; break;  // userinitials <-> KWord's author initial)
    case 62:    m_fieldType = -1; break;  // useraddress (unhandled)
    default:    m_fieldType = -1; break;
    }

    if( m_fieldType < 0 )
        kDebug(30513) << "unhandled field: fld.ftl: " << (int)fld->flt << endl;

    return m_fieldType;
}
