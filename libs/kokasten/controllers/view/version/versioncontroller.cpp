/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2008 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "versioncontroller.h"

// Kasten core
#include <documentversiondata.h>
#include <versionable.h>
#include <abstractmodel.h>
// KDE
#include <KXMLGUIClient>
#include <KLocale>
#include <KActionCollection>
#include <KToolBarPopupAction>
#include <KStandardShortcut>
// Qt
#include <QtGui/QMenu>


namespace Kasten2
{

static const int MaxMenuEntries = 10;

VersionController::VersionController( KXMLGUIClient* guiClient )
 : mModel( 0 )
{
    KActionCollection* actionCollection = guiClient->actionCollection();

    mSetToOlderVersionAction = new KToolBarPopupAction( KIcon( QLatin1String("edit-undo") ), i18nc("@action:inmenu","Undo"), this );
    actionCollection->addAction( QLatin1String("edit_undo"), mSetToOlderVersionAction );
    mSetToOlderVersionAction->setShortcuts( KStandardShortcut::undo() );

    connect( mSetToOlderVersionAction, SIGNAL(triggered(bool)),
             SLOT(onSetToOlderVersionTriggered()) );
    connect( mSetToOlderVersionAction->menu(), SIGNAL(aboutToShow()),
             SLOT(onOlderVersionMenuAboutToShow()) );
    connect( mSetToOlderVersionAction->menu(), SIGNAL(triggered(QAction*)),
             SLOT(onOlderVersionMenuTriggered(QAction*)) );

    mSetToNewerVersionAction = new KToolBarPopupAction( KIcon( QLatin1String("edit-redo") ), i18nc("@action:inmenu","Redo"), this );
    actionCollection->addAction( QLatin1String("edit_redo"), mSetToNewerVersionAction );
    mSetToNewerVersionAction->setShortcuts( KStandardShortcut::redo() );

    connect( mSetToNewerVersionAction, SIGNAL(triggered(bool)),
             SLOT(onSetToNewerVersionTriggered()) );
    connect( mSetToNewerVersionAction->menu(), SIGNAL(aboutToShow()),
             SLOT(onNewerVersionMenuAboutToShow()) );
    connect( mSetToNewerVersionAction->menu(), SIGNAL(triggered(QAction*)),
             SLOT(onNewerVersionMenuTriggered(QAction*)) );

    setTargetModel( 0 );
}

void VersionController::setTargetModel( AbstractModel* model )
{
    if( mModel )
    {
        mModel->disconnect( this );
        AbstractModel* versionedModel = mModel->findBaseModelWithInterface<If::Versionable*>();
        if( versionedModel ) versionedModel->disconnect( this );
    }

    mModel = model;
    AbstractModel* versionedModel = mModel ? mModel->findBaseModelWithInterface<If::Versionable*>() : 0;
    mVersionControl = versionedModel ? qobject_cast<If::Versionable*>( versionedModel ) : 0;

    if( mVersionControl )
    {
        connect( versionedModel, SIGNAL(revertedToVersionIndex(int)), SLOT(onVersionIndexChanged(int)) );
        connect( versionedModel, SIGNAL(headVersionChanged(int)),     SLOT(onVersionIndexChanged(int)) );
        connect( mModel, SIGNAL(readOnlyChanged(bool)), SLOT(onReadOnlyChanged(bool)) );
    }
    else
        mModel = 0;

    const bool isVersionable = ( mVersionControl != 0 && !mModel->isReadOnly() );

    if( isVersionable )
        onVersionIndexChanged( mVersionControl->versionIndex() );
    else
    {
        mSetToOlderVersionAction->setEnabled( false );
        mSetToNewerVersionAction->setEnabled( false );
    }
}

void VersionController::onVersionIndexChanged( int versionIndex )
{
    const bool hasOlderVersions = ( versionIndex > 0 );
    mSetToOlderVersionAction->setEnabled( hasOlderVersions );
    if( hasOlderVersions )
        mSetToOlderVersionAction->setData( versionIndex-1 );

    const bool hasNewerVersions = ( versionIndex < (mVersionControl->versionCount()-1) );
    mSetToNewerVersionAction->setEnabled( hasNewerVersions );
    if( hasNewerVersions )
        mSetToNewerVersionAction->setData( versionIndex+1 );
}

void VersionController::onSetToOlderVersionTriggered()
{
    const int versionIndex = mSetToOlderVersionAction->data().toInt();
    mVersionControl->revertToVersionByIndex( versionIndex );
}

void VersionController::onSetToNewerVersionTriggered()
{
    const int versionIndex = mSetToNewerVersionAction->data().toInt();
    mVersionControl->revertToVersionByIndex( versionIndex );
}

void VersionController::onOlderVersionMenuAboutToShow()
{
    QMenu *menu = mSetToOlderVersionAction->menu();
    menu->clear();

    int menuEntries = 0;
    for( int versionIndex = mVersionControl->versionIndex();
         versionIndex > 0 && menuEntries < MaxMenuEntries;
         --versionIndex, ++menuEntries )
    {
        DocumentVersionData versionData = mVersionControl->versionData( versionIndex );

        const QString changeComment = versionData.changeComment();

        const QString actionText = i18nc( "@action Undo: [change]", "Undo: %1", changeComment );

        QAction *action = menu->addAction( actionText );
        action->setData( versionIndex-1 );
    }
}

void VersionController::onNewerVersionMenuAboutToShow()
{
    QMenu *menu = mSetToNewerVersionAction->menu();
    menu->clear();

    int menuEntries = 0;
    for( int versionIndex = mVersionControl->versionIndex()+1;
         versionIndex < mVersionControl->versionCount() && menuEntries < MaxMenuEntries;
         ++versionIndex, ++menuEntries )
    {
        DocumentVersionData versionData = mVersionControl->versionData( versionIndex );

        const QString changeComment = versionData.changeComment();

        const QString actionText = i18nc( "@action Redo: [change]", "Redo: %1", changeComment );

        QAction *action = menu->addAction( actionText );
        action->setData( versionIndex );
    }
}

void VersionController::onOlderVersionMenuTriggered( QAction *action )
{
    const int versionIndex = action->data().toInt();
    mVersionControl->revertToVersionByIndex( versionIndex );
}

void VersionController::onNewerVersionMenuTriggered( QAction *action )
{
    const int versionIndex = action->data().toInt();
    mVersionControl->revertToVersionByIndex( versionIndex );
}

void VersionController::onReadOnlyChanged( bool isReadOnly )
{
    if( isReadOnly )
    {
        mSetToOlderVersionAction->setEnabled( false );
        mSetToNewerVersionAction->setEnabled( false );
    }
    else
        onVersionIndexChanged( mVersionControl->versionIndex() );
}

}
