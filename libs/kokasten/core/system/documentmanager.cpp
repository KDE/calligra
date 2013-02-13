/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2006-2007,2009,2011 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "documentmanager.h"

// lib
#include <abstractdocument.h>
// KDE
#include <KUrl>
// Qt
#include <QtCore/QMutableListIterator>
#include <QtCore/QStringList>

// temporary
#include "documentcreatemanager.h"
#include "documentsyncmanager.h"
#include "modelcodecmanager.h"


namespace Kasten2
{

static int lastDocumentId = 0;


DocumentManager::DocumentManager()
  : mCreateManager( new DocumentCreateManager(this) ),
    mSyncManager( new DocumentSyncManager(this) ),
    mCodecManager( new ModelCodecManager(this) )
{
}

QList<AbstractDocument*> DocumentManager::documents() const { return mList; }
bool DocumentManager::isEmpty() const { return mList.isEmpty(); }

void DocumentManager::addDocument( AbstractDocument* document )
{
    // TODO: check for double insert
    document->setId( QString::number(++lastDocumentId) );
    mList.append( document );
    // TODO: only emit if document was not included before
    QList<AbstractDocument*> addedDocuments;
    addedDocuments.append( document );
    emit added( addedDocuments );
}

void DocumentManager::closeDocument( AbstractDocument* document )
{
    QMutableListIterator<AbstractDocument*> iterator( mList );

    if( iterator.findNext(document) )
    {
    // TODO: first check if unsaved and ask, only then close

        iterator.remove();

        QList<AbstractDocument*> closedDocuments;
        closedDocuments.append( document );
        emit closing( closedDocuments );

        delete document;
    }
}

void DocumentManager::closeDocuments( const QList<AbstractDocument*>& documents )
{
    // TODO: optimize
    foreach( AbstractDocument* document, documents )
        mList.removeOne( document );

    emit closing( documents );

    foreach( AbstractDocument* document, documents )
        delete document;
}

void DocumentManager::closeAll()
{
    // TODO: is it better for remove the document from the list before emitting closing(document)?
    // TODO: or better emit close(documentList)? who would use this?
    QList<AbstractDocument*> closedDocuments = mList;
    mList.clear();

    emit closing( closedDocuments );

    foreach( AbstractDocument* document, closedDocuments )
        delete document;
}

void DocumentManager::closeAllOther( AbstractDocument* keptDocument )
{
    // TODO: is it better for remove the document from the list before emitting closing(document)?
    // TODO: or better emit close(documentList)? who would use this?
    QList<AbstractDocument*> closedDocuments = mList;
    closedDocuments.removeOne( keptDocument );

    mList.clear();
    mList.append( keptDocument );

    emit closing( closedDocuments );

    foreach( AbstractDocument* document, closedDocuments )
    {
        delete document;
    }
}

bool DocumentManager::canClose( AbstractDocument* document )
{
    return mSyncManager->canClose( document );
}

bool DocumentManager::canClose( const QList<AbstractDocument*>& documents )
{
    bool canClose = true;

    foreach( AbstractDocument* document, documents )
    {
        if( ! mSyncManager->canClose(document) )
        {
            canClose = false;
            break;
        }
    }

    return canClose;
}

bool DocumentManager::canCloseAll()
{
    bool canCloseAll = true;

    foreach( AbstractDocument* document, mList )
    {
        if( !mSyncManager->canClose(document) )
        {
            canCloseAll = false;
            break;
        }
    }

    return canCloseAll;
}

bool DocumentManager::canCloseAllOther( AbstractDocument* keptDocument )
{
    bool canCloseAll = true;

    foreach( AbstractDocument* document, mList )
    {
        if( ( document != keptDocument ) &&
            ! mSyncManager->canClose(document) )
        {
            canCloseAll = false;
            break;
        }
    }

    return canCloseAll;
}

void DocumentManager::requestFocus( AbstractDocument* document )
{
    emit focusRequested( document );
}

DocumentManager::~DocumentManager()
{
    // TODO: emit signal here, too?
    qDeleteAll( mList );

    delete mCreateManager;
    delete mSyncManager;
    delete mCodecManager;
} //TODO: destroy all documents?

}
