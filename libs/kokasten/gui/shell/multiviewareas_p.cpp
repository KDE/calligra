/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "multiviewareas_p.h"

// lib
#include "viewbox.h"
#include "toolinlineviewwidget.h"
#include <abstracttoolinlineview.h>
#include <abstractdocument.h>
#include <abstractview.h>
// KDE
#include <KGlobalSettings>

#include <KDebug>

namespace Kasten2
{
// TODO: catch area focues change!
MultiViewAreasPrivate::MultiViewAreasPrivate( MultiViewAreas* parent )
  : AbstractGroupedViewsPrivate( parent ),
    mCurrentInlineToolViewArea( 0 )
{
}

void MultiViewAreasPrivate::init()
{
    Q_Q( MultiViewAreas );

    mMainSplitter = new QSplitter();

    // create start view area
    TabbedViews* viewArea = new TabbedViews();
    q->connect( viewArea, SIGNAL(focusChanged(bool)),
                SLOT(onViewAreaFocusChanged(bool)) );
    q->connect( viewArea, SIGNAL(viewFocusChanged(Kasten2::AbstractView*)),
                SIGNAL(viewFocusChanged(Kasten2::AbstractView*)) );
    q->connect( viewArea, SIGNAL(closeRequest(QList<Kasten2::AbstractView*>)),
                SIGNAL(closeRequest(QList<Kasten2::AbstractView*>)) );
    q->connect( viewArea, SIGNAL(removing(QList<Kasten2::AbstractView*>)),
                SLOT(onViewsRemoved()) );
    q->connect( viewArea, SIGNAL(dataOffered(const QMimeData*,bool&)),
                SIGNAL(dataOffered(const QMimeData*,bool&)) );
    q->connect( viewArea, SIGNAL(dataDropped(const QMimeData*)),
                SIGNAL(dataDropped(const QMimeData*)) );

    mViewAreaList.append( viewArea );
    mCurrentViewArea = viewArea;

    mMainSplitter->setOpaqueResize( KGlobalSettings::opaqueResize() );
    mMainSplitter->addWidget( viewArea->widget() );
}


AbstractViewArea* MultiViewAreasPrivate::splitViewArea( AbstractViewArea* _viewArea, Qt::Orientation orientation )
{
    Q_Q( MultiViewAreas );

    TabbedViews* firstViewArea = static_cast<TabbedViews*>( _viewArea );
    QWidget* firstViewAreaWidget = firstViewArea->widget();
    QSplitter* baseSplitter = static_cast<QSplitter*>( firstViewAreaWidget->parentWidget() );

    QSplitter* splitter;
    if( baseSplitter->count() == 1 ) // only valid with mMainSplitter
        splitter = baseSplitter;
    else
    {
        const QList<int> baseSplitterSizes = baseSplitter->sizes();
        const int index = baseSplitter->indexOf( firstViewAreaWidget );
        splitter = new QSplitter( baseSplitter );
        // TODO: react on opaqueResize change
        splitter->setOpaqueResize( KGlobalSettings::opaqueResize() );
        baseSplitter->insertWidget( index, splitter );
        splitter->addWidget( firstViewAreaWidget );
        baseSplitter->setSizes( baseSplitterSizes );
    }

    TabbedViews* secondViewArea = new TabbedViews();
    q->connect( secondViewArea, SIGNAL(focusChanged(bool)),
                SLOT(onViewAreaFocusChanged(bool)) );
    q->connect( secondViewArea, SIGNAL(viewFocusChanged(Kasten2::AbstractView*)),
                SIGNAL(viewFocusChanged(Kasten2::AbstractView*)) );
    q->connect( secondViewArea, SIGNAL(closeRequest(QList<Kasten2::AbstractView*>)),
                SIGNAL(closeRequest(QList<Kasten2::AbstractView*>)) );
    q->connect( secondViewArea, SIGNAL(removing(QList<Kasten2::AbstractView*>)),
                SLOT(onViewsRemoved()) );
    q->connect( secondViewArea, SIGNAL(dataOffered(const QMimeData*,bool&)),
                SIGNAL(dataOffered(const QMimeData*,bool&)) );
    q->connect( secondViewArea, SIGNAL(dataDropped(const QMimeData*)),
                SIGNAL(dataDropped(const QMimeData*)) );
    mViewAreaList.append( secondViewArea );
    mCurrentViewArea = secondViewArea;

    splitter->setOrientation( orientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal );
    splitter->addWidget( secondViewArea->widget() );
    // set to equal sizes
    QList<int> splitterSizes = splitter->sizes();
    // TODO: check if there are more, style-dependent spaces
    const int equalSize = ( splitterSizes[0] + splitterSizes[1] - splitter->handleWidth() ) / 2;
    splitterSizes[1] = splitterSizes[0] = equalSize;
    splitter->setSizes( splitterSizes );

    QList<AbstractViewArea*> viewAreas;
    viewAreas.append( secondViewArea );
    emit q->viewAreasAdded( viewAreas );
    emit q->viewAreaFocusChanged( secondViewArea );

    return secondViewArea;
}


void MultiViewAreasPrivate::onViewsRemoved()
{
     Q_Q( MultiViewAreas );

   // keep a minimum of one area
    if( mViewAreaList.count() < 2 )
        return;

    TabbedViews* viewArea = qobject_cast<TabbedViews*>( q->sender() );

    if( viewArea->viewCount() == 0 )
    {
        QWidget* viewAreaWidget = viewArea->widget();
        QSplitter* baseSplitter = static_cast<QSplitter*>( viewAreaWidget->parentWidget() );

        const int index = baseSplitter->indexOf( viewAreaWidget );
        const int otherIndex = 1 - index;

        QWidget* otherWidget = baseSplitter->widget( otherIndex );
        // do not delete the main splitter
        if( baseSplitter != mMainSplitter )
        {
            QSplitter* baseOfBaseSplitter = static_cast<QSplitter*>( baseSplitter->parentWidget() );

            const QList<int> baseOfBaseSplitterSizes = baseOfBaseSplitter->sizes();
            const int indexOfBaseSplitter = baseOfBaseSplitter->indexOf( baseSplitter );
            baseOfBaseSplitter->insertWidget( indexOfBaseSplitter, otherWidget );
            viewAreaWidget->setParent( 0 );
            delete baseSplitter;
            baseOfBaseSplitter->setSizes( baseOfBaseSplitterSizes );
        }

        mViewAreaList.removeOne( viewArea );

        if( mCurrentInlineToolViewArea == viewArea )
            mCurrentInlineToolViewArea = 0;

        if( mCurrentViewArea == viewArea )
        {
            // search for the previous widget which is the next or the previous, using index
            while( true )
            {
                QSplitter* splitter = qobject_cast<QSplitter*>( otherWidget );
                if( splitter )
                    otherWidget = splitter->widget( index );
                else
                    break;
            }

            foreach( TabbedViews* viewArea, mViewAreaList )
            {
                if( viewArea->widget() == otherWidget )
                {
                    viewArea->setFocus();
                    break;
                }
            }
        }

        QList<AbstractViewArea*> viewAreas;
        viewAreas.append( viewArea );
        emit q->viewAreasRemoved( viewAreas );

        delete viewArea;
    }
}

void MultiViewAreasPrivate::onViewAreaFocusChanged( bool hasFocus )
{
    Q_Q( MultiViewAreas );

    TabbedViews* viewArea = qobject_cast<TabbedViews *>( q->sender() );
kDebug()<<viewArea<<hasFocus;
    if( mCurrentViewArea == viewArea )
        return;

    if( mCurrentInlineToolViewArea && mCurrentInlineToolViewArea == mCurrentViewArea )
        mCurrentInlineToolViewArea->setCurrentToolInlineView( 0 );

    // TODO: care for ! hasFocus?
    if( hasFocus )
    {
        mCurrentViewArea = viewArea;

        emit q->viewAreaFocusChanged( viewArea );
        emit q->viewFocusChanged( viewArea->viewFocus() );
    }
}

#if 0
void MultiViewAreasPrivate::onModifiedChanged( AbstractDocument::SyncStates newStates )
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

MultiViewAreasPrivate::~MultiViewAreasPrivate()
{
    qDeleteAll( mViewAreaList );
    delete mMainSplitter;
}

}
