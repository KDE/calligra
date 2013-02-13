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

#ifndef MULTIVIEWAREAS_P_H
#define MULTIVIEWAREAS_P_H

// lib
#include "abstractgroupedviews_p.h"
#include "multiviewareas.h"
#include "tabbedviews.h"
// Qt
#include <QtGui/QSplitter>

class QDragMoveEvent;
class QDropEvent;


namespace Kasten2
{

class TabbedViews;


class MultiViewAreasPrivate : public AbstractGroupedViewsPrivate
{
  public:
    explicit MultiViewAreasPrivate( MultiViewAreas* parent );

    virtual ~MultiViewAreasPrivate();

  public:
    void init();

  public: // AbstractViewArea API
    void setFocus();
    QWidget* widget() const;
    bool hasFocus() const;

  public: // AbstractGroupedViews API
    void addViews( const QList<AbstractView*>& views );
    void removeViews( const QList<AbstractView*>& views );
    void setViewFocus( AbstractView* view );

    QList<AbstractView*> viewList() const;
    int viewCount() const;
    AbstractView* viewFocus() const;

  public: // If::ToolInlineViewable API
    void setCurrentToolInlineView( AbstractToolInlineView* view );

  public: // If::ViewAreaSplitable API
    AbstractViewArea* splitViewArea( AbstractViewArea* viewArea, Qt::Orientation orientation );
    void closeViewArea( AbstractViewArea* viewArea );
    void setViewAreaFocus( AbstractViewArea* viewArea );
    AbstractViewArea* viewAreaFocus() const;
    int viewAreasCount() const;

  protected:
    int indexOf( AbstractView* view ) const;

  protected:// slots
    void onViewsRemoved();
    void onViewAreaFocusChanged( bool hasFocus );

  protected:
    Q_DECLARE_PUBLIC( MultiViewAreas )

  protected:
    QList<TabbedViews*> mViewAreaList;
    QSplitter* mMainSplitter;

    TabbedViews* mCurrentViewArea;
    TabbedViews* mCurrentInlineToolViewArea;
};


inline QWidget* MultiViewAreasPrivate::widget()                 const { return mMainSplitter; }
inline bool MultiViewAreasPrivate::hasFocus()                   const { return mCurrentViewArea->hasFocus(); }
inline AbstractView* MultiViewAreasPrivate::viewFocus()         const { return mCurrentViewArea->viewFocus(); }
inline AbstractViewArea* MultiViewAreasPrivate::viewAreaFocus() const { return mCurrentViewArea; }
inline int MultiViewAreasPrivate::viewAreasCount()              const { return mViewAreaList.count(); }

inline QList<AbstractView*> MultiViewAreasPrivate::viewList() const
{
    QList<AbstractView*> result;

    foreach( TabbedViews* viewArea, mViewAreaList )
        result.append( viewArea->viewList() );

    return result;
}

inline int MultiViewAreasPrivate::viewCount() const
{
    int result = 0;

    foreach( TabbedViews* viewArea, mViewAreaList )
        result += viewArea->viewCount();

    return result;
}

inline int MultiViewAreasPrivate::indexOf( AbstractView* view ) const
{
    int result = -1;

    int globalBaseIndex = 0;
    foreach( TabbedViews* viewArea, mViewAreaList )
    {
        const int localIndexOf = viewArea->indexOf( view );
        if( localIndexOf != -1 )
        {
            result = globalBaseIndex + localIndexOf;
            break;
        }
        globalBaseIndex += viewArea->viewCount();
    }

    return result;
}

inline void MultiViewAreasPrivate::setFocus()
{
    mCurrentViewArea->setFocus();
}

inline void MultiViewAreasPrivate::addViews( const QList<AbstractView*>& views )
{
    Q_Q( MultiViewAreas );

    mCurrentViewArea->addViews( views );

    emit q->added( views );
}

inline void MultiViewAreasPrivate::removeViews( const QList<AbstractView*>& views )
{
    Q_Q( MultiViewAreas );

    // TODO: possible to just send the views of the given area?
    foreach( TabbedViews* viewArea, mViewAreaList )
        viewArea->removeViews( views );

    // TODO: above might trigger removal of areas before, is this a problem?
    emit q->removing( views );
}

inline void MultiViewAreasPrivate::setCurrentToolInlineView( AbstractToolInlineView* view )
{
    if( mCurrentInlineToolViewArea && mCurrentInlineToolViewArea != mCurrentViewArea )
        mCurrentInlineToolViewArea->setCurrentToolInlineView( 0 );

    mCurrentInlineToolViewArea = mCurrentViewArea;

    mCurrentViewArea->setCurrentToolInlineView( view );
}

inline void MultiViewAreasPrivate::setViewFocus( AbstractView *view )
{
    // TODO: makes this more efficient!
    foreach( TabbedViews* viewArea, mViewAreaList )
    {
        const int localIndex = viewArea->indexOf( view );
        if( localIndex != -1 )
        {
            viewArea->setViewFocus( view );
            break;
        }
    }
}

 //TODO: this method could be removed, as it is the same as _viewArea->setFocus(), or?
inline void MultiViewAreasPrivate::setViewAreaFocus( AbstractViewArea* _viewArea )
{
    TabbedViews* viewArea = static_cast<TabbedViews*>( _viewArea );
    if( viewArea == mCurrentViewArea )
        return;

    if( mViewAreaList.contains(viewArea) )
        viewArea->setFocus();
}

inline void MultiViewAreasPrivate::closeViewArea( AbstractViewArea* _viewArea )
{
    Q_Q( MultiViewAreas );

    TabbedViews* viewArea = static_cast<TabbedViews*>( _viewArea );

    const QList<AbstractView*> views = viewArea->viewList();

    emit q->closeRequest( views );
}

}

#endif
