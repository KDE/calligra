/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2011-2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "singleviewwindow_p.h"

// this lib
#include "singleviewarea.h"
#include <abstractxmlguicontroller.h>
#include <toolviewdockwidget.h>
#include <abstracttoolview.h>
#include <abstractview.h>
// Kasten core
#include <abstracttool.h>
#include <abstractmodelsynchronizer.h>


namespace Kasten2
{

SingleViewWindowPrivate::SingleViewWindowPrivate( SingleViewWindow* parent,
                                                  AbstractView* view )
  : q_ptr( parent )
  , mView( 0 )
  , mDocument( 0 )
  , mSynchronizer( 0 )
  , mViewArea( new SingleViewArea() )
{
    parent->setCentralWidget( mViewArea->widget() );

    setView( view );
}

void SingleViewWindowPrivate::setView( AbstractView* view )
{
    Q_Q( SingleViewWindow );

    if( mView == view )
        return;

    if( mView ) mView->disconnect( q );

    // TODO: what to do with the old one if existing?
    mView = view;
    mViewArea->setView( view );

    foreach( AbstractXmlGuiController* controller, mControllers )
        controller->setTargetModel( view );

    foreach( ToolViewDockWidget* dockWidget, mDockWidgets )
    {
        if( dockWidget->isShown() )
            dockWidget->toolView()->tool()->setTargetModel( view );
    }

    AbstractDocument* oldDocument = mDocument;
    mDocument = view ? view->findBaseModel<AbstractDocument*>() : 0;
    const bool isNewDocument = (mDocument != oldDocument);

    AbstractModelSynchronizer* oldSynchronizer = mSynchronizer;
    mSynchronizer = mDocument ? mDocument->synchronizer() : 0;
    const bool isNewSynchronizer = (mSynchronizer != oldSynchronizer);

    if( oldSynchronizer )
    {
        if( isNewSynchronizer ) oldSynchronizer->disconnect( q );
    }
    else
    {
        if( oldDocument && isNewDocument ) oldDocument->disconnect( q );
    }

    const QString title = view ? view->title() : QString();
    const bool hasChanges =
        mSynchronizer ? (mSynchronizer->localSyncState() == LocalHasChanges) :
        mDocument ?     mDocument->contentFlags().testFlag(ContentHasUnstoredChanges) :
                        false;
    q->setCaption( title, hasChanges );

    if( view )
        q->connect( view, SIGNAL(titleChanged(QString)), SLOT(onTitleChanged(QString)) );

    if( mSynchronizer )
    {
        if( isNewSynchronizer )
        {
            q->connect( mSynchronizer, SIGNAL(localSyncStateChanged(Kasten2::LocalSyncState)),
                        SLOT(onLocalSyncStateChanged(Kasten2::LocalSyncState)) );
            q->connect( mSynchronizer, SIGNAL(destroyed(QObject*)),
                        SLOT(onSynchronizerDeleted(QObject*)) );
        }
    }
    else if( mDocument )
    {
        if( isNewDocument )
            q->connect( mDocument, SIGNAL(contentFlagsChanged(Kasten2::ContentFlags)),
                        SLOT(onContentFlagsChanged(Kasten2::ContentFlags)) );
    }
}

void SingleViewWindowPrivate::addXmlGuiController( AbstractXmlGuiController* controller )
{
    mControllers.append( controller );
}

void SingleViewWindowPrivate::addTool( AbstractToolView* toolView )
{
    Q_Q( SingleViewWindow );

    ToolViewDockWidget* dockWidget = new ToolViewDockWidget( toolView, q );
    // TODO: where to set the initial area?
    q->addDockWidget( Qt::RightDockWidgetArea, dockWidget );

    mTools.append( toolView->tool() );
    mDockWidgets.append( dockWidget );

    if( dockWidget->isVisible() && mView )
        toolView->tool()->setTargetModel( mView );

    q->connect( dockWidget, SIGNAL(visibilityChanged(bool)),
                SLOT(onToolVisibilityChanged(bool)) );
}

void SingleViewWindowPrivate::onTitleChanged( const QString& newTitle )
{
    Q_Q( SingleViewWindow );

    if( mView )
    {
        AbstractDocument* document = mView->findBaseModel<AbstractDocument*>();
        AbstractModelSynchronizer* synchronizer = document->synchronizer();
        const bool hasChanges =
            synchronizer ? (synchronizer->localSyncState() == LocalHasChanges) :
            document ?     document->contentFlags().testFlag(ContentHasUnstoredChanges) :
                           false;
        q->setCaption( newTitle, hasChanges );
    }
}

void SingleViewWindowPrivate::onContentFlagsChanged( ContentFlags contentFlags )
{
    Q_Q( SingleViewWindow );

    if( mView )
    {
        const bool hasChanges = (contentFlags & ContentHasUnstoredChanges);
        q->setCaption( mView->title(), hasChanges );
    }
}

void SingleViewWindowPrivate::onLocalSyncStateChanged( LocalSyncState newState )
{
    Q_Q( SingleViewWindow );

    if( mView )
    {
        const bool hasChanges = (newState == LocalHasChanges);
        q->setCaption( mView->title(), hasChanges );
    }
}

void SingleViewWindowPrivate::onSynchronizerDeleted( QObject* synchronizer )
{
    Q_Q( SingleViewWindow );

    if( synchronizer != mSynchronizer )
        return;

    mSynchronizer = 0;

    // switch to document state
    q->connect( mDocument, SIGNAL(contentFlagsChanged(Kasten2::ContentFlags)),
                SLOT(onContentFlagsChanged(Kasten2::ContentFlags)) );

    onContentFlagsChanged( mDocument->contentFlags() );
}


void SingleViewWindowPrivate::onToolVisibilityChanged( bool isVisible )
{
    Q_Q( SingleViewWindow );

    ToolViewDockWidget* dockWidget = qobject_cast<ToolViewDockWidget *>( q->sender() );
    if( dockWidget )
    {
        AbstractView* view = isVisible ? mView : 0;
        dockWidget->toolView()->tool()->setTargetModel( view );
    }
}

SingleViewWindowPrivate::~SingleViewWindowPrivate()
{
    // we have to explicitly reset any inline tool view before first deleting all tools
    // and then the view area, because on destruction of the inline tool view it
    // operates on the tool, which then has been no longer ->crash
    // The other option would be to first delete the view, but for reasons if do not
    // remember currently I prefer the destruction in this order
    // TODO: make this call unneeded
    mViewArea->setCurrentToolInlineView( 0 );

    qDeleteAll( mControllers );
    qDeleteAll( mDockWidgets );
    qDeleteAll( mTools );

    delete mViewArea;
}

}
