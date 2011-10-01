/*
  Copyright 2008 Brad Hards  <bradh@frogmouth.net>
  Copyright 2009, 2011 Inge Wallin <inge@lysator.liu.se>

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

#ifndef EMFABSTRACTBACKEND_H
#define EMFABSTRACTBACKEND_H

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

/**
   \file

   Primary definitions for EMF backends
*/

/**
   Namespace for Enhanced Metafile (EMF) classes
*/
namespace Libemf
{

class EmfDeviceContext;

/**
    Abstract backend for EMF Parser
*/
class EMF_EXPORT EmfAbstractBackend
{
public:
    EmfAbstractBackend() {};
    virtual ~EmfAbstractBackend() {};

    /**
       Initialisation routine

       \param header the EMF Header record
    */
    virtual void init( const Header *header) = 0;

    /**
       Cleanup routine

       This function is called when the painting is done.  Any
       initializations that are done in init() can be undone here if
       necessary.

       \param header the EMF Header record
    */
    virtual void cleanup( const Header *header) = 0;

    /**
       Close-out routine
    */
    virtual void eof() = 0;

    /**
       Handler for the EMR_SETPIXELV record type

       This fills a specified pixel with a particular color

       \param point the point to fill
       \param red the red component of the color
       \param green the green component of the color
       \param blue the blue component of the color
       \param reserved reserved component of the color
    */
    virtual void setPixelV(EmfDeviceContext &context,
                           QPoint &point, quint8 red, quint8 green, quint8 blue, quint8 reserved) = 0;

    /**
       Handler for the EMR_CREATEBRUSHINDIRECT record type
    */
    virtual void createBrushIndirect(EmfDeviceContext &context,
                                     quint32 ihBrush, quint32 BrushStyle, quint8 red,
                                     quint8 green, quint8 blue, quint8 reserved, 
                                     quint32 BrushHatch) = 0;

    virtual void createMonoBrush(EmfDeviceContext &context, quint32 ihBrush, Bitmap *bitmap) = 0;

    /**
       Handler for the EMR_SETMETARGN record type
    */
    virtual void setMetaRgn(EmfDeviceContext &context) = 0;

    /**
       Select a previously created (or stock) object

       \param ihObject the reference number for the object to select
    */
    virtual void selectObject(EmfDeviceContext &context, const quint32 ihObject) = 0;

    /**
       Delete a previously created (or stock) object

       \param ihObject the reference number for the object to delete
    */
    virtual void deleteObject(EmfDeviceContext &context, const quint32 ihObject) = 0;

    /**
       Handler for the EMR_ARC record type

       \param box the bounding box
       \param start the coordinates of the point that defines the first radial end point
       \param end the coordinates of the point that defines the second radial end point
    */
    virtual void arc(EmfDeviceContext &context,
                     const QRect &box, const QPoint &start, const QPoint &end) = 0;

    /**
       Handler for the EMR_CHORD record type

       \param box the bounding box
       \param start the coordinates of the point that defines the first radial end point
       \param end the coordinates of the point that defines the second radial end point
    */
    virtual void chord(EmfDeviceContext &context,
                       const QRect &box, const QPoint &start, const QPoint &end) = 0;

    /**
       Handler for the EMR_PIE record type

       \param box the bounding box
       \param start the coordinates of the point that defines the first radial end point
       \param end the coordinates of the point that defines the second radial end point
    */
    virtual void pie(EmfDeviceContext &context,
                     const QRect &box, const QPoint &start, const QPoint &end) = 0;

    /**
      Handler for the EMR_ELLIPSE record type

      \param box the bounding box for the ellipse
    */
    virtual void ellipse(EmfDeviceContext &context, const QRect &box) = 0;

    /**
      Handler for the EMR_RECTANGLE record type

      \param box the bounding box for the rectangle
    */
    virtual void rectangle(EmfDeviceContext &context, const QRect &box) = 0;

    /**
       Handler for the EMR_EXTCREATEFONTINDIRECTW record type

       \param extCreateFontIndirectWRecord the contents of the
       EMR_EXTCREATEFONTINDIRECTW record
    */
    virtual void extCreateFontIndirectW(EmfDeviceContext &context,
                                        const ExtCreateFontIndirectWRecord &extCreateFontIndirectW) = 0;

    /**
       Handler for text rendering, as described in the the
       EMR_EXTTEXTOUTW and EMR_EXTTEXTOUTA record types.

       \param bounds the bounds used for e.g. clipping 
       \param texObject The object describing the text.
    */
    virtual void extTextOut(EmfDeviceContext &context,
                            const QRect &bounds, const EmrTextObject &textObject) = 0;

    /**
       Handler for the EMR_MOVETOEX record type

       \param x the X coordinate of the point to move to
       \param y the Y coordiante of the point to move to
    */
    virtual void moveToEx(EmfDeviceContext &context, const qint32 x, const qint32 y) = 0;

    /**
       Handler for the EMR_SAVEDC record type
    */
    virtual void saveDC(EmfDeviceContext &context) = 0;

    /**
       Handler for the EMR_RESTOREDC record type

       \param savedDC the device context to restore to (always negative)
    */
    virtual void restoreDC(EmfDeviceContext &context, const qint32 savedDC) = 0;

    /**
       Handler for the EMR_LINETO record type

       \param finishPoint the point to draw to
    */
    virtual void lineTo(EmfDeviceContext &context, const QPoint &finishPoint) = 0;

    /**
       Handler for the EMR_ARCTO record type

       \param box the bounding box
       \param start the coordinates of the point that defines the first radial end point
       \param end the coordinates of the point that defines the second radial end point
    */
    virtual void arcTo(EmfDeviceContext &context,
                       const QRect &box, const QPoint &start, const QPoint &end) = 0;

    /**
       Handler for the EMR_POLYGON16 record type.

       This record type specifies how to output a multi-segment filled
       polygon.

       \param bounds the bounding rectangle for the line segment
       \param points the sequence of points that describe the polygon
    */
    virtual void polygon16(EmfDeviceContext &context,
                           const QRect &bounds, const QList<QPoint> points) = 0;

    /**
       Handler for the EMR_POLYLINE record type.

       This record type specifies how to output a multi-segment line
       (unfilled polyline).

       \param bounds the bounding rectangle for the line segments
       \param points the sequence of points that describe the line

       \note the line is not meant to be closed (i.e. do not connect
       the last point to the first point) or filled.
    */
    virtual void polyLine(EmfDeviceContext &context,
                          const QRect &bounds, const QList<QPoint> points) = 0;
    
    /**
       Handler for the EMR_POLYLINE16 record type.

       This record type specifies how to output a multi-segment line
       (unfilled polyline).

       \param bounds the bounding rectangle for the line segment
       \param points the sequence of points that describe the line

       \note the line is not meant to be closed (i.e. do not connect
       the last point to the first point) or filled.
    */
    virtual void polyLine16(EmfDeviceContext &context,
                            const QRect &bounds, const QList<QPoint> points) = 0;

    /**
       Handler for the EMR_POLYPOLYLINE16 record type.

       This record type specifies how to output a set of multi-segment line
       (unfilled polylines). Each vector in the list is a separate polyline

       \param bounds the bounding rectangle for the line segments
       \param points the sequence of points that describe the line

       \note the lines are not meant to be closed (i.e. do not connect
       the last point to the first point) or filled.
    */
    virtual void polyPolyLine16(EmfDeviceContext &context,
                                const QRect &bounds, const QList< QVector< QPoint > > &points) = 0;

    /**
       Handler for the EMR_POLYPOLYGON16 record type.

       This record type specifies how to output a set of multi-segment polygons.
       Each vector in the list is a separate filled polygon.

       \param bounds the bounding rectangle for the polygons
       \param points the sequence of points that describe the polygons
    */
    virtual void polyPolygon16(EmfDeviceContext &context,
                               const QRect &bounds, const QList< QVector< QPoint > > &points) = 0;

    /**
       Handler for the EMR_POLYLINETO16 record type.

       This record type specifies how to output a multi-segment set of
       lines (unfilled).

       \param bounds the bounding rectangle for the bezier curves
       \param points the sequence of points that describe the curves

       \note the line is not meant to be closed (i.e. do not connect
       the last point to the first point) or filled.
    */
    virtual void polyLineTo16(EmfDeviceContext &context,
                              const QRect &bounds, const QList<QPoint> points) = 0;

    /**
       Handler for the EMR_POLYBEZIERO16 record type.

       This record type specifies how to output a multi-segment set of
       bezier curves (unfilled).

       \param bounds the bounding rectangle for the bezier curves
       \param points the sequence of points that describe the curves

       \note the line is not meant to be closed (i.e. do not connect
       the last point to the first point) or filled.
    */
    virtual void polyBezier16(EmfDeviceContext &context,
                              const QRect &bounds, const QList<QPoint> points) = 0;

    /**
       Handler for the EMR_POLYLINETO16 record type.

       This record type specifies how to output a multi-segment set of
       bezier curves (unfilled), starting at the current point.

       \param bounds the bounding rectangle for the bezier curves
       \param points the sequence of points that describe the curves

       \note the line is not meant to be closed (i.e. do not connect
       the last point to the first point) or filled.
    */
    virtual void polyBezierTo16(EmfDeviceContext &context,
                                const QRect &bounds, const QList<QPoint> points) = 0;

    /**
       Handler for the EMR_FILLPATH record type.

       \param bounds the bounding rectangle for the region to be filled.
    */
    virtual void fillPath(EmfDeviceContext &context,
                          const QRect &bounds) = 0;

    /**
       Handler for the EMR_STROKEANDFILLPATH record type.

       \param bounds the bounding rectangle for the region to be stroked / filled
    */
    virtual void strokeAndFillPath(EmfDeviceContext &context, const QRect &bounds) = 0;

    /**
       Handler for the EMR_STROKEPATH record type.

       \param bounds the bounding rectangle for the region to be stroked
    */
    virtual void strokePath(EmfDeviceContext &context, const QRect &bounds) = 0;

    /**
       Handler for the EMR_SETCLIPPATH record type.
       
       See [MS-EMF] Section 2.1.29 for valid ways to set the path.
       
       \param regionMode how to set the clipping path.
    */
    virtual void setClipPath(EmfDeviceContext &context, const quint32 regionMode) = 0;

    /**
       Handler for the EMR_BITBLT record type

       \param bitBltRecord contents of the record type
    */
    virtual void bitBlt(EmfDeviceContext &context, BitBltRecord &bitBltRecord) = 0;

    /**
       Handler for the EMR_STRETCHDIBITS record type

       \param stretchDiBitsRecord contents of the record type
    */
    virtual void stretchDiBits(EmfDeviceContext &context, StretchDiBitsRecord &stretchDiBitsRecord) = 0;
};


}

#endif
