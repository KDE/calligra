/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2006-2008,2011 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "loadercontroller.h"

// Kasten core
#include <abstractdocumentstrategy.h>
// KDE
#include <KUrl>
#include <KFileDialog>
#include <KRecentFilesAction>
#include <KActionCollection>
#include <KStandardAction>
#include <KXMLGUIClient>
#include <KConfigGroup>
#include <KGlobal>


namespace Kasten2
{

static const char CreatorConfigGroupId[] = "Recent Files";


LoaderController::LoaderController( AbstractDocumentStrategy* documentStrategy,
                                    KXMLGUIClient* guiClient )
  : AbstractXmlGuiController()
  , mDocumentStrategy( documentStrategy )
{
    KActionCollection* const actionCollection = guiClient->actionCollection();

    KStandardAction::open( this, SLOT(load()), actionCollection );
    mOpenRecentAction =
        KStandardAction::openRecent( this, SLOT(loadRecent(KUrl)), actionCollection );

    KConfigGroup configGroup( KGlobal::config(), CreatorConfigGroupId );
    mOpenRecentAction->loadEntries( configGroup );

    connect( mDocumentStrategy, SIGNAL(urlUsed(KUrl)), SLOT(onUrlUsed(KUrl)) );
}


void LoaderController::setTargetModel( AbstractModel* model )
{
Q_UNUSED( model )
}

/// Creates a filter string as used by KFileDialog from @a _mimetypes
/// Does a workaround for "application/octet-stream" because the mimetype system
/// does not have a real entry for it ATM. It is replaced with "all/allfiles" in
/// the created string which is instead used by Filedialog as fake mimetype for
/// a type which is base type of all files.
/// See also DocumentSyncManager.
static QString mimetypeFilterString( const QStringList& _mimetypes )
{
    QStringList mimetypes = _mimetypes;

    const int index = mimetypes.indexOf( QLatin1String("application/octet-stream") );
    if( index != -1 )
        mimetypes.replace( index, QLatin1String("all/allfiles") );

    return mimetypes.join( QLatin1String(" ") );
}

void LoaderController::load()
{
    const QString filterString = mimetypeFilterString( mDocumentStrategy->supportedRemoteTypes() );

    const KUrl::List urls =
        KFileDialog::getOpenUrls( KUrl()/*mWorkingUrl.url()*/, filterString, /*mWidget*/0 );

    foreach( const KUrl& url, urls )
        mDocumentStrategy->load( url );
}

void LoaderController::loadRecent( const KUrl& url )
{
    mDocumentStrategy->load( url );
}

void LoaderController::onUrlUsed( const KUrl& url )
{
    mOpenRecentAction->addUrl( url );
}

LoaderController::~LoaderController()
{
    KConfigGroup configGroup( KGlobal::config(), CreatorConfigGroupId );
    mOpenRecentAction->saveEntries( configGroup );
}

}
