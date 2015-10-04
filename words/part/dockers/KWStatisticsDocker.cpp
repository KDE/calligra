/* This file is part of the KDE project
 * Copyright (C) 2007 Fredy Yanardi <fyanardi@gmail.com>
 * Copyright (C) 2010-2011 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2012 Shreya Pandit <shreya@shreyapandit.com>
 * Copyright (C) 2012 Inge Wallin <inge@lysator.liu.se>
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

#include <KoCanvasResourceManager.h>
#include "KWCanvas.h"

#include <klocalizedstring.h>

KWStatisticsDocker::KWStatisticsDocker()
{
    m_canvasReset = false;
    setWindowTitle(i18n("Statistics"));

    m_statisticsWidget = new KWStatisticsWidget(this);
    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(ondockLocationChanged(Qt::DockWidgetArea)));
    setWidget(m_statisticsWidget);
}

KWStatisticsDocker::~KWStatisticsDocker()
{
}

void KWStatisticsDocker::setCanvas(KoCanvasBase *_canvas)
{
    KWCanvas *canvas = dynamic_cast<KWCanvas*>(_canvas);
    m_statisticsWidget->setCanvas(canvas);
}

void KWStatisticsDocker::unsetCanvas()
{
    m_statisticsWidget->unsetCanvas();
}

void KWStatisticsDocker::ondockLocationChanged(Qt::DockWidgetArea newArea)
{
    if (newArea == Qt::TopDockWidgetArea || newArea == Qt::BottomDockWidgetArea) {
        m_statisticsWidget->setLayoutDirection(KWStatisticsWidget::LayoutHorizontal);
    } else {
        m_statisticsWidget->setLayoutDirection(KWStatisticsWidget::LayoutVertical);
    }
}


KWStatisticsDockerFactory::KWStatisticsDockerFactory()
{
}

QString KWStatisticsDockerFactory::id() const
{
    return QString("Statistics");
}

QDockWidget *KWStatisticsDockerFactory::createDockWidget()
{
    KWStatisticsDocker *widget = new KWStatisticsDocker();
    widget->setObjectName(id());
    return widget;
}


