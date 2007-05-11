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

#ifndef KDCHARTCARTESIANAXIS_P_H
#define KDCHARTCARTESIANAXIS_P_H

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

#include "KDChartCartesianAxis.h"
#include "KDChartAbstractCartesianDiagram.h"
#include "KDChartAbstractAxis_p.h"

#include <KDABLibFakes>


namespace KDChart {

/**
  * \internal
  */
class CartesianAxis::Private : public AbstractAxis::Private
{
    friend class CartesianAxis;

public:
    Private( AbstractCartesianDiagram* diagram, AbstractAxis* axis )
        : AbstractAxis::Private( diagram, axis )
        , useDefaultTextAttributes( true )
    {};
    ~Private() {};

private:
    QString titleText;
    TextAttributes titleTextAttributes;
    bool useDefaultTextAttributes;
    Position position;
    QRect geometry;
};

inline CartesianAxis::CartesianAxis( Private * p, AbstractDiagram* diagram )
    : AbstractAxis( p, diagram )
{
    init();
}
inline CartesianAxis::Private * CartesianAxis::d_func()
{ return static_cast<Private*>( AbstractAxis::d_func() ); }
inline const CartesianAxis::Private * CartesianAxis::d_func() const
{ return static_cast<const Private*>( AbstractAxis::d_func() ); }

}

#endif
