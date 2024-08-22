/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 * SPDX-FileCopyrightText: 2010-2011 Boudewijn Rempt <boud@kogmbh.com>
 * SPDX-FileCopyrightText: 2012 Shreya Pandit <shreya@shreyapandit.com>
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWStatisticsDocker.h"

#include "KWCanvas.h"
#include <KoCanvasResourceManager.h>

#include <KLocalizedString>

KWStatisticsDocker::KWStatisticsDocker()
{
    m_canvasReset = false;
    setWindowTitle(i18n("Statistics"));

    m_statisticsWidget = new KWStatisticsWidget(this);
    connect(this, &QDockWidget::dockLocationChanged, this, &KWStatisticsDocker::ondockLocationChanged);
    setWidget(m_statisticsWidget);
}

KWStatisticsDocker::~KWStatisticsDocker() = default;

void KWStatisticsDocker::setCanvas(KoCanvasBase *_canvas)
{
    KWCanvas *canvas = dynamic_cast<KWCanvas *>(_canvas);
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

KWStatisticsDockerFactory::KWStatisticsDockerFactory() = default;

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
