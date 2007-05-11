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
#ifndef __KDCHARTSERIALIZERREGISTRATOR_H__
#define __KDCHARTSERIALIZERREGISTRATOR_H__

/** \file KDChartSerializerRegistrator.h
  */

#include <KDChartSerializer>
#include <KDChartAbstractSerializerFactory>

#include <QSet>

namespace KDChart {

    template< class S, class T >
    class SerializerRegistrator : public AbstractSerializerFactory
    {
    public:
        SerializerRegistrator( QObject* parent = 0 )
            : AbstractSerializerFactory( parent )
            , m_instance( new S )
        {
        }

        ~SerializerRegistrator()
        {
            if( m_instance != 0 )
                delete m_instance;
        }

        AbstractSerializer* instance( const QString& className ) const
        {
            if( className == QString::fromLatin1( T::staticMetaObject.className() ) )
                return m_instance;
            return 0;
        }

        QObject* createNewObject( const QString& className ) const
        {
            if( className == QString::fromLatin1( T::staticMetaObject.className() ) )
                return new T;
            return 0;
        }

    private:
        S* m_instance;
    };

    template< class S, class T >
    void registerElementSerializer( QObject* parent )
    {
        Serializer::registerElementSerializerFactory< T >( new SerializerRegistrator< S, T >( parent ) );
    }

    template< class S, class T >
    void unregisterElementSerializer()
    {
        AbstractSerializerFactory* f = Serializer::elementSerializerFactory< T >();
        Serializer::unregisterElementSerializerFactory< T >();
        delete f;
    }

} // end of namespace

#endif
