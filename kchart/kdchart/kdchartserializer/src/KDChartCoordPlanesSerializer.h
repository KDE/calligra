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
#ifndef __KDCHARTCOORPLANESSERIALIZER_H__
#define __KDCHARTCOORPLANESSERIALIZER_H__

/** \file KDChartCoordPlanesSerializer.h
    \brief Auxiliary methods for reading/saving KD Chart data and configuration in streams.
  */

#include "KDChartAttributesSerializer.h"

#include <KDChartAbstractSerializer>

#include <KDChartCartesianCoordinatePlane>
#include <KDChartPolarCoordinatePlane>
#include <KDChartChart>

namespace KDChart {

    class DiagramsSerializer;

    class KDCHARTSERIALIZER_EXPORT CoordPlanesSerializer : public QObject, public AbstractSerializer
    {
        Q_OBJECT
        Q_DISABLE_COPY( CoordPlanesSerializer )

        KDCHART_DECLARE_PRIVATE_BASE_POLYMORPHIC( CoordPlanesSerializer )

    public:
        /**
         * If the model is set to a non-zero value before parsePlane()
         * is called, then this model will be assigned to any diagrams
         * that will be created by parsePlane().
         * 
         * The model could be changed via setModel, but this will not affect
         * any digrams that have been parsed before setModel was called.
         * 
         * \sa setModel
         */
        explicit CoordPlanesSerializer(QAbstractItemModel * model = 0 );
        virtual ~CoordPlanesSerializer();

        /** reimp */
        void saveElement( QDomDocument& doc, QDomElement& e, const QObject* obj ) const;
        /** reimp */
        bool parseElement( const QDomElement& container, QObject* ptr ) const;


        /**
         * \brief Set the data model to be assigned to diagrams created by parsePlane().
         * 
         * This will affect new diagrams instantiated after setModel was
         * called, but it will not change any data model assignments that
         * were made before setModel was called.
         *
         * \note If you do not call setModel nor specify a model with the
         * Serializer's constructor, then you need to manually call the
         * setModel methods of all of the diagrams after the serializer has
         * finished parsing. File \c examples/complexLayout/mainwindow.cpp
         * illustrates a way how to do that.
         */
        void setModel(QAbstractItemModel * model);

       /**
         * Parse the coordinate-plane element, and return a AbstractCoordinatePlane* in \c planePtr
         * if the respective coordinate-plane was found in the list of global elements.
         *
         * This method is called transparently by the Serializer, so you should
         * not need to call it explicitely.
         *
         * In case still want to call it just make sure that you have called
         * \c KDChart::SerializeCollector::instance()->initializeParsedGlobalPointers()
         * \em before invoking this method, or it will stop parsing and return false.
        */
        bool parsePlane(
                const QDomNode& rootNode,
                const QDomNode& pointerNode,
                AbstractCoordinatePlane*& planePtr )const;
        void savePlanes(
                QDomDocument& doc,
                QDomElement& e,
                const CoordinatePlaneList& planes,
                const QString& title )const;

private:

    public:
        const QString globalList()const;
    };

} // end of namespace

#endif
