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

#include <KDChartIdMapper.h>
#include <KDChartIdMapper_p.h>

#include <QDebug>

#define d d_func()

using namespace KDChart;

IdMapper::Private::Private( IdMapper* qq )
    : q( qq )
{
}

IdMapper::Private::~Private() {}

IdMapper::IdMapper()
    : _d( new Private( this ) )
{
    d->m_counterTag = QString::fromLatin1( "::C_O_U_N_T_E_R" );
}

IdMapper::~IdMapper()
{
    clear();
    delete _d; _d = 0;
}

void IdMapper::init()
{
}

void IdMapper::clear()
{
    // this space left empty intentionally
}

IdMapper* IdMapper::instance()
{
    static IdMapper instance;
    return &instance;
}

QString IdMapper::findOrMakeName(
        const void* id,
        const QString& baseName,
        bool& wasFound,
        bool useMapOfKnownElements )
{
    QString name = findName( id, useMapOfKnownElements );
    wasFound = ! name.isNull();

    if( ! wasFound ){
        QMap<const void*, QString>& map = useMapOfKnownElements ? d->m_mapOfKnownElements : d->m_unresolvedMap;
        // check if we have a counter stored already - if not we add one
        int counter = 1;
        QString counterName( baseName + d->m_counterTag );
        QMapIterator<const void*, QString> i( map );
        while( i.hasNext() ) {
            i.next();
            if( i.value() == counterName ){
                // we may cast away constness, since this is not
                // an external pointer but our own auxiliary counter entry
                int* storedCount = const_cast<int*>( static_cast<const int*>( i.key() ) );
                (*storedCount)++;
                counter = *storedCount;
            }
        }
        if( counter == 1 ){
            int* p = new int;
            *p = counter;
            map[ p ] = counterName;
        }

        // store a new name using the counter value, and return it
        name = baseName + ":" + QString::number( counter );
        map[ id ] = name;
    }
    return name;
}


QString IdMapper::findName( const void* id,
                            bool useMapOfKnownElements )const
{
    const QMap<const void*, QString>& map = useMapOfKnownElements ? d->m_mapOfKnownElements : d->m_unresolvedMap;
    if( map.contains( id ) )
        return map.value( id );
    return QString();
}



const void* IdMapper::findId( const QString& name,
                              bool useMapOfKnownElements )const
{
    const QMap<const void*, QString>& map = useMapOfKnownElements ? d->m_mapOfKnownElements : d->m_unresolvedMap;
    QMapIterator<const void*, QString> i( map );
    while( i.hasNext() ) {
        i.next();
        if( i.value() == name )
            return i.key();
    }
    return 0;
}

const QMap<const void*, QString> IdMapper::unresolvedMap()const
{
    return d->m_unresolvedMap;
}

void IdMapper::debugOut()const
{
    qDebug() << "IdMapper::debugOut():";
    qDebug() << "map of known elements:";
    QMapIterator<const void*, QString> i( d->m_mapOfKnownElements );
    while( i.hasNext() ) {
        i.next();
        qDebug() << "key:" << i.key() << "pointer:" << i.value();
    }
    qDebug() << "unresolved map:";
    QMapIterator<const void*, QString> i2( d->m_unresolvedMap );
    while( i2.hasNext() ) {
        i2.next();
        qDebug() << "key:" << i2.key() << "pointer:" << i2.value();
    }
}
