/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2007-2009,2011 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "shellwindow.h"
#include "shellwindow_p.h"
#include "shellwindow.moc"


namespace Kasten2
{

ShellWindow::ShellWindow( ViewManager* viewManager )
  : KXmlGuiWindow(),
    d_ptr( new ShellWindowPrivate(this, viewManager) )
{
}

ViewManager* ShellWindow::viewManager() const
{
    Q_D( const ShellWindow );

    return d->viewManager();
}

MultiViewAreas* ShellWindow::viewArea() const
{
    Q_D( const ShellWindow );

    return d->viewArea();
}

QList<ToolViewDockWidget*> ShellWindow::dockWidgets() const
{
    Q_D( const ShellWindow );

    return d->dockWidgets();
}

void ShellWindow::addXmlGuiController( AbstractXmlGuiController* controller )
{
    Q_D( ShellWindow );

    d->addXmlGuiController( controller );
}

void ShellWindow::addTool( AbstractToolView* toolView )
{
    Q_D( ShellWindow );

    d->addTool( toolView );
}

void ShellWindow::showDocument( AbstractDocument* document )
{
    Q_D( ShellWindow );

    d->showDocument( document );
}

void ShellWindow::updateControllers( AbstractView* view )
{
    Q_D( ShellWindow );

    d->updateControllers( view );
}

ShellWindow::~ShellWindow()
{
    delete d_ptr;
}

}
