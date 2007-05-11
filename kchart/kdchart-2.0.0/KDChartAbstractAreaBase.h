/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2005-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Chart licenses may use this file in
 ** accordance with the KD Chart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdchart for
 **   information about KD Chart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#ifndef KDCHARTABSTRACTAREABASE_H
#define KDCHARTABSTRACTAREABASE_H

#include <QPointF>
#include <QSizeF>
#include <QRectF>

#include "KDChartGlobal.h"
#include "KDChartLayoutItems.h"
#include "KDChartRelativePosition.h"


class QPainter;
class QString;
namespace KDChart {
    class TextAttributes;
    class BackgroundAttributes;
    class FrameAttributes;
    class PaintContext;


/**
  * @class AbstractAreaBase KDChartAbstractAreaBase.h
  * @brief Base class for AbstractArea and AbstractAreaWidget: An area
  * in the chart with a background, a frame, etc.
  *
  * AbstractAreaBase is the base class for all chart elements that have
  * a set of background attributes and frame attributes, such as
  * legends or axes.
  *
  * @note Normally you should not use AbstractAreaBase directly, but
  * derive your classes from AbstractArea or AbstractAreaWidget.
  *
  * @note This classis not a QObject, so it is easier to inherit from
  * it, if your are inheriting from a QObject too like AbstractAreaWidget does it.
  *
  * @sa AbstractArea, AbstractAreaWidget
  */
class KDCHART_EXPORT AbstractAreaBase
{
    Q_DISABLE_COPY( AbstractAreaBase )
    KDCHART_DECLARE_PRIVATE_BASE_POLYMORPHIC( AbstractAreaBase )

protected:
    AbstractAreaBase();
    virtual ~AbstractAreaBase() ;

public:
//    virtual AbstractAreaBase * clone() const = 0;


    void alignToReferencePoint( const RelativePosition& position );

    void setFrameAttributes( const FrameAttributes &a );
    FrameAttributes frameAttributes() const;

    void setBackgroundAttributes( const BackgroundAttributes &a );
    BackgroundAttributes backgroundAttributes() const;

    virtual void paintBackground( QPainter& painter, const QRect& rectangle );
    virtual void paintFrame( QPainter& painter, const QRect& rectangle );


protected:
    /** \internal
      * This internal method is used by AbstractArea and AbstractAreaWidget
      * to reduce the size of the area before calling paint( QPainter ).
      * \note Normally you should not call this method, but derive your classes
      * from AbstractArea or AbstractAreaWidget.
      * \sa AbstractArea, AbstractAreaWidget
      */
    QRect innerRect() const;

    /** \internal
      * This internal method is used by AbstractArea and AbstractAreaWidget
      * to find out the real widget size.
      * \sa AbstractArea, AbstractAreaWidget
      */
    virtual QRect areaGeometry() const = 0;

    /** \internal
      * This internal method can be overwritten by derived classes,
      * if they want to emit a signal (or perform other actions, resp.)
      * when the Position of the area has been changed.
      * The default implementation does nothing.
      */
    virtual void positionHasChanged();

}; // End of class AbstractAreaBase

}
#endif // KDCHARTABSTRACTAREABASE_H
