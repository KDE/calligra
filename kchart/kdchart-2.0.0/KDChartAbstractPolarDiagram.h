/****************************************************************************
 ** Copyright (C) 2006 Klarälvdalens Datakonsult AB.  All rights reserved.
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
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#ifndef KDCHARTABSTRACTPOLARDIAGRAM_H
#define KDCHARTABSTRACTPOLARDIAGRAM_H

#include "KDChartPolarCoordinatePlane.h"
#include "KDChartAbstractDiagram.h"

namespace KDChart {

    class GridAttributes;

    class AbstractPolarDiagram : public AbstractDiagram
    {
        Q_OBJECT
        Q_DISABLE_COPY( AbstractPolarDiagram )
        KDCHART_DECLARE_DERIVED_DIAGRAM( AbstractPolarDiagram, PolarCoordinatePlane )

    public:
        explicit AbstractPolarDiagram (
            QWidget* parent = 0, PolarCoordinatePlane* plane = 0 );
        virtual ~AbstractPolarDiagram() {}

        virtual double valueTotals () const = 0;
        virtual double numberOfValuesPerDataset() const = 0;
        virtual double numberOfGridRings() const = 0;

        const PolarCoordinatePlane * polarCoordinatePlane() const;

        int columnCount() const;
    };

}

#endif
