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
#ifndef __KDCHARTDIAGRAMSSERIALIZER_H__
#define __KDCHARTDIAGRAMSSERIALIZER_H__

/** \file KDChartDiagramsSerializer.h
    \brief Auxiliary methods for reading/saving KD Chart data and configuration in streams.
  */

#include "KDChartAbstractSerializer"
#include "KDChartLegendsSerializer"

#include "KDChartAttributesSerializer"

#include <KDChartChart>
#include <KDChartLineDiagram>
#include <KDChartBarDiagram>
#include <KDChartPieDiagram>
#include <KDChartPolarDiagram>
#include <KDChartRingDiagram>
#include <KDChartPlotter>

namespace KDChart {

    class CoordPlanesSerializer;
    class AxesSerializer;
    class AttributesModelSerializer;

    class KDCHARTSERIALIZER_EXPORT DiagramsSerializer : public QObject, public AbstractSerializer
    {
        Q_OBJECT
        Q_DISABLE_COPY( DiagramsSerializer )

        KDCHART_DECLARE_PRIVATE_BASE_POLYMORPHIC( DiagramsSerializer )

        friend class Private;
        friend class CoordPlanesSerializer;
        friend class LegendsSerializer;
        friend class LegendsSerializer::Private;

    public:
        explicit DiagramsSerializer( CoordPlanesSerializer* coordS = 0 );
        virtual ~DiagramsSerializer();

        /** reimp */
        void saveElement( QDomDocument& doc, QDomElement& e, const QObject* obj ) const;

        /**
         * reimp
         * 
         * Parse the diagram element, and store it in \c ptr.
         *
         * Make sure that you have called
         * \c KDChart::SerializeCollector::instance()->initializeParsedGlobalPointers()
         * before invoking this method, or it will stop parsing and return false.
         */
        bool parseElement( const QDomElement& container, QObject* ptr ) const;

       /**
         * Parse the diagram element, and return a AbstractDiagram* in \c diagramPtr
         * if the respective diagram was found in the list of global elements.
         *
         * This method is called transparently by the Serializer, so you should
         * not need to call it explicitely.
         *
         * In case still want to call it just make sure that you have called
         * \c KDChart::SerializeCollector::instance()->initializeParsedGlobalPointers()
         * \em before invoking this method, or it will stop parsing and return false.
        */
        bool parseDiagram( const QDomNode& rootNode, const QDomNode& pointerNode,
                           AbstractDiagram*& diagramPtr ) const;
        void saveDiagrams( QDomDocument& doc, QDomElement& e,
                           const ConstAbstractDiagramList& diags, const QString& title ) const;
    };

} // end of namespace

#endif
