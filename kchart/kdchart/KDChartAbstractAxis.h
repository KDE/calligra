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

#ifndef KDCHARTAXIS_H
#define KDCHARTAXIS_H

// #include <QObject>
#include <QRectF>
// #include <QWidget>

#include "kdchart_export.h"
#include "KDChartGlobal.h"
#include "KDChartAbstractArea.h"
#include "KDChartTextAttributes.h"


class QPainter;
class QSizeF;
class QRectF;


namespace KDChart {

    class Area;
    class AbstractCoordinatePlane;
    class PaintContext;
    class AbstractDiagram;

    /**
      * The base class for axes.
      *
      * For being useful, axes need to be assigned to a diagram, see
      * AbstractCartesianDiagram::addAxis and AbstractCartesianDiagram::takeAxis.
      *
      * \sa PolarAxis, AbstractCartesianDiagram
      */
    class KDCHART_EXPORT AbstractAxis : public AbstractArea
    {
        Q_OBJECT

        Q_DISABLE_COPY( AbstractAxis )
        KDCHART_DECLARE_PRIVATE_DERIVED_PARENT( AbstractAxis, AbstractDiagram* )

    public:
        explicit AbstractAxis( AbstractDiagram* diagram = 0 );
        virtual ~AbstractAxis();

        // FIXME implement when code os ready for it:
        // virtual Area* clone() const = 0;

        // FIXME (Mirko) readd when needed
        // void copyRelevantDetailsFrom( const KDChartAxis* axis );

    /*    virtual void paint( PaintContext* ) const = 0;
        virtual QSize sizeHint() const = 0;*/
	//virtual void paintEvent( QPaintEvent* event) = 0;

        void createObserver( AbstractDiagram* diagram );
        void deleteObserver( AbstractDiagram* diagram );
        const AbstractDiagram* diagram() const;
        bool observedBy( AbstractDiagram* diagram ) const;
        virtual void connectSignals();

        void setTextAttributes( const TextAttributes &a );
        TextAttributes textAttributes() const;

        void setLabels( const QStringList& list );
        QStringList labels() const;
        void setShortLabels( const QStringList& list );
        QStringList shortLabels() const;

        virtual void setGeometry( const QRect& rect ) = 0;
        virtual QRect geometry() const = 0;

        const AbstractCoordinatePlane* coordinatePlane() const;

    protected Q_SLOTS:
        /** called for initializing after the c'tor has completed */
        virtual void delayedInit();

    public Q_SLOTS:
        void update();
    };
}

#endif // KDCHARTAXIS_H
