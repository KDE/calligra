/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
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
#ifndef __KDCHARTDIAGRAMSERIALIZERFACTORY_P_H__
#define __KDCHARTDIAGRAMSERIALIZERFACTORY_P_H__

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

#include <KDChartDiagramSerializerFactory.h>

#include <QMap>

namespace KDChart
{
    class DiagramsSerializer;
}

/**
 * \internal
 */
class KDChart::DiagramSerializerFactory::Private
{
    friend class ::KDChart::DiagramSerializerFactory;
    DiagramSerializerFactory* const q;
public:
    explicit Private( DiagramSerializerFactory * qq );
    ~Private(); // non-virtual, since nothing inherits this

protected:
    mutable KDChart::DiagramsSerializer* m_instance;
};


#endif // KDChartDiagramSerializerFactory_p_H
