/****************************************************************************
 ** Copyright (C) 2005-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License versions 2.0 or 3.0 as published by the Free Software
 ** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file.  Alternatively you may (at
 ** your option) use any later version of the GNU General Public
 ** License if such license has been publicly approved by
 ** Klarälvdalens Datakonsult AB (or its successors, if any).
 ** 
 ** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
 ** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE. Klarälvdalens Datakonsult AB reserves all rights
 ** not expressly granted herein.
 ** 
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 **********************************************************************/

#ifndef ADJUSTED_CARTESIAN_AXIS_H
#define ADJUSTED_CARTESIAN_AXIS_H

#include <KDChartCartesianAxis>
#include <KDChartGlobal>


class AdjustedCartesianAxis: public KDChart::CartesianAxis
{
    Q_OBJECT

    Q_DISABLE_COPY( AdjustedCartesianAxis )

public:
    explicit AdjustedCartesianAxis( KDChart::AbstractCartesianDiagram* diagram = 0 );

    virtual const QString customizedLabel( const QString& label )const;

    void setBounds( qreal lower, qreal upper ){
        m_lowerBound = lower;
        m_upperBound = upper;
    }
    qreal lowerBound()const { return m_lowerBound; }
    qreal upperBound()const { return m_upperBound; }

private:
    qreal m_lowerBound;
    qreal m_upperBound;
};

#endif // ADJUSTED_CARTESIAN_AXIS_H
