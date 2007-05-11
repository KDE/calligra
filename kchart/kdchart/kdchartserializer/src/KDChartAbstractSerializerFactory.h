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
#ifndef __KDCHARTABSTRACTSERIALIZERFACTORY_H__
#define __KDCHARTABSTRACTSERIALIZERFACTORY_H__

/** \file KDChartAbstractSerializerFactory.h
  */

#include <KDChartGlobal>
#include "kdchartserializer_export.h"

#include <QObject>

namespace KDChart {

    class AbstractSerializer;

    class KDCHARTSERIALIZER_EXPORT AbstractSerializerFactory : public QObject
    {
        Q_DISABLE_COPY( AbstractSerializerFactory )

        KDCHART_DECLARE_PRIVATE_BASE_POLYMORPHIC( AbstractSerializerFactory );

    public:
        AbstractSerializerFactory( QObject* parent = 0 );
        virtual ~AbstractSerializerFactory();

        virtual AbstractSerializer* instance( const QString& className ) const = 0;
        virtual QObject* createNewObject( const QString& className ) const = 0;
    };

} // end of namespace

#endif
