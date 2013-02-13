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

#include "toollistmenucontroller.h"

// lib
#include <widgetsdockable.h>
#include <toolviewdockwidget.h>
// KDE
#include <KXMLGUIClient>
#include <KXMLGUIFactory>
// Qt
#include <QtGui/QAction>


namespace Kasten2
{

static const char ToolListActionListId[] = "tools_list";

ToolListMenuController::ToolListMenuController( If::WidgetsDockable* widgetsDockable,
                                                KXMLGUIClient* guiClient )
 : mWidgetsDockable( widgetsDockable ), mGuiClient( guiClient )
{
    // TODO: for now this is only called on start, so first create all tools/views before this controller
    updateActions();
}

void ToolListMenuController::setTargetModel( AbstractModel* model )
{
Q_UNUSED(model)
}

void ToolListMenuController::updateActions()
{
    mGuiClient->unplugActionList( QLatin1String(ToolListActionListId) );

    qDeleteAll( mToolActionList );
    mToolActionList.clear();

    const QList<ToolViewDockWidget*> dockWidgets = mWidgetsDockable->dockWidgets();

    foreach( const ToolViewDockWidget* dockWidget, dockWidgets )
    {
        QAction *action = dockWidget->toggleViewAction();
        action->setText( dockWidget->windowTitle() );
//         action->setText( mToolView->title() );
        mToolActionList.append( action );
    }
    mGuiClient->plugActionList( QLatin1String(ToolListActionListId), mToolActionList );
}

}
