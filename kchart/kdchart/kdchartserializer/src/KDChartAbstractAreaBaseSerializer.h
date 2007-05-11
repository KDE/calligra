/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDChart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KDChart1 licenses may use this file in
 ** accordance with the KDChart1 Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.klaralvdalens-datakonsult.se/?page=products for
 **   information about KDChart1 Commercial License Agreements.
 **
 ** Contact info@klaralvdalens-datakonsult.se if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#ifndef __KDCHARTABSTRACTAREABASESERIALIZER_H__
#define __KDCHARTABSTRACTAREABASESERIALIZER_H__

/** \file KDChartAbstractAreaBaseSerializer.h
    \brief Auxiliary methods for reading/saving KD Chart data and configuration in streams.
  */

#include <KDChartAbstractAreaBase.h>
#include <QDomDocument>

#include "kdchartserializer_export.h"

namespace KDChart {

    class KDCHARTSERIALIZER_EXPORT AbstractAreaBaseSerializer
    {
        Q_DISABLE_COPY( AbstractAreaBaseSerializer )

        KDCHART_DECLARE_PRIVATE_BASE_POLYMORPHIC( AbstractAreaBaseSerializer )
    public:
        AbstractAreaBaseSerializer();
        virtual ~AbstractAreaBaseSerializer();

        static bool parseAbstractAreaBase(
                const QDomElement& e, AbstractAreaBase& area );
        static void saveAbstractAreaBase(
                QDomDocument& doc,
                QDomElement& e,
                const AbstractAreaBase& area,
                const QString& title );
    };

} // end of namespace

#endif
