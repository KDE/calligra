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

#ifndef KDCHARTGRIDATTRIBUTES_H
#define KDCHARTGRIDATTRIBUTES_H

#include <QMetaType>
#include "KDChartGlobal.h"
#include "KDChartEnums.h"

class QPen;

namespace KDChart {

class KDCHART_EXPORT GridAttributes
{
public:
    GridAttributes();
    GridAttributes( const GridAttributes& );
    GridAttributes &operator= ( const GridAttributes& );

    ~GridAttributes();

    void setGridVisible( bool visible );
    bool isGridVisible() const;


    void setGridStepWidth( qreal stepWidth=0.0 );
    qreal gridStepWidth() const;

    void setGridSubStepWidth(  qreal subStepWidth=0.0 );
    qreal gridSubStepWidth() const;

    void setGridGranularitySequence( KDChartEnums::GranularitySequence sequence );
    KDChartEnums::GranularitySequence gridGranularitySequence() const;


    void setGridPen( const QPen & pen );
    QPen gridPen() const;


    void setSubGridVisible( bool visible );
    bool isSubGridVisible() const;

    void setSubGridPen( const QPen & pen );
    QPen subGridPen() const;


    void setZeroLinePen( const QPen & pen );
    QPen zeroLinePen() const;

    bool operator==( const GridAttributes& ) const;
    inline bool operator!=( const GridAttributes& other ) const { return !operator==(other); }

private:
    KDCHART_DECLARE_PRIVATE_BASE_VALUE( GridAttributes )
}; // End of class GridAttributes

}

#if !defined(QT_NO_DEBUG_STREAM)
KDCHART_EXPORT QDebug operator<<(QDebug, const KDChart::GridAttributes& );
#endif /* QT_NO_DEBUG_STREAM */

KDCHART_DECLARE_SWAP_SPECIALISATION( KDChart::GridAttributes )
Q_DECLARE_METATYPE( KDChart::GridAttributes )
Q_DECLARE_TYPEINFO( KDChart::GridAttributes, Q_MOVABLE_TYPE );


#endif // KDCHARTGRIDATTRIBUTES_H
