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
 * Boston, MA 02110-1301, USA.
*/

#ifndef CDRSTRUCTS_H
#define CDRSTRUCTS_H

// Qt
#include <QtCore/QtGlobal>

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
// type 01: rectangle(?)
    CdrRectangleObjectId = 0x01,
// type 02: ellipse (?)
    CdrEllipseObjectId = 0x02,
// type 03: line and curve (?)
    CdrPathObjectId = 0x03,
// type 04: text (?)
    CdrTextObjectId = 0x04,
// type 05: bitmap (?)
    CdrBitmapObjectId = 0x05,
// type 06: 
// type 0B: "Gitter"
// type 0C: "Hilfslinien"
// type 11: "Desktop"
};

typedef qint16 cdr4Coord;

struct Cdr4Point
{
    cdr4Coord mX;
    cdr4Coord mY;
};

struct CdrArgumentData
{
    // TODO: convert endianness on bigendian system
    // void convertToBugEndian();

    const quint16* argOffsets() const
    { return reinterpret_cast<const quint16*>( reinterpret_cast<const char*>(this)+startOfArgs ); }
    const quint16* argTypes() const
    { return reinterpret_cast<const quint16*>( reinterpret_cast<const char*>(this)+startOfArgTypes ); }

    // order of types seems inverted...
    quint16 argType( int i ) const { return argTypes()[count-i-1]; }

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
    // 6..7: start of arguments types
    quint16 startOfArgTypes;
    // 8..9: type of loda
    quint16 chunkType;
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

struct TransformData
{
    quint32 factor[6];
    quint16 _unknown;
};

// CDR4 has 54 bytes, CDR5 56 bytes (const structure?)
struct MCfgData
{
    // 0..1: width
    quint16 width;
    // 2..3: height
    quint16 height;
};

#endif
