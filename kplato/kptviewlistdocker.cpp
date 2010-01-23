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

#include <KoToolManager.h>
#include <KoShapeManager.h>
#include <KoResourceManager.h>

#include <klocale.h>
#include <kdebug.h>

namespace KPlato
{

ViewListDocker::ViewListDocker(View *view)
{
    setWindowTitle(i18n("View Selector"));
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
    m_viewlist = new ViewListWidget(view->getPart(), view);
    setWidget(m_viewlist);
}

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

