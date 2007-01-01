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

#ifndef KDCHARTABSTRACTPIEDIAGRAM_H
#define KDCHARTABSTRACTPIEDIAGRAM_H

#include "KDChartAbstractPolarDiagram.h"

namespace KDChart {
    class PieAttributes;
    class ThreeDPieAttributes;

class KDCHART_EXPORT AbstractPieDiagram : public AbstractPolarDiagram
{
    Q_OBJECT

    Q_DISABLE_COPY( AbstractPieDiagram )
    KDCHART_DECLARE_DERIVED_DIAGRAM( AbstractPieDiagram, PolarCoordinatePlane )

public:
    explicit AbstractPieDiagram(
        QWidget* parent = 0, PolarCoordinatePlane* plane = 0 );
    virtual ~AbstractPieDiagram();

    /** Set the starting angle for the first dataset. */
    void setStartPosition( qreal degrees );

    /** @return the starting angle for the first dataset. */
    qreal startPosition() const;

    /** Set the granularity: the smaller the granularity the more your diagram
     * segments will show facettes instead of rounded segments.
     * \param value the granularity value between 0.05 (one twentieth of a degree)
     * and 36.0 (one tenth of a full circle), other values will be interpreted as 1.0.
     */
    void setGranularity( qreal value );

    /** @return the granularity. */
    qreal granularity() const;

    void setPieAttributes( const PieAttributes & a );
    void setPieAttributes( int   column,
                           const PieAttributes & a );
    PieAttributes pieAttributes() const;
    PieAttributes pieAttributes( int column ) const;
    PieAttributes pieAttributes( const QModelIndex & index ) const;

    void setThreeDPieAttributes( const ThreeDPieAttributes & a );
    void setThreeDPieAttributes( int   column,
                                 const ThreeDPieAttributes & a );
    void setThreeDPieAttributes( const QModelIndex & index,
                                 const ThreeDPieAttributes & a );
    ThreeDPieAttributes threeDPieAttributes() const;
    ThreeDPieAttributes threeDPieAttributes( int column ) const;
    ThreeDPieAttributes threeDPieAttributes( const QModelIndex & index ) const;
}; // End of class KDChartAbstractPieDiagram

}

#endif // KDCHARTABSTACTPIEDIAGRAM_H
