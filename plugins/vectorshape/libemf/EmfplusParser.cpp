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
#include "EmfplusParser.h"

// Qt
#include <QColor>
#include <QFile>
#include <QBuffer>

// KDE
#include <KDebug>

// LibEmf
#include "EmfplusEnums.h"
#include "EmfplusObjects.h"
#include "EmfDeviceContext.h"
//#include "EmfplusDeviceContext.h"
//#include "EmfRecords.h"
//#include "EmfObjects.h"


// 0 - No debug
// 1 - Print a lot of debug info
// 2 - Just print all the records instead of parsing them
#define DEBUG_EMFPLUSPARSER 1


namespace Libemf
{


static void soakBytes(QDataStream &stream, int numBytes)
{
    quint8 scratch;
    for (int i = 0; i < numBytes; ++i) {
        stream >> scratch;
    }
}

static void outputBytes(QDataStream &stream, int numBytes)
{
    quint8 scratch;
    for ( int i = 0; i < numBytes; ++i ) {
        stream >> scratch;
	//qDebug("byte(%i):%c", i, scratch);
    }
}


// ================================================================


EmfplusParser::EmfplusParser()
    : m_backend(0)
{
}

EmfplusParser::~EmfplusParser()
{
}


bool EmfplusParser::parse(QDataStream &stream, EmfDeviceContext &context,
                          EmfAbstractBackend *backend) 
{
#if DEBUG_EMFPLUSPARSER
    kDebug(31000) << "================ Starting EMFPLUS";
#endif

    m_backend = backend;

    while (!stream.atEnd()) {
        if (!parseRecord(stream, context)) {
            return false;
        }
    }

    return true;
}

/**
   The RecordType enumeration defines record types used in EMF+ metafiles.

   See [MS-EMFPLUS] Section 2.1.1.1 for more information.
*/
enum RecordType {
    EMFPLUS_FIRSTRECORD            = 0x4000,

    EmfPlusHeader                  = 0x4001,
    EmfPlusEndOfFile               = 0x4002,
    EmfPlusComment                 = 0x4003,
    EmfPlusGetDC                   = 0x4004,
    EmfPlusMultiFormatStart        = 0x4005,
    EmfPlusMultiFormatSection      = 0x4006,
    EmfPlusMultiFormatEnd          = 0x4007,
    EmfPlusObject                  = 0x4008,
    EmfPlusClear                   = 0x4009,
    EmfPlusFillRects               = 0x400A,
    EmfPlusDrawRects               = 0x400B,
    EmfPlusFillPolygon             = 0x400C,
    EmfPlusDrawLines               = 0x400D,
    EmfPlusFillEllipse             = 0x400E,
    EmfPlusDrawEllipse             = 0x400F,
    EmfPlusFillPie                 = 0x4010,
    EmfPlusDrawPie                 = 0x4011,
    EmfPlusDrawArc                 = 0x4012,
    EmfPlusFillRegion              = 0x4013,
    EmfPlusFillPath                = 0x4014,
    EmfPlusDrawPath                = 0x4015,
    EmfPlusFillClosedCurve         = 0x4016,
    EmfPlusDrawClosedCurve         = 0x4017,
    EmfPlusDrawCurve               = 0x4018,
    EmfPlusDrawBeziers             = 0x4019,
    EmfPlusDrawImage               = 0x401A,
    EmfPlusDrawImagePoints         = 0x401B,
    EmfPlusDrawString              = 0x401C,
    EmfPlusSetRenderingOrigin      = 0x401D,
    EmfPlusSetAntiAliasMode        = 0x401E,
    EmfPlusSetTextRenderingHint    = 0x401F,
    EmfPlusSetTextContrast         = 0x4020,
    EmfPlusSetInterpolationMode    = 0x4021,
    EmfPlusSetPixelOffsetMode      = 0x4022,
    EmfPlusSetCompositingMode      = 0x4023,
    EmfPlusSetCompositingQuality   = 0x4024,
    EmfPlusSave                    = 0x4025,
    EmfPlusRestore                 = 0x4026,
    EmfPlusBeginContainer          = 0x4027,
    EmfPlusBeginContainerNoParams  = 0x4028,
    EmfPlusEndContainer            = 0x4029,
    EmfPlusSetWorldTransform       = 0x402A,
    EmfPlusResetWorldTransform     = 0x402B,
    EmfPlusMultiplyWorldTransform  = 0x402C,
    EmfPlusTranslateWorldTransform = 0x402D,
    EmfPlusScaleWorldTransform     = 0x402E,
    EmfPlusRotateWorldTransform    = 0x402F,
    EmfPlusSetPageTransform        = 0x4030,
    EmfPlusResetClip               = 0x4031,
    EmfPlusSetClipRect             = 0x4032,
    EmfPlusSetClipPath             = 0x4033,
    EmfPlusSetClipRegion           = 0x4034,
    EmfPlusOffsetClip              = 0x4035,
    EmfPlusDrawDriverstring        = 0x4036,
    EmfPlusStrokeFillPath          = 0x4037,
    EmfPlusSerializableObject      = 0x4038,
    EmfPlusSetTSGraphics           = 0x4039,
    EmfPlusSetTSClip               = 0x403A,

    EMFPLUS_LASTRECORD
};



static const struct {
    int  recordType;
    QString name;
} EmfplusRecords[] = {
    { 0x0000, "NULL" },

    { 0x4001, "EmfPlusHeader" }, 
    { 0x4002, "EmfPlusEndOfFile" }, 
    { 0x4003, "EmfPlusComment" }, 
    { 0x4004, "EmfPlusGetDC" }, 
    { 0x4005, "EmfPlusMultiFormatStart" }, 
    { 0x4006, "EmfPlusMultiFormatSection" }, 
    { 0x4007, "EmfPlusMultiFormatEnd" }, 
    { 0x4008, "EmfPlusObject" }, 
    { 0x4009, "EmfPlusClear" }, 
    { 0x400A, "EmfPlusFillRects" }, 
    { 0x400B, "EmfPlusDrawRects" }, 
    { 0x400C, "EmfPlusFillPolygon" }, 
    { 0x400D, "EmfPlusDrawLines" }, 
    { 0x400E, "EmfPlusFillEllipse" }, 
    { 0x400F, "EmfPlusDrawEllipse" }, 
    { 0x4010, "EmfPlusFillPie" }, 
    { 0x4011, "EmfPlusDrawPie" }, 
    { 0x4012, "EmfPlusDrawArc" }, 
    { 0x4013, "EmfPlusFillRegion" }, 
    { 0x4014, "EmfPlusFillPath" }, 
    { 0x4015, "EmfPlusDrawPath" }, 
    { 0x4016, "EmfPlusFillClosedCurve" }, 
    { 0x4017, "EmfPlusDrawClosedCurve" }, 
    { 0x4018, "EmfPlusDrawCurve" }, 
    { 0x4019, "EmfPlusDrawBeziers" }, 
    { 0x401A, "EmfPlusDrawImage" }, 
    { 0x401B, "EmfPlusDrawImagePoints" }, 
    { 0x401C, "EmfPlusDrawString" }, 
    { 0x401D, "EmfPlusSetRenderingOrigin" }, 
    { 0x401E, "EmfPlusSetAntiAliasMode" }, 
    { 0x401F, "EmfPlusSetTextRenderingHint" }, 
    { 0x4020, "EmfPlusSetTextContrast" }, 
    { 0x4021, "EmfPlusSetInterpolationMode" }, 
    { 0x4022, "EmfPlusSetPixelOffsetMode" }, 
    { 0x4023, "EmfPlusSetCompositingMode" }, 
    { 0x4024, "EmfPlusSetCompositingQuality" }, 
    { 0x4025, "EmfPlusSave" }, 
    { 0x4026, "EmfPlusRestore" }, 
    { 0x4027, "EmfPlusBeginContainer" }, 
    { 0x4028, "EmfPlusBeginContainerNoParams" }, 
    { 0x4029, "EmfPlusEndContainer" }, 
    { 0x402A, "EmfPlusSetWorldTransform" }, 
    { 0x402B, "EmfPlusResetWorldTransform" }, 
    { 0x402C, "EmfPlusMultiplyWorldTransform" }, 
    { 0x402D, "EmfPlusTranslateWorldTransform" }, 
    { 0x402E, "EmfPlusScaleWorldTransform" }, 
    { 0x402F, "EmfPlusRotateWorldTransform" }, 
    { 0x4030, "EmfPlusSetPageTransform" }, 
    { 0x4031, "EmfPlusResetClip" }, 
    { 0x4032, "EmfPlusSetClipRect" }, 
    { 0x4033, "EmfPlusSetClipPath" }, 
    { 0x4034, "EmfPlusSetClipRegion" }, 
    { 0x4035, "EmfPlusOffsetClip" }, 
    { 0x4036, "EmfPlusDrawDriverstring" }, 
    { 0x4037, "EmfPlusStrokeFillPath" }, 
    { 0x4038, "EmfPlusSerializableObject" }, 
    { 0x4039, "EmfPlusSetTSGraphics" }, 
    { 0x403A, "EmfPlusSetTSClip" }, 
};


bool EmfplusParser::parseRecord(QDataStream &stream, EmfDeviceContext &context)
{
    if (!m_backend) {
        qWarning() << "backend not set";
        return false;
    }

    quint16 type;
    quint16 flags;
    quint32 size;

    stream >> type;
    stream >> flags;
    stream >> size;

    {
        QString name;
        if (EMFPLUS_FIRSTRECORD < type && type < EMFPLUS_LASTRECORD)
            name = EmfplusRecords[type & 0xff].name;
        else
            name = "(out of bounds)";
#if DEBUG_EMFPLUSPARSER
        kDebug(31000) << "Record length" << size << "type" << hex << type << name
                      << "flags:" << flags << dec;
#endif
    }

#if DEBUG_EMFPLUSPARSER == 2
    soakBytes(stream, size - 8);
#else
    switch (type) {
        // ---------------- Control records ----------------
    case EmfPlusHeader:
        {
            quint32  dataSize; // redundant. size contains the same value + 12.
            quint32  version;  // version of OS that created this one.
            quint32  emfplusFlags; // flags.  Currently only reference device (irrelevant)
            quint32  logicalDpiX;  // resolution X, pixels per inch
            quint32  logicalDpiY;

            stream >> dataSize;
            stream >> version;
            stream >> emfplusFlags;
            stream >> logicalDpiX >> logicalDpiY;

            if (flags & 0x01)
                context.emfPlusMode = ModeEmfPlusDual;
            else
                context.emfPlusMode = ModeEmfPlusOnly;
            
        }
        break;
    case EmfPlusEndOfFile:
        {
            context.emfPlusMode = ModeEmf;
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusComment:
        {
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusGetDC:
        {
            context.emfPlusGetDCSeen = true;
            soakBytes(stream, size - 8);
        }
        break;

        // ----------------  ----------------

    case EmfPlusMultiFormatStart:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusMultiFormatSection:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusMultiFormatEnd:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusObject:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusClear:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusFillRects:
        {
            quint32  dataSize; 
            quint32  count;

            stream >> dataSize;

            QBrush  brush;
            if (flags & 0x01) {
                // BrushID is an ARGB color
                quint8  blue, red, green, alpha;
                stream >> blue; // Yes, this is the actual order(!).
                stream >> red;
                stream >> green;
                stream >> alpha;
                brush.setColor(QColor::fromRgb(red, green, blue, alpha));
            }
            else {
                // BrushID is a real BrushID
                quint32 dummy;
                stream >> dummy;
                brush.setColor(Qt::blue); // FIXME:
            }

            
            bool     isCompressed = (flags & 0x02);
            stream >> count;
            QVector<QRectF>  rects;
            rects.resize(count);
#if DEBUG_EMFPLUSPARSER
#endif
            for (uint i = 0; i < count; ++i) {
                if (isCompressed) {
                    rects[i] = emfPlusRectFromStream(stream);
                }
                else {
                    QRectF  rect;
                    stream >> rect;
                    rects[i] = rect;
                }
            }

            QPen  dummyPen;
            m_backend->rects(context, EmfAbstractBackend::DoFill, dummyPen, brush, count, rects);
        }
        break;
    case EmfPlusDrawRects:
        {
            quint32  dataSize; 
            quint32  count;

            stream >> dataSize;
            stream >> count;

            bool     isCompressed = (flags & 0x02);
            quint32  penID = (flags >> 8) & 0xff;
            
            QVector<QRectF>  rects;
            rects.resize(count);
            for (uint i = 0; i < count; ++i) {
                if (isCompressed) {
                    rects[i] = emfPlusRectFromStream(stream);
                }
                else {
                    QRectF  rect;
                    stream >> rect;
                    rects[i] = rect;
                }
            }

            // FIXME: Get pen here from penID
            QPen     pen = QPen(Qt::black);
            QBrush   dummyBrush;
            m_backend->rects(context, EmfAbstractBackend::DoStroke, pen, dummyBrush, count, rects);
        }
        break;
    case EmfPlusFillPolygon:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusDrawLines:
        {
            quint32  dataSize;
            quint32  count;

            bool  isCompressed = (flags & 0x02);
            
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusFillEllipse:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusDrawEllipse:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusFillPie:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusDrawPie:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusDrawArc:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusFillRegion:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusFillPath:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusDrawPath:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusFillClosedCurve:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusDrawClosedCurve:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusDrawCurve:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusDrawBeziers:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusDrawImage:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusDrawImagePoints:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusDrawString:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusSetRenderingOrigin:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusSetAntiAliasMode:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusSetTextRenderingHint:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusSetTextContrast:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusSetInterpolationMode:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusSetPixelOffsetMode:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusSetCompositingMode:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusSetCompositingQuality:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusSave:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusRestore:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusBeginContainer:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusBeginContainerNoParams:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusEndContainer:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusSetWorldTransform:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusResetWorldTransform:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusMultiplyWorldTransform:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusTranslateWorldTransform:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusScaleWorldTransform:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusRotateWorldTransform:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusSetPageTransform:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusResetClip:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusSetClipRect:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusSetClipPath:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusSetClipRegion:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusOffsetClip:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusDrawDriverstring:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusStrokeFillPath:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusSerializableObject:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusSetTSGraphics:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    case EmfPlusSetTSClip:
        {
            // Not Yet Implemented
            soakBytes(stream, size - 8);
        }
        break;
    default:
#if DEBUG_EMFPLUSPARSER
        kDebug(31000) << "unknown record type:" << type;
#endif
	soakBytes(stream, size - 8); // because we already took 8.
    }
#endif

    return true;
}

}
