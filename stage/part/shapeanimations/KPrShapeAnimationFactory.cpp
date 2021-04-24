/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "KPrShapeAnimationFactory.h"

struct KPrShapeAnimationFactory::Private
{
    Private( const QString & id, const QString & name )
    : id( id )
    , name( name )
    {}

    QString id;
    QString name;
};

KPrShapeAnimationFactory::KPrShapeAnimationFactory( const QString & id, const QString & name )
: d( new Private( id, name ) )
{
}

KPrShapeAnimationFactory::~KPrShapeAnimationFactory()
{
    delete d;
}

QString KPrShapeAnimationFactory::id() const
{
    return d->id;
}

QString KPrShapeAnimationFactory::name() const
{
    return d->name;
}

