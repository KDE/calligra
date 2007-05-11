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

#ifndef KDCHARTABSTRACTAREA_H
#define KDCHARTABSTRACTAREA_H

#include <QObject>

#include "KDChartGlobal.h"
#include "KDChartAbstractAreaBase.h"
#include "KDChartLayoutItems.h"

namespace KDChart {


/**
  * @class AbstractArea KDChartAbstractArea.h
  * @brief An area in the chart with a background, a frame, etc.
  *
  * AbstractArea is the base class for all non-widget chart elements that have
  * a set of background attributes and frame attributes, such as
  * coordinate planes or axes.
  *
  * @note This class inherits from AbstractAreaBase, AbstractLayoutItem, QObject.
  * The reason for this tripple inheritance is that neither AbstractAreaBase nor
  * AbstractLayoutItem are QObject.
  */
class KDCHART_EXPORT AbstractArea : public QObject, public AbstractAreaBase, public AbstractLayoutItem
{
    Q_OBJECT

    Q_DISABLE_COPY( AbstractArea )
    KDCHART_DECLARE_PRIVATE_DERIVED( AbstractArea )


public:
    virtual ~AbstractArea() ;

//    virtual AbstractArea * clone() const = 0;
    /**
      * @brief Draws the background and frame, then calls paint().
      *
      * In most cases there is no need to overwrite this method in a derived
      * class, but you would overwrite AbstractLayoutItem::paint() instead.
      */
    virtual void paintIntoRect( QPainter& painter, const QRect& rect );

    /**
      * Call paintAll, if you want the background and the frame to be drawn
      * before the normal paint() is invoked automatically.
      */
    void paintAll( QPainter& painter );

protected:
    AbstractArea();
    virtual QRect areaGeometry() const;
    virtual void positionHasChanged();

Q_SIGNALS:
    void positionChanged( AbstractArea * );

    //KDCHART_DECLARE_PRIVATE_DERIVED(AbstractArea)
}; // End of class AbstractArea

}
#endif // KDCHARTABSTRACTAREA_H
