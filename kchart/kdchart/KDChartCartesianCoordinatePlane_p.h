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

#ifndef KDCHARTCARTESIANCOORDINATEPLANE_P_H
#define KDCHARTCARTESIANCOORDINATEPLANE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the KD Chart API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "KDChartAbstractCoordinatePlane_p.h"
#include "CartesianCoordinateTransformation.h"
#include "KDChartCartesianGrid.h"

#include <KDABLibFakes>


namespace KDChart {

/**
 * \internal
 */
class CartesianCoordinatePlane::Private : public AbstractCoordinatePlane::Private
{
    friend class CartesianCoordinatePlane;
public:
    explicit Private();
    virtual ~Private() { }

    virtual void initialize()
    {
        bPaintIsRunning   = false;
        bNextPaintPending = false;
        coordinateTransformation.axesCalcModeX = Linear;
        coordinateTransformation.axesCalcModeY = Linear;
        grid = new CartesianGrid();
    }


    // the coordinate plane will calculate the coordinate transformation:
    CoordinateTransformation coordinateTransformation;

    bool bPaintIsRunning;
    bool bNextPaintPending;

    // true after setGridAttributes( Qt::Orientation ) was used,
    // false if resetGridAttributes( Qt::Orientation ) was called
    bool hasOwnGridAttributesHorizontal;
    bool hasOwnGridAttributesVertical;

    // true after the first resize event came in
    // bool initialResizeEventReceived;

    // true if the coordinate plane scales isometrically
    bool isometricScaling;

    GridAttributes gridAttributesHorizontal;
    GridAttributes gridAttributesVertical;

    qreal horizontalMin;
    qreal horizontalMax;
    qreal verticalMin;
    qreal verticalMax;
};


KDCHART_IMPL_DERIVED_PLANE(CartesianCoordinatePlane, AbstractCoordinatePlane)

}

#endif /* KDCHARTBARDIAGRAM_P_H */
