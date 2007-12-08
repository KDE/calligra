/* This file is part of the KDE project
   Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>

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

struct KPrPageEffectFactory::Private
{
    Private( const QString & id, const QString & name, const QList<KPrPageEffect::SubType> & subTypes )
    : id( id )
    , name( name )
    , subTypes( subTypes )
    {}

    QString id;
    QString name;
    QList<KPrPageEffect::SubType> subTypes;
};

KPrPageEffectFactory::KPrPageEffectFactory( const QString & id, const QString & name,
                                            const QList<KPrPageEffect::SubType> & subTypes )
: d( new Private( id, name, subTypes ) )
{
}

KPrPageEffectFactory::~KPrPageEffectFactory()
{
    delete d;
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
