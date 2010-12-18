/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>
   Copyright (C) 2008 Benjamin Cail <cricketc@gmail.com>

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

#ifndef CONVERSION_H
#define CONVERSION_H

#include <wv2/src/ustring.h>
#include <QString>
#include <QColor>

class QDomElement;
namespace wvWare
{
namespace Word97 {
class LSPD;
class BRC;
}
class FLD;
}

// Static methods for simple MSWord->KWord conversions
// (enums etc.)

namespace Conversion
{
    const int MS_SYMBOL_ENCODING[256] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        32, 33, 8704, 35, 8707, 37, 38, 8715, 40, 41, 8727, 43, 44, 8722, 46, 47,
        48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
        8773, 913, 914, 935, 916, 917, 934, 915, 919, 921, 977, 922, 923, 924, 925, 927,
        928, 920, 929, 931, 932, 933, 962, 937, 926, 936, 918, 91, 8756, 93, 8869, 95,
        8254, 945, 946, 967, 948, 949, 966, 947, 951, 953, 981, 954, 955, 956, 957, 959,
        960, 952, 961, 963, 964, 965, 982, 969, 958, 968, 950, 123, 124, 125, 8764, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 978, 8242, 8804, 8725, 8734, 402, 9827, 9830, 9829, 9824, 8596, 8592, 8593, 8594, 8595,
        176, 177, 8243, 8805, 215, 8733, 8706, 8729, 247, 8800, 8801, 8776, 8943, 0, 0, 8629,
        0, 8465, 8476, 8472, 8855, 8853, 8709, 8745, 8746, 8835, 8839, 8836, 8834, 8838, 8712, 8713,
        8736, 8711, 174, 169, 8482, 8719, 8730, 8901, 172, 8743, 8744, 8660, 8656, 8657, 8658, 8659,
        9674, 9001, 0, 0, 0, 8721, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 9002, 8747, 8992, 0, 8993, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
// UString -> QString conversion. 
inline QString string(const wvWare::UString& str)
{
    // Do a deep copy.  We used to not do that, but it lead to several
    // memory corruption bugs that were difficult to find.
    //
    // FIXME: Get rid of UString altogether and port wv2 to QString.
    return QString(reinterpret_cast<const QChar*>(str.data()), str.length());
}

//special version of string() that replaces spaces with _20_,
//since style names can't have spaces in them
QString styleNameString(const wvWare::UString& str);

// Prepare text for inclusion in XML
void encodeText(QString &text);

// Convert color code (ico) to QString with hex code
QString color(int number, int defaultcolor, bool defaultWhite = false);

// Convert pattern style (ipat) to QBrush::BrushStyle
int fillPatternStyle(int ipat);

// Hackery for gray levels
int ditheringToGray(int ipat, bool* ok);

// convert shading pattern (ipat) to RGB color
uint shadingPatternToColor(int ipat);

// Convert alignment code to string
//QString alignment( int jc );

// Convert linespacing struct to string
QString lineSpacing(const wvWare::Word97::LSPD& lspd);

// Convert number format code to a value for the odt style:num-format attribute
QString numberFormatCode(int nfc);

// Set the 3 color attributes in the XML element, from a color code (ico)
// prefix: if empty, the attribute names will be red/blue/green
// if not empty, they will be xRed/xBlue/xGreen
void setColorAttributes(QDomElement& element, int ico, const QString& prefix = QString(), bool defaultWhite = false);

// Return an ODF attribute for border linewidths from a BRC structure
QString setDoubleBorderAttributes(const wvWare::Word97::BRC& brc);

// Return an ODF attribute for border characteristics from a BRC structure
QString setBorderAttributes(const wvWare::Word97::BRC& brc);

//get a  koffice:borderspecial value "style"
QString borderKOfficeAttributes(const wvWare::Word97::BRC& brc);

// Convert wv2's Header Type to a KWord frameInfo value
int headerTypeToFrameInfo(unsigned char type);

// Convert wv2's Header Type to a KWord frameset name
QString headerTypeToFramesetName(unsigned char type);

// Convert a mask of Header Types to the hType value for KWord
int headerMaskToHType(unsigned char mask);

// Convert a mask of Header Types to the fType value for KWord
int headerMaskToFType(unsigned char mask);

// Convert wv2's FLD to KWord FIELD.subtype (or -1 if can't be handled)
int fldToFieldType(const wvWare::FLD* fld);

bool isHeader(unsigned char type);

qreal twipsToInch(int twips);

qreal twipsToPt(int twips);

QString rncToStartNumberingAt(int rnc);

}

#endif
