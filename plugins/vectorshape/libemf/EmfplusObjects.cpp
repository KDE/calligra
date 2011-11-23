/*
  Copyright 2011 Inge Wallin <inge@lysator.liu.se>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either 
  version 2.1 of the License, or (at your option) any later version.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public 
  License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

// Own
#include "EmfplusObjects.h"

// Qt

// KDE
#include <KDebug>

// libemf
#include "EmfplusEnums.h"

/**
   \file

   Objects used in various parts of EMFPLUS parser.
*/



using namespace Libwmf;
using namespace Libemf;

/**
   Namespace for Enhanced Metafile (EMF) classes
*/
namespace Libemf
{


static void soakBytes(QDataStream &stream, int numBytes)
{
    quint8 scratch;
    for (int i = 0; i < numBytes; ++i) {
        stream >> scratch;
    }
}



// ----------------------------------------------------------------
// 2.2.1.1 EmfPlusBrush


EmfPlusBrush::EmfPlusBrush(QDataStream &stream, quint32 size)
    : version(stream)
{
    stream >> type;
    switch (type) {
    case BrushTypeSolidColor:
        break;
    case BrushTypeHatchFill:
        break;
    case BrushTypeTextureFill:
        break;
    case BrushTypePathGradient:
        break;
    case BrushTypeLinearGradient:
        break;
    default:
        break;
    }
}


// ----------------------------------------------------------------
// 2.2.1.6 EmfPlusPath

QPainterPath  emfplusPathFromStream(QDataStream &stream, quint32 objectSize)
{
    EmfPlusGraphicsVersion  version(stream); // 4 bytes
    quint32  pathPointCount;                 // 4 bytes
    quint16  pathPointFlags;                 // 2 bytes
    quint16  reserved;                       // 2 bytes

    stream >> pathPointCount;
    stream >> pathPointFlags;
    stream >> reserved;

    bool  isCompressed          = (pathPointFlags & 0x02); // Ignore if isRelativeCoordinates set.
    bool  isRleEncoded          = (pathPointFlags & 0x08);
    bool  isRelativeCoordinates = (pathPointFlags & 0x10);

    QPainterPath  retval;

    kDebug(31000) << "Point count:" << pathPointCount
                  << "Flags: C" << isCompressed << "R" << isRleEncoded
                  << "P" << isRelativeCoordinates;

    // PathPoints;
    qreal lastX = qreal(0.0);
    qreal lastY = qreal(0.0);
    for (uint i = 0; i < pathPointCount; ++i) {
        qreal  xReal;
        qreal  yReal;
        if (isCompressed) {
            quint16 x;
            quint16 y;
            stream >> x >> y;
            xReal = qreal(x);
            yReal = qreal(y);
        }
        else {
            stream >> xReal >> yReal;
        }

        // Handle relative coordinates or not.
        if (isRelativeCoordinates) {
            lastX += xReal;
            lastY += yReal;
            retval.lineTo(QPointF(lastX, lastY));
        }
        else {
            retval.lineTo(QPointF(xReal, yReal));
            lastX = xReal;
            lastY = yReal;
        }
    }


#if 1 // Path point types not yet implemented
    soakBytes(stream, objectSize - 12 - 2 * pathPointCount * (isCompressed ? 2 : 4));
#else
    // Path point types
    if (isRleEncoded) {
        // RLE encoding Not yet implemented
        soakBytes(stream, recordSize - 12);
    }
    else {
        
    }
#endif

    kDebug(31000) << "Returns:" << retval;
    return retval;
}


// ----------------------------------------------------------------
// 2.2.2.19 EmfPlusGraphicsVersion

/**
   EmfPlusGraphicsVersion Object

   The EmfPlusGraphicsVersion object specifies the version of
   operating system graphics that is used to create an EMF+ metafile.

   See [MS-EMFPLUS] 2.2.2.19
*/

EmfPlusGraphicsVersion::EmfPlusGraphicsVersion(QDataStream &stream)
{
    quint32  temp;
    stream >> temp;

    metafileSignature = temp & 0x0fffff;       // 20 bits
    graphicsVersion   = (temp >> 20) & 0x0fff; // 12 bits
}


/**
   EmfPlusRect Object

   The EmfPlusRect object specifies a rectangle origin, height, and
   width as 16-bit signed integers.

   See [MS-EMFPLUS] 2.2.2.38
*/

QRectF emfPlusRectFromStream(QDataStream &stream)
{
    quint16 x;
    quint16 y;
    quint16 width;
    quint16 height;

    stream >> x;
    stream >> y;
    stream >> width;
    stream >> height;

    return QRect(x, y, width, height);
}


/**
   BlurEffect Object

   The BlurEffect object specifies a decrease in the difference in
   intensity between pixels in an image.

   See [MS-EMFPLUS] 2.2.3.1
*/

BlurEffect::BlurEffect( QDataStream &stream ) 
{
    stream >> blurRadius;
    stream >> expandEdge;
}


}
