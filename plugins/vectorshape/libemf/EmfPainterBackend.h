/*
  Copyright 2008        Brad Hards  <bradh@frogmouth.net>
  Copyright 2009 - 2011 Inge Wallin <inge@lysator.liu.se>

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

#ifndef EMFPAINTERBACKEND_H
#define EMFPAINTERBACKEND_H

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

   Primary definitions for EMF backend
*/

/**
   Namespace for Enhanced Metafile (EMF) classes
*/
namespace Libemf
{

class EmrTextObject;

/**
    QPainter based backend for EMF Parser.

    This class allows rendering of an EMF file to a QPixmap or any other QPaintDevice.
*/
class EMF_EXPORT EmfPainterBackend : public EmfAbstractBackend
{
public:
    /**
       Constructor.

       This will probably need to take an enum to say what sort of output
       we want.
    */
    EmfPainterBackend();
    EmfPainterBackend(QPainter &painter, QSize &size, bool keepAspectRatio = false);
    ~EmfPainterBackend();

    void init( const Header *header );
    void cleanup( const Header *header );
    void eof();

    /**
       The image that has been rendered to.
    */
    QImage *image();

    void createPen(EmfDeviceContext &context,
                   quint32 ihPen, quint32 penStyle, quint32 x, quint32 y,
                   quint8 red, quint8 green, quint8 blue, quint8 reserved );
    void createBrushIndirect(EmfDeviceContext &context,
                             quint32 ihBrush, quint32 BrushStyle, quint8 red,
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
    void beginPath(EmfDeviceContext &context);
    void closeFigure(EmfDeviceContext &context);
    void endPath(EmfDeviceContext &context);
    void extCreateFontIndirectW(EmfDeviceContext &context,
                                const ExtCreateFontIndirectWRecord &extCreateFontIndirectW );
    void setPixelV(EmfDeviceContext &context,
                   QPoint &point, quint8 red, quint8 green, quint8 blue, quint8 reserved );
    void extTextOut(EmfDeviceContext &context,
                    const QRect &bounds, const EmrTextObject &textObject );
    void moveToEx(EmfDeviceContext &context, const qint32 x, const qint32 y );
    void saveDC(EmfDeviceContext &context);
    void restoreDC(EmfDeviceContext &context, const qint32 savedDC );
    void lineTo(EmfDeviceContext &context, const QPoint &finishPoint );
    void arcTo(EmfDeviceContext &context,
               const QRect &box, const QPoint &start, const QPoint &end );
    void polygon16(EmfDeviceContext &context, const QRect &bounds, const QList<QPoint> points );
    void polyLine16(EmfDeviceContext &context, const QRect &bounds, const QList<QPoint> points );
    void polyPolygon16(EmfDeviceContext &context, const QRect &bounds, const QList< QVector< QPoint > > &points );
    void polyPolyLine16(EmfDeviceContext &context, const QRect &bounds, const QList< QVector< QPoint > > &points );
    void polyLine(EmfDeviceContext &context, const QRect &bounds, const QList<QPoint> points );
    void polyLineTo16(EmfDeviceContext &context, const QRect &bounds, const QList<QPoint> points );
    void polyBezier16(EmfDeviceContext &context, const QRect &bounds, const QList<QPoint> points );
    void polyBezierTo16(EmfDeviceContext &context, const QRect &bounds, const QList<QPoint> points );
    void fillPath(EmfDeviceContext &context, const QRect &bounds );
    void strokeAndFillPath(EmfDeviceContext &context, const QRect &bounds );
    void strokePath(EmfDeviceContext &context, const QRect &bounds );
    void setClipPath(EmfDeviceContext &context, const quint32 regionMode );
    void bitBlt(EmfDeviceContext &context, BitBltRecord &bitBltRecord );
    void stretchDiBits(EmfDeviceContext &context, StretchDiBitsRecord &stretchDiBitsRecord );

private:
    void printPainterTransform(const char *leadText);

    /// For debugging purposes: Draw the boundary box.
    void paintBounds(const Header *header);

    /// Recalculate the world transform and then apply it to the painter
    /// This must be called at the end of every function that changes the transform.
    void recalculateWorldTransform();

    /**
       Select a stock object.

       See [MS-EMF] Section 2.1.31.

       \param ihObject the stock object value
    */
    void selectStockObject(EmfDeviceContext &context, const quint32 ihObject);


    /**
       Helper routine to convert the EMF angle (centrepoint + radial endpoint) into
       the Qt format (in degress - may need to multiply by 16 for some purposes)
    */
    qreal angleFromArc( const QPoint &centrePoint, const QPoint &radialPoint );

    /**
      Calculate the angular difference (span) between two angles
      
      This should always be positive.
    */
    qreal angularSpan( const qreal startAngle, const qreal endAngle );

    /**
       Convert the EMF font weight scale (0..1000) to Qt equivalent.
       
       This is a bit rough - the EMF spec only says 400 is normal, and 
       700 is bold.
    */
    int convertFontWeight( quint32 emfWeight );

    void updateFromDeviceContext(EmfDeviceContext &context);


    Header                  *m_header;   // Save to be able to retain scaling.

    int                      m_painterSaves; // The number of times that save() was called.
    QSize                    m_outputSize;
    bool                     m_keepAspectRatio;

    QMap<quint32, QVariant>  m_objectTable;

    QPainterPath *m_path;
    bool          m_currentlyBuildingPath;

    QPainter                *m_painter;
    QTransform               m_worldTransform; // The transform inside the EMF.
    QTransform               m_outputTransform; // The transform that the painter already had

    // Everything that has to do with window and viewport calculation
    QPoint        m_windowOrg;
    QSize         m_windowExt;
    QPoint        m_viewportOrg;
    QSize         m_viewportExt;
    bool          m_windowExtIsSet;
    bool          m_viewportExtIsSet;
    bool          m_windowViewportIsSet;

#if 0
    // This matrix is needed because the window / viewport calculation
    // is not the last one in the chain. After that one comes the
    // transform that the painter already has when the painting
    // starts, and that one has to be saved and reapplied again after
    // the window / viewport calculation is redone.
    QTransform    m_outputTransform;
#endif

    // ----------------------------------------------------------------
    //                     The playback device context

    // The Playback Device Context (PDC) contains the following:
    //  - bitmap
    //  - brush	(part of the painter)
    //  - palette
    //  - font	(part of the painter)
    //  - pen	(part of the painter)
    //  - region
    //  - drawing mode
    //  - mapping mode
    // FIXME: what more?  textalign?  textpen?

    /**
       The current coordinates
    */
    QPoint  m_currentCoords;
};

}

#endif
