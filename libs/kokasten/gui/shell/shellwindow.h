/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2007-2008,2011 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef SHELLWINDOW_H
#define SHELLWINDOW_H

// Kasten gui
#include "kastengui_export.h"
#include "widgetsdockable.h"
// KDE
#include <KXmlGuiWindow>

template<class T> class QList;


namespace Kasten2
{
class ShellWindowPrivate;

class ViewManager;
class MultiViewAreas;
class AbstractXmlGuiController;
class AbstractToolView;
class AbstractView;
class AbstractDocument;


class KASTENGUI_EXPORT ShellWindow : public KXmlGuiWindow,
                                     public If::WidgetsDockable
{
   Q_OBJECT
   Q_INTERFACES( Kasten2::If::WidgetsDockable )

  public:
    explicit ShellWindow( ViewManager* viewManager );
    virtual ~ShellWindow();

  public Q_SLOTS:
    void showDocument( Kasten2::AbstractDocument* document ); // TODO: better name

  public:
    void updateControllers( AbstractView* view );
    void addXmlGuiController( AbstractXmlGuiController* controller );
    void addTool( AbstractToolView* toolView );

  public: // If::WidgetsDockable API
    virtual QList<ToolViewDockWidget*> dockWidgets() const;

  protected:
    MultiViewAreas* viewArea() const;
    ViewManager* viewManager() const;

  private:
    Q_PRIVATE_SLOT( d_func(), void onTitleChanged( const QString& newTitle ) )
    Q_PRIVATE_SLOT( d_func(), void onContentFlagsChanged( Kasten2::ContentFlags contentFlags ) )
    Q_PRIVATE_SLOT( d_func(), void onLocalSyncStateChanged( Kasten2::LocalSyncState newState ) )
    Q_PRIVATE_SLOT( d_func(), void onViewFocusChanged( Kasten2::AbstractView* view ) )
    Q_PRIVATE_SLOT( d_func(), void onToolVisibilityChanged( bool isVisible ) )
    Q_PRIVATE_SLOT( d_func(), void onSynchronizerDeleted( QObject* synchronizer ) )

  protected:
    ShellWindowPrivate* const d_ptr;
    Q_DECLARE_PRIVATE( ShellWindow )
};

}

#endif
