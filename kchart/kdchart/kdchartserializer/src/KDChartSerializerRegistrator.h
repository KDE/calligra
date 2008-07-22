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
        Serializer::registerElementSerializerFactory( T::staticMetaObject.className(), new SerializerRegistrator< S, T >( parent ) );
    }

    template< class S, class T >
    void unregisterElementSerializer()
    {
        AbstractSerializerFactory* f = Serializer::elementSerializerFactory( QString::fromLatin1( T::staticMetaObject.className() ) );
        Serializer::unregisterElementSerializerFactory( T::staticMetaObject.className() );
        delete f;
    }

} // end of namespace

#endif
