/* This file is part of the KDE project
   Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#include "KPrPageEffectFactory.h"

#define BOOST_MULTI_INDEX_DISABLE_SERIALIZATION
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include "KPrPageEffectStrategy.h"

struct SmilData : boost::multi_index::composite_key<
    KPrPageEffectStrategy,
    boost::multi_index::const_mem_fun<KPrPageEffectStrategy, const QString &, &KPrPageEffectStrategy::smilSubType>,
    boost::multi_index::const_mem_fun<KPrPageEffectStrategy, bool, &KPrPageEffectStrategy::reverse>
>
{
};


typedef boost::multi_index_container<
    KPrPageEffectStrategy *,
    boost::multi_index::indexed_by<
        boost::multi_index::ordered_unique<
            boost::multi_index::const_mem_fun<KPrPageEffectStrategy, KPrPageEffect::SubType, &KPrPageEffectStrategy::subType>
        >,
        boost::multi_index::ordered_unique<
            SmilData
        >
    >
> EffectStrategies;

struct KPrPageEffectFactory::Private
{
    Private( const QString & id, const QString & name )
    : id( id )
    , name( name )
    {}

    ~Private()
    {
        // TODO delete strategies
    }

    QString id;
    QString name;
    QList<KPrPageEffect::SubType> subTypes;
    EffectStrategies strategies;
};

KPrPageEffectFactory::KPrPageEffectFactory( const QString & id, const QString & name )
: d( new Private( id, name ) )
{
}

KPrPageEffectFactory::~KPrPageEffectFactory()
{
    delete d;
}

KPrPageEffect * KPrPageEffectFactory::createPageEffect( const Properties & properties ) const
{
    KPrPageEffectStrategy * strategy = 0;

    EffectStrategies::iterator it( d->strategies.find( properties.subType ) );

    if ( it != d->strategies.end() ) {
        strategy = *it;
    }
    Q_ASSERT( strategy );

    return new KPrPageEffect( properties.duration, d->id, strategy );
}

KPrPageEffect * KPrPageEffectFactory::createPageEffect( const KoXmlElement & element ) const
{
    // TODO
    return 0;
}

QString KPrPageEffectFactory::id() const
{
    return d->id;
}

QString KPrPageEffectFactory::name() const
{
    return d->name;
}

QList<KPrPageEffect::SubType> KPrPageEffectFactory::subTypes() const
{
    return d->subTypes;
}

void KPrPageEffectFactory::addStrategy( KPrPageEffectStrategy * strategy )
{
    bool inserted = d->strategies.insert( strategy ).second;
    Q_ASSERT( inserted == true );
    d->subTypes.append( strategy->subType() );
}
