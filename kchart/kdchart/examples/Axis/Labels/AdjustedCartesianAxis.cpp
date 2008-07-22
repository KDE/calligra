/****************************************************************************
 ** Copyright (C) 2006 Klarävdalens Datakonsult AB.  All rights reserved.
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


#include "AdjustedCartesianAxis.h"


AdjustedCartesianAxis::AdjustedCartesianAxis ( KDChart::AbstractCartesianDiagram* diagram )
    : CartesianAxis ( diagram )
    , m_lowerBound(0.0)
    , m_upperBound(0.0)
{
    // this bloc left empty intentionally
}

const QString AdjustedCartesianAxis::customizedLabel( const QString& label )const
{
    bool bOK;
    double val = label.toDouble( & bOK );
    if( bOK ){
        if( val < m_lowerBound )
            return tr("LOW");
        else if( val > m_upperBound )
            return tr("HIGH");
    }
    return label;
}
