/*
  Copyright 2008      Brad Hards  <bradh@frogmouth.net>
  Copyright 2009-2010 Inge Wallin <inge@lysator.liu.se>

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

#include "EmfDebugBackend.h"

#include <math.h>

#include <KDebug>

#include "EmfDeviceContext.h"
#include "EmfObjects.h"

namespace Libemf
{




EmfDebugBackend::EmfDebugBackend()
{
}

EmfDebugBackend::~EmfDebugBackend()
{
}

void EmfDebugBackend::init( const Header *header )
{
    kDebug(33100) << "Initialising EmfDebugBackend";
    kDebug(33100) << "image size:" << header->bounds().size();
}

void EmfDebugBackend::cleanup( const Header *header )
{
    kDebug(33100) << "Cleanup EmfDebugBackend";
    kDebug(33100) << "image size:" << header->bounds().size();
}

void EmfDebugBackend::eof()
{
    kDebug(33100) << "EMR_EOF";
}

void EmfDebugBackend::setPixelV(EmfDeviceContext &context,
                                QPoint &point, quint8 red, quint8 green, quint8 blue, quint8 reserved )
{
    Q_UNUSED(context);
    Q_UNUSED(reserved);

    kDebug(33100) << "EMR_SETPIXELV:" << point << QColor( red, green, blue );
}

void EmfDebugBackend::beginPath(EmfDeviceContext &context)
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_BEGINPATH";
}

void EmfDebugBackend::closeFigure(EmfDeviceContext &context)
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_CLOSEFIGURE";
}

void EmfDebugBackend::endPath(EmfDeviceContext &context)
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_ENDPATH";
}

void EmfDebugBackend::saveDC(EmfDeviceContext &context)
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_SAVEDC";
}

void EmfDebugBackend::restoreDC(EmfDeviceContext &context, qint32 savedDC )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_RESTOREDC" << savedDC;
}

void EmfDebugBackend::setMetaRgn(EmfDeviceContext &context)
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_SETMETARGN";
}

void EmfDebugBackend::setWindowOrgEx(EmfDeviceContext &context, const QPoint &origin )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_SETWINDOWORGEX" << origin;
}

void EmfDebugBackend::setWindowExtEx(EmfDeviceContext &context, const QSize &size )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_SETWINDOWEXTEX" << size;
}

void EmfDebugBackend::setViewportOrgEx(EmfDeviceContext &context, const QPoint &origin )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_SETVIEWPORTORGEX" << origin;
}

void EmfDebugBackend::setViewportExtEx(EmfDeviceContext &context, const QSize &size )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_SETVIEWPORTEXTEX" << size;
}

void EmfDebugBackend::deleteObject(EmfDeviceContext &context, const quint32 ihObject )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_DELETEOBJECT:" << ihObject;
}

void EmfDebugBackend::arc(EmfDeviceContext &context, const QRect &box, const QPoint &start, const QPoint &end )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_ARC" << box << start << end;
}

void EmfDebugBackend::chord(EmfDeviceContext &context, const QRect &box, const QPoint &start, const QPoint &end )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_CHORD" << box << start << end;
}

void EmfDebugBackend::pie(EmfDeviceContext &context, const QRect &box, const QPoint &start, const QPoint &end )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_PIE" << box << start << end;
}

void EmfDebugBackend::ellipse(EmfDeviceContext &context, const QRect &box )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_ELLIPSE:" << box;
}

void EmfDebugBackend::rectangle(EmfDeviceContext &context, const QRect &box )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_RECTANGLE:" << box;
}

void EmfDebugBackend::modifyWorldTransform(EmfDeviceContext &context, quint32 mode, float M11, float M12,
					float M21, float M22, float Dx, float Dy )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_MODIFYWORLDTRANSFORM:" << mode << QTransform ( M11, M12, M21, M22, Dx, Dy );
}

void EmfDebugBackend::setWorldTransform(EmfDeviceContext &context, float M11, float M12, float M21,
				     float M22, float Dx, float Dy )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_SETWORLDTRANSFORM:" << QTransform ( M11, M12, M21, M22, Dx, Dy );
}

void EmfDebugBackend::setBkMode(EmfDeviceContext &context, const quint32 backgroundMode )
{
    Q_UNUSED(context);

    if ( backgroundMode == TRANSPARENT ) {
        kDebug(33100) << "EMR_SETBKMODE: Transparent";
    } else if ( backgroundMode == OPAQUE ) {
        kDebug(33100) << "EMR_SETBKMODE: Opaque";
    } else {
        kDebug(33100) << "EMR_SETBKMODE: Unexpected value -" << backgroundMode;
        Q_ASSERT( 0 );
    }
}

void EmfDebugBackend::setLayout(EmfDeviceContext &context, const quint32 layoutMode )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_SETLAYOUT:" << layoutMode;
}

void EmfDebugBackend::extCreateFontIndirectW(EmfDeviceContext &context,
                                             const ExtCreateFontIndirectWRecord &extCreateFontIndirectW )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_CREATEFONTINDIRECTW:" << extCreateFontIndirectW.fontFace();
}

void EmfDebugBackend::createPen(EmfDeviceContext &context, quint32 ihPen, quint32 penStyle, quint32 x, quint32 y,
			       quint8 red, quint8 green, quint8 blue, quint8 reserved )
{
    Q_UNUSED(context);
    Q_UNUSED(y);
    Q_UNUSED(reserved);

    kDebug(33100) << "EMR_CREATEPEN" << "ihPen:" << ihPen << ", penStyle:" << penStyle
                  << "width:" << x << "color:" << QColor( red, green, blue );
}

void EmfDebugBackend::createBrushIndirect(EmfDeviceContext &context, quint32 ihBrush, quint32 BrushStyle, quint8 red,
				       quint8 green, quint8 blue, quint8 reserved,
				       quint32 BrushHatch )
{
    Q_UNUSED(context);
    Q_UNUSED(reserved);

    kDebug(33100) << "EMR_CREATEBRUSHINDIRECT:" << ihBrush << "style:" << BrushStyle
             << "Colour:" << QColor( red, green, blue ) << ", Hatch:" << BrushHatch;
}

void EmfDebugBackend::createMonoBrush(EmfDeviceContext &context, quint32 ihBrush, Bitmap *bitmap )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_CREATEMONOBRUSH:" << ihBrush << "bitmap:" << bitmap;
}

void EmfDebugBackend::selectObject(EmfDeviceContext &context, const quint32 ihObject )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_SELECTOBJECT" << ihObject;
}

void EmfDebugBackend::extTextOut(EmfDeviceContext &context, const QRect &bounds, const EmrTextObject &textObject )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_EXTTEXTOUTW:" << bounds
                  << textObject.referencePoint()
                  << textObject.textString();
}

void EmfDebugBackend::moveToEx(EmfDeviceContext &context, const qint32 x, const qint32 y )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_MOVETOEX" << QPoint( x, y );
}

void EmfDebugBackend::lineTo(EmfDeviceContext &context, const QPoint &finishPoint )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_LINETO" << finishPoint;
}

void EmfDebugBackend::arcTo(EmfDeviceContext &context, const QRect &box, const QPoint &start, const QPoint &end )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_ARCTO" << box << start << end;
}

void EmfDebugBackend::polygon16(EmfDeviceContext &context, const QRect &bounds, const QList<QPoint> points )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_POLYGON16" << bounds << points;
}

void EmfDebugBackend::polyLine(EmfDeviceContext &context, const QRect &bounds, const QList<QPoint> points )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_POLYLINE" << bounds << points;
}

void EmfDebugBackend::polyLine16(EmfDeviceContext &context, const QRect &bounds, const QList<QPoint> points )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_POLYLINE16" << bounds << points;
}

void EmfDebugBackend::polyPolyLine16(EmfDeviceContext &context,
                                     const QRect &bounds, const QList< QVector< QPoint > > &points )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_POLYPOLYLINE16" << bounds << points;
}

void EmfDebugBackend::polyPolygon16(EmfDeviceContext &context,
                                    const QRect &bounds, const QList< QVector< QPoint > > &points )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_POLYPOLYGON16" << bounds << points;
}

void EmfDebugBackend::polyLineTo16(EmfDeviceContext &context,
                                   const QRect &bounds, const QList<QPoint> points )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_POLYLINETO16" << bounds << points;
}

void EmfDebugBackend::polyBezier16(EmfDeviceContext &context,
                                   const QRect &bounds, const QList<QPoint> points )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_POLYBEZIER16" << bounds << points;
}

void EmfDebugBackend::polyBezierTo16(EmfDeviceContext &context,
                                     const QRect &bounds, const QList<QPoint> points )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_POLYBEZIERTO16" << bounds << points;
}

void EmfDebugBackend::fillPath(EmfDeviceContext &context, const QRect &bounds )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_FILLPATH" << bounds;
}

void EmfDebugBackend::strokeAndFillPath(EmfDeviceContext &context, const QRect &bounds )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_STROKEANDFILLPATH" << bounds;
}

void EmfDebugBackend::strokePath(EmfDeviceContext &context, const QRect &bounds )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_STROKEPATH" << bounds;
}

void EmfDebugBackend::setClipPath(EmfDeviceContext &context, quint32 regionMode )
{
    Q_UNUSED(context);

   kDebug(33100) << "EMR_SETCLIPPATH:" << regionMode;
}

void EmfDebugBackend::bitBlt(EmfDeviceContext &context, BitBltRecord &bitBltRecord )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_BITBLT:" << bitBltRecord.destinationRectangle();
}

void EmfDebugBackend::setStretchBltMode(EmfDeviceContext &context, const quint32 stretchMode )
{
    Q_UNUSED(context);

    switch ( stretchMode ) {
    case 0x01:
        kDebug(33100) << "EMR_STRETCHBLTMODE: STRETCH_ANDSCANS";
        break;
    case 0x02:
        kDebug(33100) << "EMR_STRETCHBLTMODE: STRETCH_ORSCANS";
        break;
    case 0x03:
        kDebug(33100) << "EMR_STRETCHBLTMODE: STRETCH_DELETESCANS";
        break;
    case 0x04:
        kDebug(33100) << "EMR_STRETCHBLTMODE: STRETCH_HALFTONE";
        break;
    default:
        kDebug(33100) << "EMR_STRETCHBLTMODE - unknown stretch mode:" << stretchMode;
    }
}

void EmfDebugBackend::stretchDiBits(EmfDeviceContext &context, StretchDiBitsRecord &stretchDiBitsRecord )
{
    Q_UNUSED(context);

    kDebug(33100) << "EMR_STRETCHDIBITS:" << stretchDiBitsRecord.sourceRectangle()
                  << "," << stretchDiBitsRecord.destinationRectangle();
}


} // xnamespace...
