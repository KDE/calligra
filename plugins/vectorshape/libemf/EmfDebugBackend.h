/*
  Copyright 2008 Brad Hards  <bradh@frogmouth.net>
  Copyright 2009 Inge Wallin <inge@lysator.liu.se>

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

#ifndef EMFDEBUGBACKEND_H
#define EMFDEBUGBACKEND_H

#include "emf_export.h"

#include <QList>
#include <QPainter>
#include <QPixmap>
#include <QRect> // also provides QSize, QPoint
#include <QString>
#include <QVariant>

#include "EmfEnums.h"
#include "EmfHeader.h"
#include "EmfRecords.h"
#include "EmfAbstractBackend.h"

/**
   \file

   Contains definitions for an EMF debug backend
*/

/**
   Namespace for Enhanced Metafile (EMF) classes
*/
namespace Libemf
{


/**
    Debug (text dump) backend for EMF Parser
*/
class EMF_EXPORT EmfDebugBackend : public EmfAbstractBackend
{
public:
    EmfDebugBackend();
    ~EmfDebugBackend();

    void init( const Header *header );
    void cleanup( const Header *header );
    void eof();

    void createPen(EmfDeviceContext &context, quint32 ihPen, quint32 penStyle, quint32 x, quint32 y,
		    quint8 red, quint8 green, quint8 blue, quint8 reserved );
    void createBrushIndirect(EmfDeviceContext &context, quint32 ihBrush, quint32 BrushStyle, quint8 red,
			      quint8 green, quint8 blue, quint8 reserved, 
			      quint32 BrushHatch );
    void createMonoBrush(EmfDeviceContext &context, quint32 ihBrush, Bitmap *bitmap );
    void selectObject(EmfDeviceContext &context, const quint32 ihObject );
    void deleteObject(EmfDeviceContext &context, const quint32 ihObject );
    void arc(EmfDeviceContext &context, const QRect &box, const QPoint &start, const QPoint &end );
    void chord(EmfDeviceContext &context, const QRect &box, const QPoint &start, const QPoint &end );
    void pie(EmfDeviceContext &context, const QRect &box, const QPoint &start, const QPoint &end );
    void ellipse(EmfDeviceContext &context, const QRect &box );
    void rectangle(EmfDeviceContext &context, const QRect &box );
    void setMetaRgn(EmfDeviceContext &context);
    void setWindowOrgEx(EmfDeviceContext &context, const QPoint &origin );
    void setWindowExtEx(EmfDeviceContext &context, const QSize &size );
    void setViewportOrgEx(EmfDeviceContext &context, const QPoint &origin );
    void setViewportExtEx(EmfDeviceContext &context, const QSize &size );
    void beginPath(EmfDeviceContext &context);
    void closeFigure(EmfDeviceContext &context);
    void endPath(EmfDeviceContext &context);
    void setBkMode(EmfDeviceContext &context, const quint32 backgroundMode );
    void setPolyFillMode(EmfDeviceContext &context, const quint32 polyFillMode );
    void setLayout(EmfDeviceContext &context, const quint32 layoutMode );
    void extCreateFontIndirectW(EmfDeviceContext &context, const ExtCreateFontIndirectWRecord &extCreateFontIndirectW );
    void setPixelV(EmfDeviceContext &context, QPoint &point, quint8 red, quint8 green, quint8 blue, quint8 reserved );
    void modifyWorldTransform(EmfDeviceContext &context, quint32 mode, float M11, float M12,
			       float M21, float M22, float Dx, float Dy );
    void setWorldTransform(EmfDeviceContext &context, float M11, float M12, float M21,
			    float M22, float Dx, float Dy );
    void extTextOut(EmfDeviceContext &context, const QRect &bounds, const EmrTextObject &textObject );
    void moveToEx(EmfDeviceContext &context, const qint32 x, const qint32 y );
    void saveDC(EmfDeviceContext &context);
    void restoreDC(EmfDeviceContext &context, const qint32 savedDC );
    void lineTo(EmfDeviceContext &context, const QPoint &finishPoint );
    void arcTo(EmfDeviceContext &context, const QRect &box, const QPoint &start, const QPoint &end );
    void polygon16(EmfDeviceContext &context, const QRect &bounds, const QList<QPoint> points );
    void polyLine(EmfDeviceContext &context, const QRect &bounds, const QList<QPoint> points );
    void polyLine16(EmfDeviceContext &context, const QRect &bounds, const QList<QPoint> points );
    void polyPolygon16(EmfDeviceContext &context, const QRect &bounds, const QList< QVector< QPoint > > &points );
    void polyPolyLine16(EmfDeviceContext &context, const QRect &bounds, const QList< QVector< QPoint > > &points );
    void polyLineTo16(EmfDeviceContext &context, const QRect &bounds, const QList<QPoint> points );
    void polyBezier16(EmfDeviceContext &context, const QRect &bounds, const QList<QPoint> points );
    void polyBezierTo16(EmfDeviceContext &context, const QRect &bounds, const QList<QPoint> points );
    void fillPath(EmfDeviceContext &context, const QRect &bounds );
    void strokeAndFillPath(EmfDeviceContext &context, const QRect &bounds );
    void strokePath(EmfDeviceContext &context, const QRect &bounds );
    void setClipPath(EmfDeviceContext &context, const quint32 regionMode );
    void bitBlt(EmfDeviceContext &context, BitBltRecord &bitBltRecord );
    void setStretchBltMode(EmfDeviceContext &context, const quint32 stretchMode );
    void stretchDiBits(EmfDeviceContext &context, StretchDiBitsRecord &stretchDiBitsRecord );
};


}

#endif
