/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDChart library.
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
#ifndef __KDCHARTLEGENDSERIALIZERFACTORY_H__
#define __KDCHARTLEGENDSERIALIZERFACTORY_H__

/** \file KDChartLegendSerializerFactory.h
  */

#include <KDChartAbstractSerializerFactory>

namespace KDChart {

    class AbstractSerializer;

    class KDCHARTSERIALIZER_EXPORT LegendSerializerFactory : public AbstractSerializerFactory
    {
        Q_OBJECT
        Q_DISABLE_COPY( LegendSerializerFactory )

        KDCHART_DECLARE_PRIVATE_BASE_POLYMORPHIC( LegendSerializerFactory )

    public:
        LegendSerializerFactory( QObject* parent = 0 );
        virtual ~LegendSerializerFactory();

        AbstractSerializer* instance( const  QString& className ) const;
        QObject* createNewObject( const QString& className ) const;
    };

} // end of namespace

#endif
