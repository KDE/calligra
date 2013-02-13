/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2007-2009,2011 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "documentsyncmanager.h"

// lib
#include "abstractoverwritedialog.h"
#include "abstractsavediscarddialog.h"
#include "jobmanager.h"
#include "documentmanager.h"
#include <abstractloadjob.h>
#include <abstractconnectjob.h>
#include <abstractsynctoremotejob.h>
#include <abstractsyncwithremotejob.h>
#include <abstractsyncfromremotejob.h>
#include <abstractmodelsynchronizerfactory.h>
#include <abstractdocument.h>
// KDE
#include <KIO/NetAccess>
#include <KFileDialog>
#include <KLocale>
// Qt
#include <QtGui/QApplication>


namespace Kasten2
{


DocumentSyncManager::DocumentSyncManager( DocumentManager* manager )
  : mManager( manager ),
    mSynchronizerFactory( 0 ),
    mSaveDiscardDialog( 0 ),
    mOverwriteDialog( 0 )
{}

void DocumentSyncManager::setSaveDiscardDialog( AbstractSaveDiscardDialog* saveDiscardDialog )
{
    mSaveDiscardDialog = saveDiscardDialog;
}

void DocumentSyncManager::setOverwriteDialog( AbstractOverwriteDialog* overwriteDialog )
{
    mOverwriteDialog = overwriteDialog;
}

// TODO: make a difference between stream/storage formats and work/live formats?
QStringList DocumentSyncManager::supportedRemoteTypes() const
{
    return QStringList( mSynchronizerFactory->supportedRemoteType() );
}

bool DocumentSyncManager::hasSynchronizerForLocal( const QString& workDocumentType ) const
{
    // TODO: need synchronizerfactory classes to query for this or a local datastructure
    return ( mSynchronizerFactory->supportedWorkType() == workDocumentType );
}

KUrl DocumentSyncManager::urlOf( AbstractDocument* document ) const
{
    AbstractModelSynchronizer* synchronizer = document->synchronizer();

    return synchronizer ? synchronizer->url() : KUrl();
}

void DocumentSyncManager::setDocumentSynchronizerFactory( AbstractModelSynchronizerFactory* synchronizerFactory )
{
    mSynchronizerFactory = synchronizerFactory;
}

void DocumentSyncManager::load( const KUrl& url )
{
    foreach( AbstractDocument* document, mManager->documents() )
    {
        if( url == urlOf(document) )
        {
            // TODO: query if file should be reloaded/synched from disk
            emit mManager->focusRequested( document );
            return;
        }
    }

    AbstractModelSynchronizer* synchronizer = mSynchronizerFactory->createSynchronizer();
    AbstractLoadJob* loadJob = synchronizer->startLoad( url );
    connect( loadJob, SIGNAL(documentLoaded(Kasten2::AbstractDocument*)),
             SLOT(onDocumentLoaded(Kasten2::AbstractDocument*)) );

    JobManager::executeJob( loadJob ); // TODO: pass a ui handler to jobmanager

    // store path
//     mWorkingUrl = url.upUrl();
    emit urlUsed( url );
}

/// Creates a filter string as used by KFileDialog from @a _mimetypes
/// Does a workaround for "application/octet-stream" because the mimetype system
/// does not have a real entry for it ATM. It is replaced with "all/allfiles" in
/// the created string which is instead used by Filedialog as fake mimetype for
/// a type which is base type of all files.
/// See also e.g. LoaderController.
static QString mimetypeFilterString( const QStringList& _mimetypes )
{
    QStringList mimetypes = _mimetypes;

    const int index = mimetypes.indexOf( QLatin1String("application/octet-stream") );
    if( index != -1 )
        mimetypes.replace( index, QLatin1String("all/allfiles") );

    return mimetypes.join( QLatin1String(" ") );
}

bool DocumentSyncManager::setSynchronizer( AbstractDocument* document )
{
    bool storingDone = false;

    AbstractModelSynchronizer* currentSynchronizer = document->synchronizer();
    // TODO: warn if there were updates in the second before saveAs was activated
//     if( currentSynchronizer )
//         currentSynchronizer->pauseSynchronization(); also unpause below
    const QString processTitle =
        i18nc( "@title:window", "Save As" );
    const QString filterString = mimetypeFilterString( supportedRemoteTypes() );
    do
    {
        KUrl newUrl = KFileDialog::getSaveUrl( /*mWorkingUrl.url()*/KUrl(), filterString, /*mWidget*/0, processTitle );

        if( !newUrl.isEmpty() )
        {
            const bool isNewUrl = ( currentSynchronizer == 0 )
                                  || ( newUrl != currentSynchronizer->url() );

            if( isNewUrl )
            {
                const bool isUrlInUse =
                    KIO::NetAccess::exists( newUrl, KIO::NetAccess::DestinationSide, /*mWidget*/0 );

                if( isUrlInUse )
                {
                    // TODO: care for case that file from url is already loaded by (only?) this program
//                     const bool otherFileLoaded = mManager->documentByUrl( newUrl );
                    // TODO: replace "file" with synchronizer->storageTypeName() or such
                    // TODO: offer "Synchronize" as alternative, if supported, see below
                    // ask synchronizer for capabilities, as some can only overwrite
                    const Answer answer =
                        mOverwriteDialog ? mOverwriteDialog->queryOverwrite( newUrl, processTitle ) : Cancel;

                    if( answer == Cancel )
                        break;
                    if( answer == PreviousQuestion )
                        continue;
                }

                // switch url and synchronizer
                if( currentSynchronizer && true )//TODO: same remote mimetype
                {
                    //TODO: overwrite for now
                    AbstractSyncWithRemoteJob* syncJob = currentSynchronizer->startSyncWithRemote( newUrl,
                                                               AbstractModelSynchronizer::ReplaceRemote );
                    const bool syncSucceeded = JobManager::executeJob( syncJob );
//                     currentSynchronizer->unpauseSynchronization(); also pause above
                    storingDone = syncSucceeded;
                }
                else
                {
                    //TODO: is overwrite for now, is this useful?
                    AbstractModelSynchronizer* synchronizer = mSynchronizerFactory->createSynchronizer();
                    AbstractConnectJob* connectJob = synchronizer->startConnect( document, newUrl,
                                                               AbstractModelSynchronizer::ReplaceRemote );
                    const bool connectSucceeded = JobManager::executeJob( connectJob );

                    storingDone = connectSucceeded;
                }

                if( storingDone )
                    emit urlUsed( newUrl );
#if 0
//     mWorkingUrl = Url.upUrl();
    OpenRecentAction->addUrl( Url );
#endif
            }
            // same url
            else
            {
                // TODO: what to do? synchTo? synchWith? synchFrom? Or does the synchronizer care for this?
                // By e.g. warning that we might be overwriting something?
                // synchTo might be the intention, after all the user wanted a new storage
                // 
                AbstractSyncToRemoteJob* syncJob = document->synchronizer()->startSyncToRemote();
                const bool syncFailed = JobManager::executeJob( syncJob );

                storingDone = !syncFailed;
            }
        }
        else
            break;
   }
   while( !storingDone );

   return storingDone;
}

bool DocumentSyncManager::canClose( AbstractDocument* document )
{
    bool canClose = true;

    if( document->contentFlags() & ContentHasUnstoredChanges )
    {
        AbstractModelSynchronizer* synchronizer = document->synchronizer();
        const bool couldSynchronize = hasSynchronizerForLocal( document->mimeType() );

        const QString processTitle = i18nc( "@title:window", "Close" );

        if( (synchronizer && synchronizer->localSyncState() == LocalHasChanges) ||
            couldSynchronize )
        {
            const Answer answer =
                mSaveDiscardDialog ? mSaveDiscardDialog->querySaveDiscard( document, processTitle ) : Cancel;

            if( answer == Save )
            {
                if( synchronizer )
                {
                    AbstractSyncToRemoteJob* syncJob = synchronizer->startSyncToRemote();
                    const bool isSynced = JobManager::executeJob( syncJob );

                    canClose = isSynced;
                }
                else
                    canClose = setSynchronizer( document );
            }
            else
                canClose = ( answer == Discard );
        }
        else
        {
            const Answer answer =
                mSaveDiscardDialog ? mSaveDiscardDialog->queryDiscard( document, processTitle ) : Cancel;

            canClose = ( answer == Discard );
        }
    }

    return canClose;
}

void DocumentSyncManager::reload( AbstractDocument* document )
{
    AbstractModelSynchronizer* synchronizer = document->synchronizer();

    if( synchronizer->localSyncState() == LocalHasChanges )
    {
        const QString processTitle = i18nc( "@title:window", "Reload" );

        const Answer answer =
            mSaveDiscardDialog ? mSaveDiscardDialog->queryDiscardOnReload( document, processTitle ) : Cancel;

        if( answer == Cancel )
            return;
    }

    AbstractSyncFromRemoteJob* syncJob = synchronizer->startSyncFromRemote();
    JobManager::executeJob( syncJob );
}

void DocumentSyncManager::save( AbstractDocument* document )
{
    AbstractModelSynchronizer* synchronizer = document->synchronizer();
    AbstractSyncToRemoteJob* syncJob = synchronizer->startSyncToRemote();
    JobManager::executeJob( syncJob );
}

void DocumentSyncManager::onDocumentLoaded( AbstractDocument* document )
{
    if( document )
        mManager->addDocument( document );
}

void DocumentSyncManager::onDocumentsAdded( const QList<Kasten2::AbstractDocument*>& documents )
{
    Q_UNUSED( documents )
}

void DocumentSyncManager::onDocumentsClosing( const QList<Kasten2::AbstractDocument*>& documents )
{
    Q_UNUSED( documents )
}


DocumentSyncManager::~DocumentSyncManager()
{
    delete mSynchronizerFactory;
}

}
