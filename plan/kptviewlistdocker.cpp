/* This file is part of the KDE project
 * Copyright (C) 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "kptviewlistdocker.h"

#include "kptviewlist.h"

#include "kptview.h"
#include "kptdebug.h"

#include <KoToolManager.h>
#include <KoShapeManager.h>

#include <klocale.h>

namespace KPlato
{

ViewListDocker::ViewListDocker(View *view)
{
    updateWindowTitle( false );
    setView(view);
}

ViewListDocker::~ViewListDocker()
{
}

View *ViewListDocker::view()
{
    return m_view;
}

void ViewListDocker::setView(View *view)
{
    m_view = view;
    QWidget *wdg = widget();
    if (wdg)
        delete wdg;
    m_viewlist = new ViewListWidget(view->getPart(), this);
    setWidget(m_viewlist);
    m_viewlist->setProject( &( view->getProject() ) );
    connect( m_viewlist, SIGNAL( selectionChanged( ScheduleManager* ) ), view, SLOT( slotSelectionChanged( ScheduleManager* ) ) );
    connect( view, SIGNAL( currentScheduleManagerChanged( ScheduleManager* ) ), m_viewlist, SLOT( setSelectedSchedule( ScheduleManager* ) ) );
    connect( m_viewlist, SIGNAL( updateViewInfo( ViewListItem* ) ), view, SLOT( slotUpdateViewInfo( ViewListItem* ) ) );

}

void ViewListDocker::slotModified()
{
    setWindowTitle( i18nc( "@title:window", "View Selector [modified]" ) );
}

void ViewListDocker::updateWindowTitle( bool modified )
{
    if ( modified ) {
        setWindowTitle( i18nc( "@title:window", "View Selector [modified]" ) );
    } else {
        setWindowTitle(i18nc( "@title:window", "View Selector"));
    }
}

//----------
ViewListDockerFactory::ViewListDockerFactory(View *view)
{
    m_view = view;
}

QString ViewListDockerFactory::id() const
{
    return QString("KPlatoViewList");
}

QDockWidget* ViewListDockerFactory::createDockWidget()
{
    ViewListDocker *widget = new ViewListDocker(m_view);
    widget->setObjectName(id());

    return widget;
}

} //namespace KPlato

#include "kptviewlistdocker.moc"

