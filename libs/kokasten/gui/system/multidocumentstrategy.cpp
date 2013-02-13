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

#include "multidocumentstrategy.h"
#include "multidocumentstrategy_p.h"


namespace Kasten2
{

MultiDocumentStrategy::MultiDocumentStrategy( DocumentManager* documentManager,
                                              ViewManager* viewManager )
  : AbstractDocumentStrategy( new MultiDocumentStrategyPrivate(this,
                                                               documentManager,
                                                               viewManager) )
{
    Q_D( MultiDocumentStrategy );

    d->init();
}

QList<AbstractDocument*> MultiDocumentStrategy::documents() const
{
    Q_D( const MultiDocumentStrategy );

    return d->documents();
}

QStringList MultiDocumentStrategy::supportedRemoteTypes() const
{
    Q_D( const MultiDocumentStrategy );

    return d->supportedRemoteTypes();
}

bool MultiDocumentStrategy::canClose( AbstractDocument* document ) const
{
    Q_D( const MultiDocumentStrategy );

    return d->canClose( document );
}

bool MultiDocumentStrategy::canCloseAllOther( AbstractDocument* document ) const
{
     Q_D( const MultiDocumentStrategy );

   return d->canCloseAllOther( document );
}

bool MultiDocumentStrategy::canCloseAll() const
{
    Q_D( const MultiDocumentStrategy );

    return d->canCloseAll();
}

void MultiDocumentStrategy::createNew()
{
    Q_D( MultiDocumentStrategy );

    d->createNew();
}

void MultiDocumentStrategy::createNewFromClipboard()
{
    Q_D( MultiDocumentStrategy );

    d->createNewFromClipboard();
}

void MultiDocumentStrategy::createNewWithGenerator( AbstractModelDataGenerator* generator )
{
    Q_D( MultiDocumentStrategy );

    d->createNewWithGenerator( generator );
}

void MultiDocumentStrategy::load( const KUrl& url )
{
    Q_D( MultiDocumentStrategy );

    d->load( url );
}

void MultiDocumentStrategy::closeAll()
{
    Q_D( MultiDocumentStrategy );

    d->closeAll();
}

void MultiDocumentStrategy::closeAllOther( AbstractDocument* document )
{
    Q_D( MultiDocumentStrategy );

    d->closeAllOther( document );
}

void MultiDocumentStrategy::closeDocument( AbstractDocument* document )
{
    Q_D( MultiDocumentStrategy );

    d->closeDocument( document );
}


MultiDocumentStrategy::~MultiDocumentStrategy()
{
}

}
