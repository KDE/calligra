/* This file is part of the KDE project
 * Copyright (C) 2007 Fredy Yanardi <fyanardi@gmail.com>
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
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

#include "KWStatisticsDocker.h"

#include "KWView.h"
#include "KWCanvas.h"
#include "dialogs/KWStatistics.h"

#include <KoToolManager.h>
#include <KoShapeManager.h>
#include <KoResourceManager.h>

#include <klocale.h>
#include <kdebug.h>

KWStatisticsDocker::KWStatisticsDocker(KWView *view)
{
    setWindowTitle(i18n("Statistics"));
    setView(view);
}

KWStatisticsDocker::~KWStatisticsDocker()
{
}

KWView *KWStatisticsDocker::view()
{
    return m_view;
}

void KWStatisticsDocker::setView(KWView *view)
{
    m_view = view;
    QWidget *wdg = widget();
    if (wdg)
        delete wdg;
    KWStatistics *statistics = new KWStatistics(view->canvasBase()->resourceManager(), view->kwdocument(),
            view->canvasBase()->shapeManager()->selection(), this);
    setWidget(statistics);
}

KWStatisticsDockerFactory::KWStatisticsDockerFactory(KWView *view)
{
    m_view = view;
}

QString KWStatisticsDockerFactory::id() const
{
    return QString("Statistics");
}

QDockWidget *KWStatisticsDockerFactory::createDockWidget()
{
    KWStatisticsDocker *widget = new KWStatisticsDocker(m_view);
    widget->setObjectName(id());

    return widget;
}


