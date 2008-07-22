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
#ifndef __KDCHARTAXESSERIALIZER_H__
#define __KDCHARTAXESSERIALIZER_H__

/** \file KDChartAxesSerializer.h
    \brief Auxiliary methods for reading/saving KD Chart data and configuration in streams.
  */

#include "KDChartAttributesSerializer"

#include <KDChartAbstractSerializer>

#include <KDChartAbstractAxis>
#include <KDChartChart>

namespace KDChart {

    class KDCHARTSERIALIZER_EXPORT AxesSerializer : public QObject, public AbstractSerializer
    {
        Q_OBJECT

        Q_DISABLE_COPY( AxesSerializer )
        KDCHART_DECLARE_PRIVATE_BASE_POLYMORPHIC( AxesSerializer )

    public:
        explicit AxesSerializer();
        virtual ~AxesSerializer();

        /** reimp */
        void saveElement( QDomDocument& doc, QDomElement& e, const QObject* obj ) const;
        /** reimp */
        bool parseElement( const QDomElement& container, QObject* ptr ) const;

        void saveAxes( QDomDocument& doc, QDomElement& e,
                       const QList< const AbstractAxis* >& axes, const QString& title ) const;

        /**
         * Parse the axis-pointer element, and return an AbstractAxis* in \c axisPtr
         * if the respective axis was found in the list of global elements.
         *
         * This method is called transparently by the Serializer, so you should
         * not need to call it explicitely.
         *
         * In case still want to call it just make sure that you have called
         * \c KDChart::SerializeCollector::instance()->initializeParsedGlobalPointers()
         * \em before invoking this method, or it will stop parsing and return false.
         */
        bool parseAxis(
                const QDomNode& rootNode,
                const QDomNode& pointerNode,
                AbstractAxis*& axisPtr )const;

        bool parseAbstractAxis(
                const QDomElement& container,
                AbstractAxis& axis )const;
    };

} // end of namespace

#endif
