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
    if (newArea == 8 || newArea == 4)
	updateHorizontalUi();
    else
	updateVerticalUi();
      
}
void KWStatisticsDocker::updateVerticalUi()
{
    statisticsDock->widgetDocker.countLayout->addWidget(statisticsDock->widgetDocker.count_words);
    statisticsDock->widgetDocker.countLayout->addWidget(statisticsDock->widgetDocker.count_sentences);
    statisticsDock->widgetDocker.countLayout->addWidget(statisticsDock->widgetDocker.count_syllables);
    statisticsDock->widgetDocker.countLayout->addWidget(statisticsDock->widgetDocker.count_lines);
    statisticsDock->widgetDocker.countLayout->addWidget(statisticsDock->widgetDocker.count_spaces);
    statisticsDock->widgetDocker.countLayout->addWidget(statisticsDock->widgetDocker.count_nospaces);
    statisticsDock->widgetDocker.countLayout->addWidget(statisticsDock->widgetDocker.count_cjkchars);
    statisticsDock->widgetDocker.countLayout->addWidget(statisticsDock->widgetDocker.count_flesch);

    statisticsDock->widgetDocker.labelLayout->addWidget(statisticsDock->widgetDocker.words);
    statisticsDock->widgetDocker.labelLayout->addWidget(statisticsDock->widgetDocker.sentences);
    statisticsDock->widgetDocker.labelLayout->addWidget(statisticsDock->widgetDocker.syllables);
    statisticsDock->widgetDocker.labelLayout->addWidget(statisticsDock->widgetDocker.lines);
    statisticsDock->widgetDocker.labelLayout->addWidget(statisticsDock->widgetDocker.spaces);
    statisticsDock->widgetDocker.labelLayout->addWidget(statisticsDock->widgetDocker.nospaces);
    statisticsDock->widgetDocker.labelLayout->addWidget(statisticsDock->widgetDocker.cjkchars);
    statisticsDock->widgetDocker.labelLayout->addWidget(statisticsDock->widgetDocker.flesch); 
    
}

void KWStatisticsDocker::updateHorizontalUi()
{
    QBoxLayout *hbox = new QBoxLayout(QBoxLayout::LeftToRight, statisticsDock);
    statisticsDock->setLayout(hbox);
    statisticsDock->updateGeometry();
    hbox->addWidget(statisticsDock->widgetDocker.words);
    hbox->addWidget(statisticsDock->widgetDocker.count_words);

    hbox->addWidget(statisticsDock->widgetDocker.sentences);
    hbox->addWidget(statisticsDock->widgetDocker.count_sentences);
  
    hbox->addWidget(statisticsDock->widgetDocker.syllables);
    hbox->addWidget(statisticsDock->widgetDocker.count_syllables);

    hbox->addWidget(statisticsDock->widgetDocker.lines);
    hbox->addWidget(statisticsDock->widgetDocker.count_lines);

    hbox->addWidget(statisticsDock->widgetDocker.spaces);
    hbox->addWidget(statisticsDock->widgetDocker.count_spaces);

    hbox->addWidget(statisticsDock->widgetDocker.nospaces);
    hbox->addWidget(statisticsDock->widgetDocker.count_nospaces);	

    hbox->addWidget(statisticsDock->widgetDocker.cjkchars);
    hbox->addWidget(statisticsDock->widgetDocker.count_cjkchars);

    hbox->addWidget(statisticsDock->widgetDocker.flesch);
    hbox->addWidget(statisticsDock->widgetDocker.count_flesch);
    hbox->addWidget(statisticsDock->widgetDocker.preferences);
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


