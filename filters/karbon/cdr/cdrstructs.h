/* This file is part of the Calligra project, made within the KDE community.

   Copyright 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef CDRSTRUCTS_H
#define CDRSTRUCTS_H

// Qt
#include <QtCore/QtGlobal>
#include <QtCore/QPoint>

// TODO: make sure conversion data is alright
//      cdr    mm     ko (points)
// w    8268   210.0  ?
// h    11694  297.0  ?
#define xCDR_TO_POINT(cdr) (static_cast<qreal>(cdr)*0.071997656)
#define yCDR_TO_POINT(cdr) (static_cast<qreal>(cdr)*0.071993434)

// CDR formats

// type of object
enum CdrObjectIds
{
// type 00: "Ebene1"
// type 01: rectangle
    CdrRectangleObjectId = 0x01,
// type 02: ellipse (?)
    CdrEllipseObjectId = 0x02,
// type 03: line and curve
    CdrPathObjectId = 0x03,
// type 04: text
    CdrTextObjectId = 0x04,
// type 05: bitmap (?)
    CdrBitmapObjectId = 0x05,
// type 06: 
// type 0B: "Gitter"
// type 0C: "Hilfslinien"
// type 11: "Desktop"
};

typedef qint16 Cdr4Coord;

struct Cdr4Point
{
    // 0..1
    Cdr4Coord mX;
    // 2..3
    Cdr4Coord mY;
};

struct Cdr4BoundingBox
{
    // 0..3
    Cdr4Point mUpperLeft;
    // 4..7
    Cdr4Point mLowerRight;
};

inline qreal
koXCoord( Cdr4Coord cdrCoord )
{
    return xCDR_TO_POINT(static_cast<qreal>(cdrCoord));
}

inline qreal
koYCoord( Cdr4Coord cdrCoord )
{
    return yCDR_TO_POINT(static_cast<qreal>(cdrCoord));
}


inline QPointF
koCoords( Cdr4Point cdrCoords )
{
    return QPointF( koXCoord(cdrCoords.mX), koYCoord(cdrCoords.mY) );
}

struct CdrArgumentData
{
    // TODO: convert endianness on bigendian system
    // void convertToBugEndian();

    const quint16* argOffsets() const
    { return reinterpret_cast<const quint16*>( reinterpret_cast<const char*>(this)+startOfArgs ); }

    template<typename T>
    T arg( int i ) const
    { return *reinterpret_cast<const T*>( reinterpret_cast<const char*>(this)+argOffsets()[i] ); }
    template<typename T>
    const T* argPtr( int i ) const
    { return reinterpret_cast<const T*>( reinterpret_cast<const char*>(this)+argOffsets()[i] ); }

    // 0..1: size of data
    quint16 chunkLength;
    // 2..3: number of arguments
    quint16 count;
    // 4..5: start of arguments
    quint16 startOfArgs;
};


struct CdrArgumentWithTypeData : public CdrArgumentData
{
    // TODO: convert endianness on bigendian system
    // void convertToBugEndian();

    const quint16* argTypes() const
    { return reinterpret_cast<const quint16*>( reinterpret_cast<const char*>(this)+startOfArgTypes ); }

    // order of types seems inverted...
    quint16 argType( int i ) const { return argTypes()[count-i-1]; }

    // 6..7: start of arguments types
    quint16 startOfArgTypes;
};

struct CdrObjectArgumentData : public CdrArgumentWithTypeData
{
    // TODO: convert endianness on bigendian system
    // void convertToBugEndian();

    // 8..9: type of loda
    quint16 chunkType;
};

struct CdrStyleArgumentData : public CdrArgumentWithTypeData
{
    // TODO: convert endianness on bigendian system
    // void convertToBugEndian();

    // 8..9: ?
    qint16 _unknown0;
    // 10..11: ?
    qint16 _unknown1;
    // 12..13: ?
    qint16 _unknown2;
    // 14..15: ?
    qint16 _unknown3;
    // 16..17: ?
    qint16 _unknown4;
};


struct CdrTrflArgumentData : public CdrArgumentData
{
    // 6..7: seen FF FF
    qint16 _unknown0;
    // 8..9: seen 00 00
    qint16 _unknown1;
};

// 0D 00 16 01 01 00 - also size, bold, italic etc. ?
struct CdrStyleFontArgumentData
{
    // 0..1: font index
    quint16 mFontIndex;
    // 2..5: ?
    quint8 _unknown0;
    quint8 _unknown1;
    quint8 _unknown2;
    quint8 _unknown3;
};

// 01 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// 02 00 00 00 02 00 00 00 00 00 03 00 64 00 00 00 01 00 00 00 00 00 00 00 64 00 00 00 00 00 00 00 C2 01 3C 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

// 01 00 00 00 01 00 00 00 00 00 0E 00 64 00 00 00 01 00 00 00 00 00 00 00 64 00 00 00 00 00 00 00 C2 01 3C 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// 02 00 00 00 02 00 00 00 00 00 03 00 64 00 00 00 01 00 00 00 00 00 00 00 64 00 00 00 00 00 00 00 C2 01 3C 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// 03 00 00 00 01 00 00 00 00 00 03 00 64 00 00 00 01 00 00 00 00 00 00 00 64 00 00 00 00 00 00 00 C2 01 3C 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// 04 00 00 00 32 00 00 00 00 00 4A 00 0A 00 8E FE 01 00 00 00 00 00 00 00 64 00 00 00 00 00 00 00 C2 01 3C 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// 05 00 00 00 32 00 00 00 00 00 4A 00 0A 00 8E FE 01 00 00 00 00 00 00 00 32 00 00 00 00 00 00 00 C2 01 3C 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// 06 00 00 00 32 00 00 00 00 00 1E 00 0A 00 8E FE 01 00 00 00 00 00 00 00 3C 00 00 00 00 00 00 00 C2 01 3C 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// 07 00 00 00 32 00 00 00 00 00 1E 00 0A 00 8E FE 01 00 00 00 00 00 00 00 64 00 00 00 00 00 00 00 C2 01 3C 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//            |     |     |     |     |     |     |           |           |           |           |           |
// 0           4     6     8     10    12    14    16          20          24          28          32          36
// possible data: line type, line ending type, color
struct CdrOutlineData
{
    // 68 bytes:
    // 0..3: uint32 index/key/id? would match loda
    quint32 mIndex;
    // 4..5: type (?) seen 1, 2, 50
    quint16 mType;
    // 6..7: always 00 00
    quint16 _unknown1;
    // 8..9: always 00 00
    quint16 _unknown1;
    // 10..11: line width? seen 0, 3, 14, 30, 74
    quint16 _unknown2;
    // 12..13: seen 0, 10, 100
    quint16 _unknown3;
    // 14..15: seen 0, -370
    quint16 _unknown4;
    // 16..19: seen 0, 1
    quint32 _unknown5;
    // 20..23: seen 0
    quint32 _unknown6;
    // 24..27: seen 0, 50, 60, 100
    quint32 _unknown7;
    // 28..31: seen 0
    quint32 _unknown8;
    // 32..35: seen 0, C2 01 3C 00 (3932610 oder 450,60 oder 194,1,60,0)
    quint32 _unknown9;
    // 36..67: seen 0
    char _data[32];
};

enum CdrFillType { CdrTransparent = 0, CdrSolid };

// 01 00 00 00 00 00 00 00 64 00 00 00 00 00 00 00 C2 01 3C 00 00 00
// 01 00 00 00 00 00 00 00 14 00 00 00 00 00 00 00 C2 01 3C 00 00 00
// 01 00 00 00 00 00 00 00 3C 00 00 00 00 00 00 00 C2 01 3C 00 00 00
// 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 C2 01 3C 00 00 00

// 01 00 00 00 00 00 00 00 64 00 00 00 00 00 00 00 C2 01 3C 00 00 00
// 01 00 00 00 00 00 00 00 64 00 00 00 00 00 00 00 00 00 00 00 00 00
// 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//            |           |           |           |           |
// similar to CdrOutlineData from 16..
// possible data: color
struct CdrSolidFillData
{
    // 0..3: seen 1
    quint32 _unknown1;
    // 4..7: seen 0
    quint32 _unknown2;
    // 8..11: seen 0, 20, 60, 100
    quint32 _unknown3;
    // 12..15: seen 0
    quint32 _unknown4;
    // 16..19: seen 0, C2 01 3C 00 (3932610 oder 450,60 oder 194,1,60,0)
    quint32 _unknown5;
};

// sample data: AC 2D B6 DF 00 00
struct Cdr4RectangleData
{
    // 0..1: width?
    quint16 mWidth;
    // 2..3: height?
    quint16 mHeight;
    // 4..5: ? rounded corners?
    quint16 _unknown;
};

// sample data: 4F FA 28 FC 8C 0A 8C 0A 00 00
struct Cdr4EllipseData
{
    // 0..3: center?
    Cdr4Point mCenterPoint;
    // 4..5: x radius?
    quint16 mXRadius;
    // 6..7: y radius?
    quint16 mYRadius;
    // 8..9: ?
    quint16 _unknown;
};

// point types:
// 0C
// 44
// 48
// 64
// C0
// 84
// first point has 0C (or 0D)
// last point has 48 (or 49)
// a break in the line are points with 48 0C
// but there is also C0 84, difference is?
// lots of connected straight lines seem 44 44 44
// no idea about this example yet:
// -63.5019 , 115.261 : "c" 
// 19.6554 , 47.2277 : "44" 
// 114.116 , 98.271 : "64" 
// 114.116 , 98.271 : "c0" 
// 42.3346 , 188.911 : "c0" 
// 42.3346 , 188.911 : "84" 
// -63.5019 , 115.261 : "48" 

// looks like flags, a nibble per direction?
// 0  0000
// 4  0100
// 6  0110
// 8  1000
// 9  1001
// C  1100
// D  1101
typedef unsigned char PointType;
enum PointFlags {
    UnknownPointFlag = 1 << 0,
    LineCurved       = 1 << 1, //(?)
    LineStart        = 1 << 2,
    UnknownPointFlag2 = 1 << 3
};

struct Cdr4PointList
{
    Cdr4Point point( int i ) const
    { return (&firstPoint)[i]; }
    // TODO: check that type is not inverted like arg types are
    PointType pointType( int i ) const
    { return reinterpret_cast<const PointType*>(&firstPoint+count)[i]; }

public:
    // 0..1: number of points
    quint16 count;
    // 2..3: ?
    quint16 _unknown;
private:
    // 4..4+num*4: num of 16bit coord pairs
    Cdr4Point firstPoint; // used to get pointer in pointType(...)
    // 4+num*4+1..4+num*5: point types (inverted?)
    // seems types are padded to full 32 bit slots, but no need to care while reading
};

struct Cdr4CharData
{
    quint8 _unknown;
    char mChar;
    quint8 _unknown2;
};

struct Cdr4TextData
{
    Cdr4CharData charData( int i ) const
    { return (&firstChar)[i]; }

public:
    // 0..1: ? is 00 00 in samples
    quint16 _unknown;
    // 2..3: length of text
    quint16 mLength;
    // 4..5: size of complete data
    quint16 mDataSize;
    // now the chars, have two \0 bytes between them,
    // one before the first, not sure if the first has meaning or belongs to char
    // assume this for now, see CharData
    // "ä" was seen, so encoding is local one (was 8-bit one for me, visible with iso-8859-1)
    // linebreak is with 0D 0A chars
private:
    // 4..4+num*4: num of 16bit coord pairs
    Cdr4CharData firstChar; // used to get pointer in pointType(...)
};

// possible transformations:
// translate: needs two coords, qint16 -> 4 byte
// scale: needs two scale, qint16 -> 4 byte
// rotate: needs center coords + angle -> 4+x byte
// skewX: needs angle -> x byte
// skewY: needs angle -> x byte
// sample: 08 00 00 00 01 00 00 00 00 00 04 EC FF FF 00 00 00 00 00 00 01 00 13 0E 00 00
// sample: 08 00 29 B6 05 00 86 6A FE FF 83 F7 FF FF 7A 95 01 00 29 B6 05 00 6F FC FF FF
// sample: 08 00 29 B6 05 00 86 6A FE FF D6 F7 FF FF 7A 95 01 00 29 B6 05 00 34 FC FF FF
// sample: 08 00 1A EF 00 00 00 00 00 00 D8 F0 FF FF 00 00 00 00 00 00 01 00 8B 0B 00 00
// sample: 08 00 00 00 01 00 00 00 00 00 2A E9 FF FF 00 00 00 00 00 00 01 00 25 10 00 00
// sample: 08 00 00 00 01 00 00 00 00 00 3E 04 00 00 00 00 00 00 00 00 01 00 DF 03 00 00
// sample: 08 00 00 00 01 00 00 00 00 00 87 F3 FF FF 00 00 00 00 00 00 01 00 98 F8 FF FF
//              |     |     |           |           |           |     |     |           |
// Also found this in a list of data, size 72 bytes:
// sample: 02 00 01 00 00 00 BC F3 13 08 FB 0F E9 F7 BC F3 E9 F7 BC F3 13 08 FB 0F 13 08 FB 0F E9 F7 BC F3 E9 F7 E8 1C 00 00 00 00 80 84 2E 41 00 00 00 00 00 00 00 C0 D0 1C 00 00 00 00 00 B8 7D 40 00 00 00 00 80 84 2E 41 00 00
// for type 3 number of trfd args: 2, 15, 1, 12, in sync with arg type 10 (outline index?)
struct TransformData
{
    char _unknown[26];
};

// CDR4 has 54 bytes, CDR5 56 bytes (const structure?)
struct MCfgData
{
    // 0..1: width
    quint16 width;
    // 2..3: height
    quint16 height;
};

// 0000:0020 |       61 00  01 00 00 00  00 00 00 00  00 00 00 00 |   a.............
// 0000:0030 | 04 00 C2 00  98 1F                                 | ..Â...
struct LnkData
{
    // 0..19: unknown
    char unknown[20];
};

#endif
