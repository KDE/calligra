/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2011 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include "singledocumentstrategy.h"
#include "singledocumentstrategy_p.h"


namespace Kasten2
{

SingleDocumentStrategy::SingleDocumentStrategy( DocumentManager* documentManager,
                                                ViewManager* viewManager )
  : AbstractDocumentStrategy( new SingleDocumentStrategyPrivate(this,
                                                                documentManager,
                                                                viewManager) )
{
    Q_D( SingleDocumentStrategy );

    d->init();
}

QList<AbstractDocument*> SingleDocumentStrategy::documents() const
{
    Q_D( const SingleDocumentStrategy );

    return d->documents();
}

QStringList SingleDocumentStrategy::supportedRemoteTypes() const
{
    Q_D( const SingleDocumentStrategy );

    return d->supportedRemoteTypes();
}

bool SingleDocumentStrategy::canClose( AbstractDocument* document ) const
{
    Q_D( const SingleDocumentStrategy );

    return d->canClose( document );
}

bool SingleDocumentStrategy::canCloseAllOther( AbstractDocument* document ) const
{
    Q_D( const SingleDocumentStrategy );

    return d->canCloseAllOther( document );
}

bool SingleDocumentStrategy::canCloseAll() const
{
    Q_D( const SingleDocumentStrategy );

    return d->canCloseAll();
}

void SingleDocumentStrategy::createNew()
{
    Q_D( SingleDocumentStrategy );

    d->createNew();
}

void SingleDocumentStrategy::createNewFromClipboard()
{
    Q_D( SingleDocumentStrategy );

    d->createNewFromClipboard();
}

void SingleDocumentStrategy::createNewWithGenerator( AbstractModelDataGenerator* generator )
{
    Q_D( SingleDocumentStrategy );

    d->createNewWithGenerator( generator );
}

void SingleDocumentStrategy::load( const KUrl& url )
{
    Q_D( SingleDocumentStrategy );

    d->load( url );
}

void SingleDocumentStrategy::closeAll()
{
    Q_D( SingleDocumentStrategy );

    d->closeAll();
}

void SingleDocumentStrategy::closeAllOther( AbstractDocument* document )
{
    Q_D( SingleDocumentStrategy );

    d->closeAllOther( document );
}

void SingleDocumentStrategy::closeDocument( AbstractDocument* document )
{
    Q_D( SingleDocumentStrategy );

    d->closeDocument( document );
}

SingleDocumentStrategy::~SingleDocumentStrategy()
{
}

}
