/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2006,2008-2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

// lib
#include "abstractview.h"
// Qt
#include <QtCore/QList>
#include <QtCore/QObject>


namespace Kasten2
{

class ModelCodecViewManager;
class AbstractViewFactory;


class KASTENGUI_EXPORT ViewManager : public QObject
{
  Q_OBJECT

  public:
    ViewManager();

    virtual ~ViewManager();

  public:
    void setViewFactory( AbstractViewFactory* factory );

    void createCopyOfView( AbstractView* view, Qt::Alignment alignment = 0 );
    void removeViews( const QList<AbstractView*>& views );

  public:
    QList<AbstractView*> views() const;
    AbstractView *viewByWidget( QWidget* widget ) const;

  public:
    ModelCodecViewManager* codecViewManager();

  public Q_SLOTS:
    void createViewsFor( const QList<Kasten2::AbstractDocument*>& documents );
    void removeViewsFor( const QList<Kasten2::AbstractDocument*>& documents );

  Q_SIGNALS:
    // view was created and already added to the list
    void opened( const QList<Kasten2::AbstractView*>& views );
    // view will be closed, already removed from list
    void closing( const QList<Kasten2::AbstractView*>& views );

  private:
    QList<AbstractView*> mViewList;
    AbstractViewFactory* mFactory;

    // TODO: remove into own singleton
    ModelCodecViewManager* mCodecViewManager;
};


inline ModelCodecViewManager* ViewManager::codecViewManager() { return mCodecViewManager; }

}

#endif
