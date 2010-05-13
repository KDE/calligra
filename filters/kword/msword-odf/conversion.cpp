/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>
   Copyright (C) 2008 Benjamin Cail <cricketc@gmail.com>
   Copyright (C) 2009 Inge Wallin   <inge@lysator.liu.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.

*/

#include "conversion.h"

#include <wv2/src/word97_generated.h>
#include <wv2/src/functordata.h>
#include <wv2/src/fields.h>

#include <kdebug.h>
#include <QRegExp>
#include <qdom.h>
#include <QString>
#include <klocale.h>

QString Conversion::styleNameString(const wvWare::UString& str)
{
    QString string = QString::fromRawData(reinterpret_cast<const QChar*>(str.data()), str.length());
    //first replace all spaces with _20_
    string.replace(' ', "_20_");
    //now remove random characters
    for (int i = 0; i < string.size(); i++) {
        if (!string[i].isLetterOrNumber()) {
            if (string[i] != '_') {
                string.remove(i, 1);
                i--;
            }
        }
    }
    //if first character is a digit, it doesn't validate properly
    if (string[0].isDigit()) {
        string.prepend("s");
    }
    return string;
}

QString Conversion::color(int number, int defaultcolor, bool defaultWhite)
{
    switch (number) {
    case 0:
        if (defaultWhite)
            return QString("#FFFFFF");
    case 1://black
        return QString("#000000");
    case 2://blue
        return QString("#0000FF");
    case 3://cyan
        return QString("#00FFFF");
    case 4://green
        return QString("#008000");
    case 5://magenta
        return QString("#FF00FF");
    case 6://red
        return QString("#FF0000");
    case 7://yellow
        return QString("#FFFF00");
    case 8://white
        return QString("#FFFFFF");
    case 9://dark blue
        return QString("#00008B");
    case 10://dark cyan
        return QString("#008B8B");
    case 11://dark green
        return QString("#006400");
    case 12://dark magenta
        return QString("#8B008B");
    case 13://dark red
        return QString("#8B0000");
    case 14://dark yellow
        return QString("#808000");
    case 15://dark gray
        return QString("#A9A9A9");
    case 16://light gray
        return QString("#D3D3D3");

    default:
        kDebug(30513) << " unknown color:" << number;
        if (defaultcolor == -1) //return black
            return QString("#000000");
        else //call this function again with the default color value
            //to see if it works
            return color(defaultcolor, -1);
    }
}

int Conversion::fillPatternStyle(int ipat)
{
    // See $QTDIR/doc/html/qbrush.html#setStyle
    switch (ipat)  {
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
        kWarning(30513) << "Unhandled undocumented SHD ipat value: " << ipat;
        return Qt::NoBrush;
    }
}

uint Conversion::shadingPatternToColor(int ipat)
{
    uint resultColor = 0xffffff, grayLevel = 0;
    bool ok;

    grayLevel = ditheringToGray(ipat, &ok);     // try to convert ipat to gray level

    if(ok != true)                              // if conversion failed, return white (shouldn't happen)
        return resultColor;

    resultColor = (grayLevel << 16) | (grayLevel <<  8) | grayLevel;    // construct RGB from the same value (to create gray)
    return resultColor;
}

int Conversion::ditheringToGray(int ipat, bool* ok)
{
    *ok = true; // optimistic ;)
    switch (ipat)  {
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

void Conversion::setColorAttributes(QDomElement& element, int ico, const QString& prefix, bool defaultWhite)
{
    QColor color = Conversion::color(ico, -1, defaultWhite);
    element.setAttribute(prefix.isNull() ? "red" : prefix + "Red", color.red());
    element.setAttribute(prefix.isNull() ? "blue" : prefix + "Blue", color.blue());
    element.setAttribute(prefix.isNull() ? "green" : prefix + "Green", color.green());
}

//get a correct fo:border-line-width value "innerwidth space outerwidth"
//innerwidth = metric
//space = metric
//outerwidth = metric
QString Conversion::setDoubleBorderAttributes(const wvWare::Word97::BRC& brc)
{
    qreal w =  brc.dptLineWidth / 8.0;

    switch (brc.brcType) {
    case 0: // none
    case 1: // single
    case 2: // thick
    case 5: // hairline
    case 6: // dot
    case 7: // dash large gap
    case 8: // dot dash
    case 9: // dot dot dash
    case 20: // wave
    case 22: // dash small gap
    case 23: // dash dot stroked
    case 24: // emboss 3D
    case 25: // engrave 3D
    default:
        return QString(); // single lines so just return blank

    case 10: // triple
        return QString::number(w) + "pt " + QString::number(w*3) + "pt " + QString::number(w) + "pt";
    case 13: // thin-thick-thin small gap
    case 16: // thin-thick-thin medium gap
    case 19: // thin-thick-thin large gap

    case 3: // double
        return QString::number(w) + "pt " + QString::number(w) + "pt " + QString::number(w) + "pt";
    case 11: // thin-thick small gap
        return QString::number(w*0.25) + "pt " + QString::number(w*0.25) + "pt " + QString::number(w) + "pt";
    case 12: // thick-thin small gap
        return QString::number(w) + "pt " + QString::number(w*0.25) + "pt " + QString::number(w*0.25) + "pt";
    case 14: // thin-thick medium gap
        return QString::number(w*0.5) + "pt " + QString::number(w*0.5) + "pt " + QString::number(w) + "pt";
    case 15: // thick-thin medium gap
        return QString::number(w) + "pt " + QString::number(w*0.5) + "pt " + QString::number(w*0.5) + "pt";
    case 17: // thin-thick large gap
        return QString::number(w*0.25) + "pt " + QString::number(w) + "pt " + QString::number(w*0.5) + "pt";
    case 18: //  thick-thin large gap
        return QString::number(w*0.5) + "pt " + QString::number(w) + "pt " + QString::number(w*0.25) + "pt";
    case 21: // double wave
        return QString::number(w*2.5) + "pt " + QString::number(w*1.25) + "pt " + QString::number(w*2.5) + "pt";
    }
}

//get a correct fo:border value "width style color"
//width = thick, thin, or length specification
//style = none, solid, or double
//color = six-digit hexadecimal color value
QString Conversion::setBorderAttributes(const wvWare::Word97::BRC& brc)
{
    kDebug(30153) << "brc.brcType      = " << brc.brcType;
    kDebug(30153) << "brc.dptLineWidth = " << brc.dptLineWidth;
    kDebug(30153) << "brc.cv           = " << brc.cv;


    //set the border width
    qreal w;
    if (brc.brcType < 0x40) // this is according to http://msdn.microsoft.com/en-us/library/dd907496.aspx
        w = brc.dptLineWidth / 8.0;
    else
        w = brc.dptLineWidth;

    QString style("solid");   //reasonable default
    QString color = '#' + QString::number(brc.cv | 0xff000000, 16).right(6).toUpper();

    switch (brc.brcType) {
    case 0: // none
        //Q_ASSERT( brc.dptLineWidth == 0 ); // otherwise kword will show a border!
        style = "none";
        break;
    case 11: // thin-thick small gap
    case 12: // thick-thin small gap
        style = "double";
        w *= 1.5;
        break;
    case 17: // thin-thick large gap
    case 18: //  thick-thin large gap
        style = "double";
        w *= 1.75;
        break;
    case 14: // thin-thick medium gap
    case 15: // thick-thin medium gap
        style = "double";
        w *= 2.0;
        break;
    case 3: // double
        style = "double";
        w *= 3;
        break;
    case 5: //"hairline"
        w = 0.01;
        break;

        //ODF doesn't support dot dashed or wavy borders???

    case 7: // dash large gap
    case 22: // dash small gap
        style = "dashed"; // KWord: dashes //FIXME
        break;
    case 6: // dot
        style = "dotted";
        break;
    case 8: // dot dash
        style = "dashed"; //FIXME
        break;
    case 9: // dot dot dash
        style = "dashed"; //FIXME
        break;

    case 20: // wave
        w *= 4; // Note: we can't make a wave but at least we can make it just as wide
        break;
    case 21: // double wave
        w *= 6.25;
        style = "double"; // Note: we can't make a wave but at least we can make it just as wide
        break;

    case 10: // triple
        w *= 5;
        style = "double"; //Note: odf only support double so that will have to do
        break;

    case 13: // thin-thick-thin small gap
    case 16: // thin-thick-thin medium gap
    case 19: // thin-thick-thin large gap
    default:
        //if a fancy unsupported border is specified -> better a normal border than none
        //so just leave values as defaults
        break;
    }

    QString width =  QString::number(w) + "pt";

    QString value(width);
    value.append(" ");
    value.append(style);
    value.append(" ");
    value.append(color);

    return value;
}
//get a  koffice:borderspecial value "style"
QString Conversion::borderKOfficeAttributes(const wvWare::Word97::BRC& brc)
{
    kDebug(30153) << "brc.brcType      = " << brc.brcType;
    kDebug(30153) << "brc.dptLineWidth = " << brc.dptLineWidth;
    kDebug(30153) << "brc.cv           = " << brc.cv;


    QString style;   //empty if nothing special is neededreasonable default

    switch (brc.brcType) {
        //ODF doesn't support dot dashed or wavy borders???

    case 7: // dash large gap
        style = "dashed largegap";
        break;
    case 22: // dash small gap
        style = "dashed smallgap";
        break;
    case 6: // dot
        style = "dotted";
        break;
    case 8: // dot dash
        style = "dot-dashed";
        break;
    case 9: // dot dot dash
        style = "dot-dot-dashed";
        break;

    case 20: // wave
        style = "wave";
        break;
    case 21: // double wave
        style = "double wave";
        break;

    case 10: // triple
        style = "triple";
        break;

    case 13: // thin-thick-thin small gap
    case 16: // thin-thick-thin medium gap
    case 19: // thin-thick-thin large gap
    default:
        //if a fancy unsupported border is specified -> better a normal border than none
        //so just leave values as defaults
        break;
    }

    return style;
}

QString Conversion::numberFormatCode(int nfc)
{
    QString value("");
    switch (nfc) {
    case 1: // upper case roman
        value = 'I';
        break;
    case 2: // lower case roman
        value = 'i';
        break;
    case 3: // upper case letter
        value = 'A';
        break;
    case 4: // lower case letter
        value = 'a';
        break;
    case 5: // arabic with a trailing dot (added by writeCounter)
    case 6: // numbered (one, two, three) - not supported by KWord
    case 7: // ordinal (first, second, third) - not supported by KWord
    case 22: // leading zero (01-09, 10-99, 100-...) - not supported by KWord
    case 0: // arabic
        value = '1';
        break;
    default:
        kWarning(30513) << "Unknown NFC: " << nfc;
        value = '1';
    }
    return value;
}

int Conversion::headerTypeToFrameInfo(unsigned char type)
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

QString Conversion::headerTypeToFramesetName(unsigned char type)
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
    return QString();
}

bool Conversion::isHeader(unsigned char type)
{
    switch (type) {
    case wvWare::HeaderData::HeaderEven:
    case wvWare::HeaderData::HeaderOdd:
    case wvWare::HeaderData::HeaderFirst:
        return true;
    }
    return false;
}

int Conversion::headerMaskToHType(unsigned char mask)
{
    bool hasFirst = (mask & wvWare::HeaderData::HeaderFirst);
    // Odd is always there. We have even!=odd only if Even is there too.
    bool hasEvenOdd = (mask & wvWare::HeaderData::HeaderEven);
    //kDebug(30513) <<" hasEvenOdd=" << hasEvenOdd;
    if (hasFirst)
        return hasEvenOdd ? 1 : 2;
    return hasEvenOdd ? 3 : 0;
}

int Conversion::headerMaskToFType(unsigned char mask)
{
    bool hasFirst = (mask & wvWare::HeaderData::FooterFirst);
    bool hasEvenOdd = (mask & wvWare::HeaderData::FooterEven);
    // Odd is always there. We have even!=odd only if Even is there too.
    kDebug(30513) << " hasEvenOdd=" << hasEvenOdd;
    if (hasFirst)
        return hasEvenOdd ? 1 : 2;
    return hasEvenOdd ? 3 : 0;
}

int Conversion::fldToFieldType(const wvWare::FLD* fld)
{
    // assume unhandled
    int m_fieldType = -1;

    // sanity check
    if (!fld) return -1;

    switch (fld->flt) {
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

    if (m_fieldType < 0)
        kDebug(30513) << "unhandled field: fld.ftl:" << (int)fld->flt;

    return m_fieldType;
}

qreal Conversion::twipsToInch(int twips)
{
    qreal inches = (qreal) twips;
    inches = inches * 0.00069444444;
    return inches;
}

qreal Conversion::twipsToPt(int twips)
{
    qreal pt = (qreal) twips;
    pt = pt / 20.0;
    return pt;
}

QString Conversion::rncToStartNumberingAt(int rnc) 
{
    switch(rnc) {
    case 0:
        return "document";
    case 1:
        return "section";
    case 2:
    default:
        return "page";
    }
}

