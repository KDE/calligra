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

#ifndef ABSTRACTGROUPEDVIEWS_H
#define ABSTRACTGROUPEDVIEWS_H

// lib
#include "abstractviewarea.h"


namespace Kasten2
{

class AbstractView;
class AbstractGroupedViewsPrivate;


class KASTENGUI_EXPORT AbstractGroupedViews : public AbstractViewArea
{
  Q_OBJECT

  protected:
    AbstractGroupedViews();
    explicit AbstractGroupedViews( AbstractGroupedViewsPrivate* d );

  public:
    virtual ~AbstractGroupedViews();

  public Q_SLOTS: // set/action API to be implemented
    virtual void addViews( const QList<Kasten2::AbstractView*>& views ) = 0;
    virtual void removeViews( const QList<Kasten2::AbstractView*>& views ) = 0;
    virtual void setViewFocus( AbstractView* view ) = 0;

  public: // get API to be implemented
    // returns the list in the order of display
    virtual QList<AbstractView*> viewList() const = 0;
    virtual int viewCount() const = 0;
    virtual AbstractView* viewFocus() const = 0;

  Q_SIGNALS:
    // view was created and already added to the list
    void added( const QList<Kasten2::AbstractView*>& views );
    // view will be removed, already removed from list
    void removing( const QList<Kasten2::AbstractView*>& views );
    // closing the view is requested
    void closeRequest( const QList<Kasten2::AbstractView*>& views );
    void viewFocusChanged( Kasten2::AbstractView* view );

  protected:
    Q_DECLARE_PRIVATE( AbstractGroupedViews )
};

}

#endif
