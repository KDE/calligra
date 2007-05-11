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
#ifndef __KDCHART_ID_MAPPER_H__
#define __KDCHART_ID_MAPPER_H__

#include <QString>
#include <QMap>

#include <KDChartGlobal>

namespace KDChart {

    /**
     * Auxiliary class used by the KDChart::*Serializer classes.
     *
     * Normally there should be no need to call any of these methods yourself.
     *
     * It is all handled automatically, if you use the main KDChart::Serializer
     * class as entry point of your serialization operations.
     */
    class IdMapper
    {
        //friend class Serializer; // is allowed to delete the mapper instance

        Q_DISABLE_COPY( IdMapper )

        KDCHART_DECLARE_PRIVATE_BASE_POLYMORPHIC( IdMapper )

    public:
        static IdMapper* instance();

        IdMapper();
        virtual ~IdMapper();

    public:
        /** Returns the name that was stored for this pointer.
         *
         * If none was stored before, a new name is created
         * ( basename + ":" + a current number ),
         * the entry is stored and the name is returned.
         */
        QString findOrMakeName( const void* id,
                                const QString& baseName,
                                bool& wasFound,
                                bool useMapOfKnownElements=true );
        /** Returns the name that was stored for this pointer.
         *
         * If none was stored before, a QString() is returned.
         */
        QString findName( const void* id,
                          bool useMapOfKnownElements=true )const;
        /** Returns the pointer that was stored for the given name.
         *
         * If none was stored, it returns zero.
         */
        const void* findId( const QString& name,
                            bool useMapOfKnownElements=true )const;

        /** Delete all collected data.
         */
        void clear();

        /** Return the list of unresolved elements
         */
        const QMap<const void*, QString> unresolvedMap()const;

        /** Prints all stored names and their respective pointers via qDebug().
          */
        void debugOut()const;
    };

} // end of namespace

#endif
