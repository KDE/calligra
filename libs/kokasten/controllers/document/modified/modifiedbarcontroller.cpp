/*
    This file is part of the Okteta Kasten module, made within the KDE community.

    Copyright 2009-2010,2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "modifiedbarcontroller.h"

// Kasten ui
#include <statusbar.h>
// Kasten core
#include <abstractmodelsynchronizer.h>
#include <abstractdocument.h>
// KDE
#include <KLocale>
#include <KIcon>
// Qt
#include <QtGui/QLabel>


namespace Kasten2
{
static const int modifiedPixmapWidth = 16;


ModifiedBarController::ModifiedBarController( StatusBar* statusBar )
  : mDocument( 0 )
  , mSynchronizer( 0 )
{
    // TODO: depend an statusbar height
    const QSize modifiedPixmapSize = QSize(modifiedPixmapWidth, modifiedPixmapWidth);

    mLocalStateLabel = new QLabel( statusBar );
    mLocalStateLabel->setAlignment( Qt::AlignCenter );
    mLocalStateLabel->setFixedSize( modifiedPixmapSize );
    statusBar->addWidget( mLocalStateLabel );

    mRemoteStateLabel = new QLabel( statusBar );
    mRemoteStateLabel->setAlignment( Qt::AlignCenter );
    mRemoteStateLabel->setFixedSize( modifiedPixmapSize );
    statusBar->addWidget( mRemoteStateLabel );

    setTargetModel( 0 );
}


void ModifiedBarController::setTargetModel( AbstractModel* model )
{
    AbstractDocument* newDocument = model ? model->findBaseModel<AbstractDocument*>() : 0;

    if( mDocument == newDocument )
        return;

    if( mDocument ) mDocument->disconnect( this );

    mDocument = newDocument;

    if( mDocument )
    {
        connect( mDocument, SIGNAL(synchronizerChanged(Kasten2::AbstractModelSynchronizer*)),
                            SLOT(onSynchronizerChanged(Kasten2::AbstractModelSynchronizer*)) );
    }

    mLocalStateLabel->setEnabled( mDocument );
    mRemoteStateLabel->setEnabled( mDocument );

    onSynchronizerChanged( mDocument ? mDocument->synchronizer() : 0 );
}


void ModifiedBarController::onContentFlagsChanged( ContentFlags contentFlags )
{
    const bool hasChanges = (contentFlags & ContentHasUnstoredChanges);
    onLocalSyncStateChanged( hasChanges ? LocalHasChanges : LocalInSync );
}

void ModifiedBarController::onLocalSyncStateChanged( LocalSyncState localSyncState )
{
    const bool isModified = (localSyncState == LocalHasChanges );

    // TODO: depend an statusbar height
    const QPixmap pixmap = isModified ?
        KIcon( QLatin1String("document-save") ).pixmap(modifiedPixmapWidth) :
        QPixmap();
    mLocalStateLabel->setPixmap( pixmap );

    mLocalStateLabel->setToolTip( isModified ?
        i18nc( "@tooltip the document is modified", "Modified." ) :
        i18nc( "@tooltip the document is not modified", "Not modified." ) );

}

void ModifiedBarController::onRemoteSyncStateChanged( RemoteSyncState remoteSyncState )
{
    const char* const iconName =
        ( mSynchronizer == 0 ) ?                   "document-new" :
        ( remoteSyncState == RemoteHasChanges ) ?  "document-save" :
        ( remoteSyncState == RemoteDeleted ) ?     "edit-delete" :
        ( remoteSyncState == RemoteUnknown ) ?     "flag-yellow" :
        ( remoteSyncState == RemoteUnreachable ) ? "network-disconnect" :
        /* else */                                 0;

    // TODO: depend an statusbar height
    const QPixmap pixmap = iconName ?
        KIcon( QLatin1String(iconName) ).pixmap(modifiedPixmapWidth) :
        QPixmap();
    mRemoteStateLabel->setPixmap( pixmap );

    // TODO: tooltips
}

void ModifiedBarController::onSynchronizerChanged( AbstractModelSynchronizer* newSynchronizer )
{
    if( mSynchronizer ) mSynchronizer->disconnect( this );

    AbstractModelSynchronizer* oldSynchronizer = mSynchronizer;
    mSynchronizer = newSynchronizer;

    LocalSyncState localState;
    RemoteSyncState remoteState;
    if( mSynchronizer )
    {
        if( ! oldSynchronizer )
            mDocument->disconnect( this, SLOT(onContentFlagsChanged(Kasten2::ContentFlags)) );

        localState = mSynchronizer->localSyncState();
        remoteState = mSynchronizer->remoteSyncState();

        connect( mSynchronizer, SIGNAL(localSyncStateChanged(Kasten2::LocalSyncState)),
                 SLOT(onLocalSyncStateChanged(Kasten2::LocalSyncState)) );
        connect( mSynchronizer, SIGNAL(remoteSyncStateChanged(Kasten2::RemoteSyncState)),
                 SLOT(onRemoteSyncStateChanged(Kasten2::RemoteSyncState)) );
        connect( mSynchronizer, SIGNAL(destroyed(QObject*)),
                                SLOT(onSynchronizerDeleted(QObject*)) );
    }
    else if( mDocument )
    {
        const bool hasChanges = (mDocument->contentFlags() & ContentHasUnstoredChanges);
        localState = ( hasChanges ? LocalHasChanges : LocalInSync );
        // TODO: onRemoteSyncStateChanged(...) checks for mSynchronizer and ignores this
        remoteState = RemoteInSync;

        connect( mDocument, SIGNAL(contentFlagsChanged(Kasten2::ContentFlags)),
                 SLOT(onContentFlagsChanged(Kasten2::ContentFlags)) );
    }
    else
    {
        localState = LocalInSync;
        // TODO: onRemoteSyncStateChanged(...) checks for mSynchronizer and ignores this
        remoteState = RemoteInSync;
    }

    onLocalSyncStateChanged( localState );
    onRemoteSyncStateChanged( remoteState );
}

void ModifiedBarController::onSynchronizerDeleted( QObject* synchronizer )
{
    if( synchronizer != mSynchronizer )
        return;

    mSynchronizer = 0;

    // switch to document state
    connect( mDocument, SIGNAL(contentFlagsChanged(Kasten2::ContentFlags)),
             SLOT(onContentFlagsChanged(Kasten2::ContentFlags)) );

    onContentFlagsChanged( mDocument->contentFlags() );
        // TODO: onRemoteSyncStateChanged(...) checks for mSynchronizer and ignores the parameter
    onRemoteSyncStateChanged( RemoteInSync );
}

}
