/* This file is part of the KDE project
 * Copyright (C) 2007 Fredy Yanardi <fyanardi@gmail.com>
 * Copyright (C) 2010-2011 Boudewijn Rempt <boud@kogmbh.com>
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
#include "ui_KWStatisticsDocker.h"
#include "KWCanvas.h"
#include <QDebug>

#include <KoToolManager.h>
#include <KoShapeManager.h>
#include <KoCanvasResourceManager.h>

#include <klocale.h>
#include <kdebug.h>

KWStatisticsDocker::KWStatisticsDocker()
{
    m_canvasReset = false;
    setWindowTitle(i18n("Statistics"));
}

KWStatisticsDocker::~KWStatisticsDocker()
{
}

void KWStatisticsDocker::setCanvas(KoCanvasBase *_canvas)
{

    KWCanvas *canvas = dynamic_cast<KWCanvas*>(_canvas);

    QWidget *wdg = widget();
    if (wdg) {
        delete wdg;
        m_canvasReset = true;
    } else
        m_canvasReset = false;

    statisticsDock = new KWStatistics(canvas->resourceManager(),
                                                canvas->document(),
                                                canvas->shapeManager()->selection(),
                                                this);

    setWidget(statisticsDock);
    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(ondockLocationChanged(Qt::DockWidgetArea)));

}

void KWStatisticsDocker::unsetCanvas()
{
    if (!m_canvasReset) {
        delete widget();
        setWidget(0);
    }
}
void KWStatisticsDocker::ondockLocationChanged(Qt::DockWidgetArea newArea)
{
    if(newArea == 8) {
	updateHorizontalUi();
    }
}

void KWStatisticsDocker::updateHorizontalUi()
{
  statisticsDock->widgetDocker.groupBox->hide();
  statisticsDock->widgetDocker.bottomLayout->addWidget(statisticsDock->widgetDocker.Words);
  statisticsDock->widgetDocker.bottomLayout->addWidget(statisticsDock->widgetDocker.Words);
  statisticsDock->widgetDocker.bottomLayout->addWidget(statisticsDock->widgetDocker.count_words);

  statisticsDock->widgetDocker.bottomLayout->addWidget(statisticsDock->widgetDocker.Sentences);
  statisticsDock->widgetDocker.bottomLayout->addWidget(statisticsDock->widgetDocker.count_sentences);
  
  statisticsDock->widgetDocker.bottomLayout->addWidget(statisticsDock->widgetDocker.Syllables);
  statisticsDock->widgetDocker.bottomLayout->addWidget(statisticsDock->widgetDocker.count_syllables);

  statisticsDock->widgetDocker.bottomLayout->addWidget(statisticsDock->widgetDocker.Lines);
  statisticsDock->widgetDocker.bottomLayout->addWidget(statisticsDock->widgetDocker.count_lines);

  statisticsDock->widgetDocker.bottomLayout->addWidget(statisticsDock->widgetDocker.spaces);
  statisticsDock->widgetDocker.bottomLayout->addWidget(statisticsDock->widgetDocker.count_spaces);

  statisticsDock->widgetDocker.bottomLayout->addWidget(statisticsDock->widgetDocker.nospaces);
  statisticsDock->widgetDocker.bottomLayout->addWidget(statisticsDock->widgetDocker.count_nospaces);
  statisticsDock->widgetDocker.bottomLayout->addWidget(statisticsDock->widgetDocker.Cjkchars);
  statisticsDock->widgetDocker.bottomLayout->addWidget(statisticsDock->widgetDocker.count_cjkchars);

  statisticsDock->widgetDocker.bottomLayout->addWidget(statisticsDock->widgetDocker.Flesch);
  statisticsDock->widgetDocker.bottomLayout->addWidget(statisticsDock->widgetDocker.count_flesch);

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


