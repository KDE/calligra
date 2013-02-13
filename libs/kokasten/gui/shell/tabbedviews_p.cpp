/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2007,2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "tabbedviews_p.h"

// lib
#include "toolinlineviewwidget.h"
#include <abstracttoolinlineview.h>
#include <abstractdocument.h>
// Qt
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>


namespace Kasten2
{

TabbedViewsPrivate::TabbedViewsPrivate( TabbedViews* parent )
  : AbstractGroupedViewsPrivate( parent ),
    mCurrentView( 0 )
{
}

void TabbedViewsPrivate::init()
{
    Q_Q( TabbedViews );

    mTabWidget = new KTabWidget();
    mTabWidget->setTabsClosable( true );
    mTabWidget->setDocumentMode( true );
    mViewAreaBox = new ViewAreaBox( mTabWidget );

    q->connect( mTabWidget, SIGNAL(closeRequest(QWidget*)), SLOT(onCloseRequest(QWidget*)) );
    q->connect( mTabWidget, SIGNAL(mouseMiddleClick(QWidget*)), SLOT(onCloseRequest(QWidget*)) );
    q->connect( mTabWidget, SIGNAL(mouseMiddleClick()), SLOT(onMouseMiddleClick()) );
    q->connect( mTabWidget, SIGNAL(currentChanged(int)), SLOT(onCurrentChanged(int)) );
    q->connect( mTabWidget, SIGNAL(testCanDecode(const QDragMoveEvent*,bool&)),
                SLOT(onDragMoveEvent(const QDragMoveEvent*,bool&)) );
    q->connect( mTabWidget, SIGNAL(receivedDropEvent(QDropEvent*)),
                SLOT(onDropEvent(QDropEvent*)) );
}

QList<AbstractView*> TabbedViewsPrivate::viewList() const
{
    QList<AbstractView*> result;

    const int count = mTabWidget->count();
    for( int i=0; i<count; ++i )
    {
        const ViewBox* viewBox = static_cast<const ViewBox*>( mTabWidget->widget(i) );
        AbstractView* view = viewBox->view();
        result.append( view );
    }

    return result;
}

int TabbedViewsPrivate::indexOf( AbstractView* view ) const
{
    int result = -1;

    const int tabCount = mTabWidget->count();
    for( int i=0; i<tabCount; ++i )
    {
        const ViewBox* viewBox = static_cast<const ViewBox*>( mTabWidget->widget(i) );
        if( view == viewBox->view() )
        {
            result = i;
            break;
        }
    }

    return result;
}


void TabbedViewsPrivate::addViews( const QList<AbstractView*>& views )
{
    Q_Q( TabbedViews );

    if( views.count() == 0 )
        return;

    int insertIndex = mTabWidget->currentIndex() + 1;
    foreach( AbstractView* view, views )
    {
        q->connect( view, SIGNAL(titleChanged(QString)), SLOT(onTitleChanged(QString)) );

        ViewBox* viewBox = new ViewBox( view, mTabWidget );
        mTabWidget->insertTab( insertIndex, viewBox, view->title() );
        ++insertIndex;
    }

    mTabWidget->setCurrentIndex( insertIndex - 1 );

    // fix for Qt bug:
    if( mTabWidget->count() == 1 )
        // simulate signal reaction
        onCurrentChanged( 0 );

    emit q->added( views );
}

void TabbedViewsPrivate::removeViews( const QList<AbstractView*>& views )
{
    Q_Q( TabbedViews );

    int index = -1;

    // TODO: check if contained
    foreach( AbstractView* view, views )
    {
        view->disconnect( q );

        index = indexOf( view );
        if( index != -1 )
        {
            ViewBox* viewBox = static_cast<ViewBox*>( mTabWidget->widget(index) );

            mTabWidget->removeTab( index );
            delete viewBox;
        }
    }

    // fix for Qt bug:
    const int currentIndex = mTabWidget->currentIndex();
    // last removed or no change in index (not bound to widget)?
    if( currentIndex == -1 || currentIndex == index )
        // simulate signal reaction
        onCurrentChanged( currentIndex );

    emit q->removing( views );
}

void TabbedViewsPrivate::setCurrentToolInlineView( AbstractToolInlineView* view )
{
    ToolInlineViewWidget* currentViewWidget =
        qobject_cast<ToolInlineViewWidget*>( mViewAreaBox->bottomWidget() );
    AbstractToolInlineView* currentToolInlineView =
        currentViewWidget ? currentViewWidget->view() : 0;

    if( currentToolInlineView != view )
    {
        ToolInlineViewWidget* toolInlineViewWidget =
            view ? new ToolInlineViewWidget( view/*->widget()*/ ) : 0;
        mViewAreaBox->setBottomWidget( toolInlineViewWidget );
    }

    if( view )
        view->widget()->setFocus();
//     else
//         if( mCurrentView )
//             mCurrentView->setFocus();
}


void TabbedViewsPrivate::onCurrentChanged( int index )
{
    Q_Q( TabbedViews );

    const ViewBox* viewBox = static_cast<const ViewBox*>( mTabWidget->widget(index) );
    AbstractView* view = viewBox ? viewBox->view() : 0;

    if( view == mCurrentView )
        return;

    mViewAreaBox->setBottomWidget( 0 );

    if( mCurrentView )
        mCurrentView->disconnect( q );

    mCurrentView = view;

    if( view )
    {
        q->connect( view, SIGNAL(focusChanged(bool)), SLOT(onViewFocusChanged(bool)) );
        view->widget()->setFocus();
    }

    emit q->viewFocusChanged( view );
}

void TabbedViewsPrivate::onCloseRequest( QWidget* widget )
{
    Q_Q( TabbedViews );

    const ViewBox* viewBox = static_cast<const ViewBox*>( widget );
    AbstractView* view = viewBox->view();

    QList<Kasten2::AbstractView*> views;
    views.append( view );

    emit q->closeRequest( views );
}

void TabbedViewsPrivate::onTitleChanged( const QString& newTitle )
{
    Q_Q( TabbedViews );

    AbstractView* view = qobject_cast<AbstractView *>( q->sender() );
    if( view )
    {
        const int index = indexOf( view );
        if( index != -1 )
            mTabWidget->setTabText( index, newTitle );
    }
}


#if 0
void TabbedViewsPrivate::onModifiedChanged( AbstractDocument::SyncStates newStates )
{
Q_UNUSED( newStates )
    AbstractView* view = qobject_cast<AbstractView *>( sender() );
    if( view )
    {
        const int index = indexOf( view );
        if( index != -1 )
        {
//             mViewsTab->setIcon( index, newTitle ); //modificationSymbol
            if( index == mViewsTab->currentIndex() )
                setCaption( view->title(), view->document()->hasLocalChanges() );
        }
    }

}
#endif

// TODO: could be just a signal forwarder
void TabbedViewsPrivate::onViewFocusChanged( bool hasFocus )
{
    Q_Q( TabbedViews );

//     AbstractView* view = qobject_cast<AbstractView *>( q->sender() );
// kDebug()<<view<<view->title()<<hasFocus;

    emit q->focusChanged( hasFocus );
}

void TabbedViewsPrivate::onMouseMiddleClick()
{
    Q_Q( TabbedViews );

    const QMimeData* mimeData = QApplication::clipboard()->mimeData( QClipboard::Selection );

    emit q->dataDropped( mimeData );
}

void TabbedViewsPrivate::onDragMoveEvent( const QDragMoveEvent* event, bool& accepted )
{
    Q_Q( TabbedViews );

    const QMimeData* mimeData = event->mimeData();

    emit q->dataOffered( mimeData, accepted );
}

void TabbedViewsPrivate::onDropEvent( QDropEvent* event )
{
    Q_Q( TabbedViews );

    const QMimeData* mimeData = event->mimeData();

    emit q->dataDropped( mimeData );
}

TabbedViewsPrivate::~TabbedViewsPrivate()
{
    delete mViewAreaBox;
}

}
