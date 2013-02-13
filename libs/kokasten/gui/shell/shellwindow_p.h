/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2007-2008,2011-2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef SHELLWINDOW_P_H
#define SHELLWINDOW_P_H

// lib
#include "shellwindow.h"
// Okteta core
#include <kastencore.h>
// Qt
#include <QtCore/QList>


namespace Kasten2
{
class AbstractModelSynchronizer;
class AbstractDocument;
class AbstractTool;


class ShellWindowPrivate
{
  public:
    ShellWindowPrivate( ShellWindow* parent,
                        ViewManager* viewManager );
    ~ShellWindowPrivate();

  public:
    MultiViewAreas* viewArea() const;
    ViewManager* viewManager() const;

  public:
    void updateControllers( AbstractView* view );
    void addXmlGuiController( AbstractXmlGuiController* controller );
    void addTool( AbstractToolView* toolView );
    void showDocument( AbstractDocument* document );

  public: // If::WidgetsDockable API
    QList<ToolViewDockWidget*> dockWidgets() const;

  private: // Q_SLOTS
    void onTitleChanged( const QString& newTitle );
    void onContentFlagsChanged( Kasten2::ContentFlags contentFlags );
    void onLocalSyncStateChanged( Kasten2::LocalSyncState newState );
    void onViewFocusChanged( Kasten2::AbstractView* view );
    void onToolVisibilityChanged( bool isVisible );
    void onSynchronizerDeleted( QObject* synchronizer );

  protected:
    Q_DECLARE_PUBLIC( ShellWindow )

  protected:
    ShellWindow* const q_ptr;

    MultiViewAreas* mGroupedViews;
    // hack:
    // used to store a pointer to the current, so we can disconnect to its signals... well, not perfect
    AbstractView* mCurrentView;
    AbstractDocument* mCurrentDocument;
    AbstractModelSynchronizer* mCurrentSynchronizer;

    ViewManager* mViewManager;
    QList<AbstractXmlGuiController*> mControllers;

    QList<ToolViewDockWidget*> mDockWidgets;
    QList<AbstractTool*> mTools;
};


inline MultiViewAreas* ShellWindowPrivate::viewArea() const { return mGroupedViews; }
inline ViewManager* ShellWindowPrivate::viewManager() const { return mViewManager; }
inline QList<ToolViewDockWidget*> ShellWindowPrivate::dockWidgets() const { return mDockWidgets; }

inline void ShellWindowPrivate::addXmlGuiController( AbstractXmlGuiController* controller )
{
    mControllers.append( controller );
}

}

#endif
