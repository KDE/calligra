/* This file is part of the Calligra project
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

#include "msdoc.h"
#include "MsDocDebug.h"

#include <wv2/src/word97_generated.h>
#include <wv2/src/functordata.h>
#include <wv2/src/fields.h>

#include <QDomDocument>
#include <QRegExp>
#include <QString>
#include <QMap>

#include <klocalizedstring.h>

//#define CONVERSION_DEBUG_SHD

static QMap<int, qreal> prepareShdPairs() {
    QMap<int, qreal> shadingTable;
    shadingTable[2] = 0.05;
    shadingTable[3] = 0.10;
    shadingTable[4] = 0.20;
    shadingTable[5] = 0.25;
    shadingTable[6] = 0.30;
    shadingTable[7] = 0.40;
    shadingTable[8] = 0.50;
    shadingTable[9] = 0.60;
    shadingTable[10] = 0.70;
    shadingTable[11] = 0.75;
    shadingTable[12] = 0.80;
    shadingTable[13] = 0.90;
    shadingTable[35] = 0.025;
    shadingTable[36] = 0.075;
    shadingTable[37] = 0.125;
    shadingTable[38] = 0.15;
    shadingTable[39] = 0.175;
    shadingTable[40] = 0.225;
    shadingTable[41] = 0.275;
    shadingTable[42] = 0.325;
    shadingTable[43] = 0.35;
    shadingTable[44] = 0.375;
    shadingTable[45] = 0.425;
    shadingTable[46] = 0.45;
    shadingTable[47] = 0.475;
    shadingTable[48] = 0.525;
    shadingTable[49] = 0.55;
    shadingTable[50] = 0.575;
    shadingTable[51] = 0.625;
    shadingTable[52] = 0.65;
    shadingTable[53] = 0.675;
    shadingTable[54] = 0.725;
    shadingTable[55] = 0.775;
    shadingTable[56] = 0.825;
    shadingTable[57] = 0.85;
    shadingTable[58] = 0.875;
    shadingTable[59] = 0.925;
    shadingTable[60] = 0.95;
    shadingTable[61] = 0.975;
    return shadingTable;
}

static const QMap<int, qreal> SHADING_TABLE = prepareShdPairs();


QString Conversion::styleName2QString(const wvWare::UString& str)
{
    return processStyleName(QString::fromRawData(reinterpret_cast<const QChar*>(str.data()), str.length()));
}

QString Conversion::processStyleName(QString str)
{
    //first replace all spaces with _20_
    str.replace(' ', "_20_");
    for (int i = 0; i < str.size(); i++) {
        if (!str[i].isLetterOrNumber()) {
            if (str[i] != '_') {
                str.remove(i, 1);
                i--;
            }
        }
    }
    //if first character is a digit, it doesn't validate properly
    if (str[0].isDigit()) {
        str.prepend("s");
    }
    return str;
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
        debugMsDoc << " unknown color:" << number;
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
        warnMsDoc << "Unhandled undocumented SHD ipat value: " << ipat;
        return Qt::NoBrush;
    }
}

int luma(const QColor &color) {
     return (5036060U * quint32(color.red()) + 9886846U * quint32(color.green()) + 1920103U * quint32(color.blue())) >> 24;
}

int yMix(int yFore, int yBack, qreal pct) {
    return yBack + (yFore - yBack) * pct;
}

QString Conversion::contrastColor(const QString& color)
{
    if (color.isNull()) {
        return QColor(Qt::black).name();
    }

#if 0
    QColor color(bgColor);
    int d = 0;

    // counting the perceptive luminance - human eye favors green color...
    double a = 1 - (0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue()) / 255;

    if (a < 0.5) {
        d = 0; // bright colors - black font
    } else {
        d = 255; // dark colors - white font
    }
    return  QColor(d, d, d).name();
#else
    int luminosity = luma(QColor(color));
    if (luminosity <= 60) {
         return QColor(Qt::white).name();
    } else {
         return QColor(Qt::black).name();
    }
#endif
}

QString Conversion::computeAutoColor(const wvWare::Word97::SHD& shd, const QString& bgColor, const QString& fontColor)
{
    // NOTE: by definition, see
    // http://social.msdn.microsoft.com/Forums/en-US/os_binaryfile/thread/a02a9a24-efb6-4ba0-a187-0e3d2704882b

#ifdef CONVERSION_DEBUG_SHD
    debugMsDoc << Q_FUNC_INFO;
    debugMsDoc << "bgColor:" << bgColor;
    debugMsDoc << "fontColor:" << fontColor;
    debugMsDoc << "ipat:" << shd.ipat;
    debugMsDoc << "cvBack:" << hex << shd.cvBack;
    debugMsDoc << "cvFore:" << hex << shd.cvFore;
#endif

    if (shd.isShdAuto() || shd.isShdNil()) {
        return contrastColor(bgColor);
    }

    QColor foreColor;
    QColor backColor;

    if (shd.cvFore == wvWare::Word97::cvAuto) {
        if (fontColor.isEmpty()) {
            foreColor = QColor(contrastColor(bgColor));
        } else {
            foreColor = QColor(fontColor);
        }
    } else {
        foreColor = QColor(QRgb(shd.cvFore));
    }

    if (shd.cvBack == wvWare::Word97::cvAuto) {
        if (bgColor.isEmpty()) {
            backColor = QColor(Qt::white).name();
        } else {
            backColor = QColor(bgColor);
        }
    } else {
        backColor = QColor(QRgb(shd.cvBack));
    }

    int luminosity = 0;

    if (shd.ipat == ipatAuto) {
        luminosity = luma(backColor);
    }
    else if (shd.ipat == ipatSolid) {
        luminosity = luma(foreColor);
    }
    else if ((shd.ipat > 13) && (shd.ipat < 34)) {
        luminosity = 61;
    } else {
        if (SHADING_TABLE.contains(shd.ipat)) {
            qreal pct = SHADING_TABLE.value(shd.ipat);
            luminosity = yMix( luma(foreColor), luma(backColor), pct);
        } else {
            // this should not happen, but it's binary data
            luminosity = 61;
        }
    }

#ifdef CONVERSION_DEBUG_SHD
    debugMsDoc << "ooooooooooooooooooooooooooooooo    chp: oooooooooooooooo bgColor:" << bgColor;
    debugMsDoc << "fontColor:" << fontColor;
    debugMsDoc << (shd.cvFore == wvWare::Word97::cvAuto);
    debugMsDoc << (shd.cvBack == wvWare::Word97::cvAuto);
    debugMsDoc << "ipat" << shd.ipat;
    debugMsDoc << "fore" << QString::number(shd.cvFore | 0xff000000, 16).right(6) << foreColor.name();
    debugMsDoc << "back" << QString::number(shd.cvBack | 0xff000000, 16).right(6) << backColor.name();
    debugMsDoc << "luminosity " << luminosity;
#endif

    if (luminosity <= 60) { // it is dark color
        // window background color
        return QColor(Qt::white).name();
    } else {
        // window text color
        return QColor(Qt::black).name();
    }

} //computeAutoColor

QString Conversion::shdToColorStr(const wvWare::Word97::SHD& shd, const QString& bgColor, const QString& fontColor)
{
#ifdef CONVERSION_DEBUG_SHD
    debugMsDoc << Q_FUNC_INFO;
    debugMsDoc << "bgColor:" << bgColor;
    debugMsDoc << "fontColor:" << fontColor;
    debugMsDoc << "ipat:" << shd.ipat;
    debugMsDoc << "cvBack:" << hex << shd.cvBack;
    debugMsDoc << "cvFore:" << hex << shd.cvFore;
#endif

    QString ret;
    if (shd.isShdAuto() || shd.isShdNil()) {
        return ret;
    }

    switch (shd.ipat) {
    case ipatAuto: // "Clear" in MS Word UI
        // this color is never Auto, it can only be No Fill
        ret.append(QString::number(shd.cvBack | 0xff000000, 16).right(6).toUpper());
        ret.prepend('#');
        break;
    case ipatSolid:
        if (shd.cvFore == wvWare::Word97::cvAuto) {
            ret = contrastColor(bgColor);
        } else {
            ret.append(QString::number(shd.cvFore | 0xff000000, 16).right(6).toUpper());
            ret.prepend('#');
        }
        break;
    case ipatNil:
        break;
    default:
    {
        //handle remaining ipat values
        quint32 grayClr = shadingPatternToColor(shd.ipat);
        if (grayClr == wvWare::Word97::cvAuto) {
            ret = computeAutoColor(shd, bgColor, fontColor);
        } else {
            ret.append(QString::number(grayClr | 0xff000000, 16).right(6).toUpper());
            ret.prepend('#');

            //TODO: Let's move the following logic into shadingPatternToColor.

            // if both colors are cvAuto, it messes up the logic -- just return
            // the pattern color
            if ((shd.cvFore == wvWare::Word97::cvAuto) &&
                (shd.cvBack == wvWare::Word97::cvAuto))
            {
                return ret;
            }

            QColor foreColor;
            QColor backColor;
            if (shd.cvFore == wvWare::Word97::cvAuto) {
                foreColor = QColor(contrastColor(bgColor));
                //debugMsDoc << "fr auto" << foreColor.name() << "bgColor" << bgColor;
            } else {
                foreColor = QColor(shd.cvFore);
                //debugMsDoc << "fr  set" << foreColor.name();
            }

            if (shd.cvBack == wvWare::Word97::cvAuto) {
                // it's not autocolor, it's probably background color
                backColor = contrastColor(foreColor.name());
                //debugMsDoc << "bg auto" << backColor.name();
            } else {
                backColor = QColor(shd.cvBack);
                //debugMsDoc << "bg  set" << backColor.name();
            }
            qreal pct = QColor(ret).red() / 255.0;
            //debugMsDoc << shd.ipat << "pct" << pct;
            QColor result;
            result.setRed( yMix(backColor.red(), foreColor.red(), pct) );
            result.setGreen( yMix(backColor.green(), foreColor.green(), pct) );
            result.setBlue( yMix(backColor.blue(), foreColor.blue(), pct) );
            ret = result.name();
        }
    }
    break;
    }
    return ret;
}

quint32 Conversion::shadingPatternToColor(const quint16 ipat)
{
    quint32 resultColor = 0xff000000;
    uint grayLevel = 0;
    bool ok;

    // try to convert ipat to gray level
    grayLevel = ditheringToGray(ipat, &ok);

    //looking for a contrast color to the current background
    if (!ok) {
        return resultColor;
    }
    // construct RGB from the same value (to create gray)
    resultColor = (grayLevel << 16) | (grayLevel <<  8) | grayLevel;
    return resultColor;
}

int Conversion::ditheringToGray(const quint16 ipat, bool* ok)
{
    *ok = true; // optimistic ;)
    switch (ipat)  {
    case ipatPct5:
        return (255 - qRound(0.05 * 255));
    case ipatPct10:
        return (255 - qRound(0.1 * 255));
    case ipatPct20:
        return (255 - qRound(0.2 * 255));
    case ipatPct25:
        return (255 - qRound(0.25 * 255));
    case ipatPct30:
        return (255 - qRound(0.3 * 255));
    case ipatPct40:
        return (255 - qRound(0.4 * 255));
    case ipatPct50:
        return (255 - qRound(0.5 * 255));
    case ipatPct60:
        return (255 - qRound(0.6 * 255));
    case ipatPct70:
        return (255 - qRound(0.7 * 255));
    case ipatPct75:
        return (255 - qRound(0.75 * 255));
    case ipatPct80:
        return (255 - qRound(0.8 * 255));
    case ipatPct90:
        return (255 - qRound(0.9 * 255));
    /*
     * TODO: Implementation required, returning default to at least show the
     * user that some type of shading is applied.
     */
    case ipatDkHorizontal:
    case ipatDkVertical:
    case ipatDkForeDiag:
    case ipatDkBackDiag:
    case ipatDkCross:
    case ipatDkDiagCross:
    case ipatHorizontal:
    case ipatVertical:
    case ipatForeDiag:
    case ipatBackDiag:
    case ipatCross:
    case ipatDiagCross:
        debugMsDoc << "Unsupported shading pattern (0x" << hex << ipat << ")";
        return (255 - qRound(0.3 * 255));

    case ipatPctNew2:
        return (255 - qRound(0.025 * 255));
    case ipatPctNew7:
        return (255 - qRound(0.075 * 255));
    case ipatPctNew12:
        return (255 - qRound(0.125 * 255));
    case ipatPctNew15:
        return (255 - qRound(0.15 * 255));
    case ipatPctNew17:
        return (255 - qRound(0.175 * 255));
    case ipatPctNew22:
        return (255 - qRound(0.225 * 255));
    case ipatPctNew27:
        return (255 - qRound(0.275 * 255));
    case ipatPctNew32:
        return (255 - qRound(0.325 * 255));
    case ipatPctNew35:
        return (255 - qRound(0.35 * 255));
    case ipatPctNew37:
        return (255 - qRound(0.375 * 255));
    case ipatPctNew42:
        return (255 - qRound(0.425 * 255));
    case ipatPctNew45:
        return (255 - qRound(0.45 * 255));
    case ipatPctNew47:
        return (255 - qRound(0.475 * 255));
    case ipatPctNew52:
        return (255 - qRound(0.525 * 255));
    case ipatPctNew55:
        return (255 - qRound(0.55 * 255));
    case ipatPctNew57:
        return (255 - qRound(0.575 * 255));
    case ipatPctNew62:
        return (255 - qRound(0.625 * 255));
    case ipatPctNew65:
        return (255 - qRound(0.65 * 255));
    case ipatPctNew67:
        return (255 - qRound(0.675 * 255));
    case ipatPctNew72:
        return (255 - qRound(0.725 * 255));
    case ipatPctNew77:
        return (255 - qRound(0.775 * 255));
    case ipatPctNew82:
        return (255 - qRound(0.825 * 255));
    case ipatPctNew85:
        return (255 - qRound(0.85 * 255));
    case ipatPctNew87:
        return (255 - qRound(0.875 * 255));
    case ipatPctNew92:
        return (255 - qRound(0.925 * 255));
    case ipatPctNew95:
        return (255 - qRound(0.95 * 255));
    case ipatPctNew97:
        return (255 - qRound(0.975 * 255));
    default:
        debugMsDoc << "Unsupported shading pattern (0x" << hex << ipat << ")";
        *ok = false;
        return 0;
    }
} //ditheringToGray

void Conversion::setColorAttributes(QDomElement& element, int ico, const QString& prefix, bool defaultWhite)
{
    QColor color = Conversion::color(ico, -1, defaultWhite);
    element.setAttribute(prefix.isNull() ? QString::fromLatin1("red") : prefix + "Red", color.red());
    element.setAttribute(prefix.isNull() ? QString::fromLatin1("blue") : prefix + "Blue", color.blue());
    element.setAttribute(prefix.isNull() ? QString::fromLatin1("green") : prefix + "Green", color.green());
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
    debugMsDoc << "brc.brcType      = " << brc.brcType;
    debugMsDoc << "brc.dptLineWidth = " << brc.dptLineWidth;
    debugMsDoc << "brc.cv           = " << brc.cv;


    //set the border width
    qreal w;
    // this is according to http://msdn.microsoft.com/en-us/library/dd907496.aspx
    if (brc.brcType < 0x40) {
        w = brc.dptLineWidth / 8.0;
    } else {
        w = brc.dptLineWidth;
    }

    QString style("solid");   //reasonable default
    QString color = '#' + QString::number(brc.cv | 0xff000000, 16).right(6).toUpper();

    switch (brc.brcType) {
    case BorderNone:
        //Q_ASSERT( brc.dptLineWidth == 0 ); // otherwise words will show a border!
        style = "none";
        break;
    case BorderThinThickSmallGap:
    case BorderThickThinSmallGap:
        style = "double";
        w *= 1.5;
        break;
    case BorderThinThickLargeGap:
    case BorderThickThinLargeGap:
        style = "double";
        w *= 1.75;
        break;
    case BorderThinThickMediumGap:
    case BorderThickThinMediumGap:
        style = "double";
        w *= 2.0;
        break;
    case BorderDouble:
        style = "double";
        w *= 3;
        break;
    case BorderThin: //"hairline"
        w = 0.01;
        break;

        //ODF doesn't support dot dashed or wavy borders???

    case BorderDashed:
    case BorderDashSmallGap:
        style = "dashed"; // Words: dashes //FIXME:
        break;
    case BorderDotted:
        style = "dotted";
        break;
    case BorderDotDash:
        style = "dashed"; //FIXME:
        break;
    case BorderDotDotDash:
        style = "dashed"; //FIXME:
        break;
    case BorderWave:
        w *= 4; // Note: we can't make a wave but at least we can make it just as wide
        break;
    case BorderDoubleWave:
        w *= 6.25;
        style = "double"; // Note: we can't make a wave but at least we can make it just as wide
        break;
    case BorderTriple:
        w *= 5;
        style = "double"; //Note: odf only support double so that will have to do
        break;
    //FIXME: BorderInset = 0x1B (27)
    case 25: // inset
        style = "inset";
        break;
    case BorderThinThickThinSmallGap:
    case BorderThinThickThinMediumGap:
    case BorderThinThickThinLargeGap:
    default:
        //if a fancy unsupported border is specified -> better a normal border
        //than none so just leave values as defaults
        break;
    }

    QString width =  QString::number(w,'f') + "pt";

    QString value(width);
    value.append(" ");
    value.append(style);
    value.append(" ");
    value.append(color);

    return value;
}
//get a  calligra:borderspecial value "style"
QString Conversion::borderCalligraAttributes(const wvWare::Word97::BRC& brc)
{
    debugMsDoc << "brc.brcType      = " << brc.brcType;
    debugMsDoc << "brc.dptLineWidth = " << brc.dptLineWidth;
    debugMsDoc << "brc.cv           = " << brc.cv;


    QString style;   //empty if nothing special is neededreasonable default

    switch (brc.brcType) {
        //ODF doesn't support dot dashed or wavy borders???

    case 7: // dash large gap
        style = "dash-largegap";
        break;
    case 8: // dot dash
        style = "dot-dash";
        break;
    case 9: // dot dot dash
        style = "dot-dot-dash";
        break;

    case 20: // wave
        style = "wave";
        break;
    case 21: // double wave
        style = "double-wave";
        break;
    case 23: // slash
        style = "slash";
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
    case 6: // numbered (one, two, three) - not supported by Words
    case 7: // ordinal (first, second, third) - not supported by Words
    case 22: // leading zero (01-09, 10-99, 100-...) - not supported by Words
    case 0: // arabic
        value = '1';
        break;
    default:
        warnMsDoc << "Unknown NFC: " << nfc;
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
    //debugMsDoc <<" hasEvenOdd=" << hasEvenOdd;
    if (hasFirst)
        return hasEvenOdd ? 1 : 2;
    return hasEvenOdd ? 3 : 0;
}

int Conversion::headerMaskToFType(unsigned char mask)
{
    bool hasFirst = (mask & wvWare::HeaderData::FooterFirst);
    bool hasEvenOdd = (mask & wvWare::HeaderData::FooterEven);
    // Odd is always there. We have even!=odd only if Even is there too.
    debugMsDoc << " hasEvenOdd=" << hasEvenOdd;
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
    case 60:    m_fieldType =  2; break;  // username <-> Words's author name
    case 61:    m_fieldType = 16; break;  // userinitials <-> Words's author initial)
    case 62:    m_fieldType = -1; break;  // useraddress (unhandled)
    default:    m_fieldType = -1; break;
    }

    if (m_fieldType < 0)
        debugMsDoc << "unhandled field: fld.ftl:" << (int)fld->flt;

    return m_fieldType;
}

qreal Conversion::twipsToMM(int twips)
{
    qreal mm = twipsToInch(twips) * 25.4;
    return mm;
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

const char* Conversion::fpcToFtnPosition(quint16 fpc)
{
    switch (fpc) {
    case 0:
        return "section";
    case 2:
        return "text";
    case 1:
    default:
        return "page";
    }
}

const char* Conversion::rncToStartNumberingAt(quint16 rnc)
{
    switch (rnc) {
    case 0:
        return "document";
    case 1:
        return "section";
    case 2:
    default:
        return "page";
    }
}

const char* Conversion::getHorizontalPos(qint16 dxaAbs)
{
    // [MS-DOC] — v20101219, sprmPDxaAbs:
    // (-4) center, (-8) right, (-12) inside, (-16) outside
    switch (dxaAbs) {
    case (-4):
        return "center";
    case (-8):
        return "right";
    case (-12):
        return "inside";
    case (-16):
        return "outside";
    default:
        return "from-left";
    }
}

const char* Conversion::getHorizontalRel(uint pcHorz)
{
    // [MS-DOC] — v20101219:
    // 0 - current column, 1 - margin, 2 - page
    switch (pcHorz) {
    case 0:
        return "paragraph";
    case 1:
        return "page-content";
    case 2:
        return "page";
    default:
        return "";
    }
}

const char* Conversion::getVerticalPos(qint16 dyaAbs)
{
    // [MS-DOC] — v20101219, sprmPDyaAbs:
    // (-4) top, (-8) middle, (-12) bottom, (-16) inside, (-20) outside
    switch (dyaAbs) {
    case (-4):
        return "top";
    case (-8):
        return "middle";
    case (-12):
        return "bottom";
    case (-16):
        return "inline";
    case (-20):
        return "inline";
    default:
        return "from-top";
    }
}

const char* Conversion::getVerticalRel(uint pcVert)
{
    // [MS-DOC] — v20101219:
    // 0 - margin, 1 - page, 2 - paragraph
    switch (pcVert) {
    case 0:
        return "page-content";
    case 1:
        return "page";
    case 2:
        return "paragraph";
    default:
        return "";
    }
}
