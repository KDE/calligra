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
#ifndef __KDCHARTLEGENDSSERIALIZER_H__
#define __KDCHARTLEGENDSSERIALIZER_H__

/** \file KDChartLegendsSerializer.h
    \brief Auxiliary methods for reading/saving KD Chart data and configuration in streams.
  */

#include <KDChartAttributesSerializer>

#include <KDChartAbstractSerializer>

#include <KDChartLegend>
#include <KDChartChart>

namespace KDChart {

    class KDCHARTSERIALIZER_EXPORT LegendsSerializer : public QObject, public AbstractSerializer
    {
        Q_OBJECT
        Q_DISABLE_COPY( LegendsSerializer )

        KDCHART_DECLARE_PRIVATE_BASE_POLYMORPHIC( LegendsSerializer )

        friend class Serializer;
        friend class DiagramsSerializer;

    public:
        LegendsSerializer( QObject* parent = 0 );
        virtual ~LegendsSerializer();

        /** reimp */
        void saveElement( QDomDocument& doc, QDomElement& e, const QObject* obj ) const;

        /**
         * reimp
         * 
         * Parse the legend element, and store it in \c ptr.
         *
         * Make sure that you have called
         * \c KDChart::SerializeCollector::instance()->initializeParsedGlobalPointers()
         * before invoking this method, or it will stop parsing and return false.
         */
        bool parseElement( const QDomElement& container, QObject* ptr ) const;

        static void saveLegends( QDomDocument& doc, QDomElement& e,
                                 const LegendList& planes, const QString& title );

        /**
         * Parse the legend element, and return a Legend* in \c legend
         * if the respective legend was found in the list of global elements.
         *
         * This method is called transparently by the Serializer, so you should
         * not need to call it explicitely.
         *
         * In case still want to call it just make sure that you have called
         * \c KDChart::SerializeCollector::instance()->initializeParsedGlobalPointers()
         * \em before invoking this method, or it will stop parsing and return false.
        */
        static bool parseLegend( const QDomNode& rootNode, const QDomNode& pointerNode,
                                 Legend*& legend );
    };

} // end of namespace

#endif
