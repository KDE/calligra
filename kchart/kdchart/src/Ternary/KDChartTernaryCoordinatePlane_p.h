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

#ifndef KDCHARTTERNARYCOORDINATEPLANE_P_H
#define KDCHARTTERNARYCOORDINATEPLANE_P_H

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

#include <QRectF>

#include "KDChartTernaryGrid.h"
#include "KDChartAbstractCoordinatePlane_p.h"

#include <KDABLibFakes>

namespace KDChart {

    class TernaryAxis;

    /**
     * \internal
     */

    class TernaryCoordinatePlane::Private : public AbstractCoordinatePlane::Private
    {
        friend class TernaryCoordinatePlane;

    public:
        explicit Private();

        virtual ~Private() {
            // grid is delete in base class dtor
        }

        virtual void initialize()
        {
            grid = new TernaryGrid();
            xUnit = 0.0;
            yUnit = 0.0;
        }

        QList<TernaryAxis*> axes;

        TextAttributes labelAttributes;

        // the diagram is drawn within this rectangle, which is within
        // this widget:
        QRectF diagramRectContainer;
        // this is the "frame" of the plot area
        QRectF diagramRect;
        // multiply m_xUnit with a [0..1] value to get an isometric
        // widget coordinate
        double xUnit;
        // same for y:
        double yUnit;

    };

    KDCHART_IMPL_DERIVED_PLANE(TernaryCoordinatePlane, AbstractCoordinatePlane)
}

#endif /* KDCHARTTERNARYCOORDINATEPLANE_P_H */
