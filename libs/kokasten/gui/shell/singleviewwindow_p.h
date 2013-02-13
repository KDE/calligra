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

#ifndef SINGLEVIEWWINDOW_P_H
#define SINGLEVIEWWINDOW_P_H

// lib
#include "singleviewwindow.h"
// Kasten core
#include <kastencore.h>

class QDragMoveEvent;
class QDropEvent;


namespace Kasten2
{

class SingleViewArea;
class AbstractTool;
class AbstractModelSynchronizer;
class AbstractDocument;


class SingleViewWindowPrivate
{
  public:
    SingleViewWindowPrivate( SingleViewWindow* parent,
                             AbstractView* view );

    ~SingleViewWindowPrivate();

  public:
    void init();

  public:
    void setView( AbstractView* view );
    void addXmlGuiController( AbstractXmlGuiController* controller );
    void addTool( AbstractToolView* toolView );

  public: // If::WidgetsDockable API
    QList<ToolViewDockWidget*> dockWidgets() const;

  protected:
    AbstractView* view() const;
    SingleViewArea* viewArea() const;

  private: // Q_SLOTS
    void onTitleChanged( const QString& newTitle );
    void onContentFlagsChanged( Kasten2::ContentFlags contentFlags );
    void onLocalSyncStateChanged( Kasten2::LocalSyncState newState );
    void onToolVisibilityChanged( bool isVisible );
    void onSynchronizerDeleted( QObject* synchronizer );

  protected:
    Q_DECLARE_PUBLIC( SingleViewWindow )

  protected:
    SingleViewWindow* const q_ptr;

    AbstractView* mView;
    AbstractDocument* mDocument;
    AbstractModelSynchronizer* mSynchronizer;

    SingleViewArea* const mViewArea;
    QList<AbstractXmlGuiController*> mControllers;
    QList<ToolViewDockWidget*> mDockWidgets;
    QList<AbstractTool*> mTools;
};


inline QList<ToolViewDockWidget*> SingleViewWindowPrivate::dockWidgets() const { return mDockWidgets; }
inline AbstractView* SingleViewWindowPrivate::view() const { return mView; }
inline SingleViewArea* SingleViewWindowPrivate::viewArea() const { return mViewArea; }

}

#endif
