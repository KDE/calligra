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
    statisticsDock->setSizePolicy(QSizePolicy::Expanding);
    m_canvasReset = false;
    setWindowTitle(i18n("Statistics"));
}

KWStatisticsDocker::~KWStatisticsDocker()
{
}

void KWStatisticsDocker::updateDockerUi()
{
  /*
  QVBoxLayout * vnamelayout = new QVBoxLayout;
  vnamelayout->addWidget(new QLabel("Words"));
  vnamelayout->addWidget(new QLabel("Sentences"));
  vnamelayout->addWidget(new QLabel("Syllables"));
  vnamelayout->addWidget(new QLabel("Lines"));
  vnamelayout->addWidget(new QLabel("Flesch Reading Ease"));
  vnamelayout->addWidget(new QLabel("Characters (excluding spaces)"));
  vnamelayout->addWidget(new QLabel("Characters (including spaces)"));
  vnamelayout->addWidget(new QLabel("East Asian Characters"));
  
  QLabel * count_words  = new QLabel;
  QLabel * count_sentences  = new QLabel;
  QLabel * count_syllables = new QLabel;
  QLabel * count_lines  = new QLabel;
  QLabel * count_flesch  = new QLabel;
  QLabel * count_charnospaces = new QLabel;
  QLabel * count_charspaces  = new QLabel;
  QLabel * count_cjkchars = new QLabel;

  QVBoxLayout * valuelayout = new QVBoxLayout;
  valuelayout->addWidget(count_words);
  valuelayout->addWidget(count_sentences);
  valuelayout->addWidget(count_syllables);
  valuelayout->addWidget(count_lines);
  valuelayout->addWidget(count_flesch);
  valuelayout->addWidget(count_charnospaces);
  valuelayout->addWidget(count_charspaces);
  valuelayout->addWidget(count_cjkchars);
  QWidget * mainwidget =  new QWidget;
  QGridLayout *gridlayout = new QGridLayout;
  gridlayout.addLayout(vnamelayout,0,0);
  gridlayout.addLayout(valuelayout,0,1);
  mainwidget->setLayout(gridlayout);
 */ 
  
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
      qDebug()<< "The docker is in bottom area";
      updateHorizontalUi();
    }
}

void KWStatisticsDocker::updateHorizontalUi()
{

  
  statisticsDock->widgetDocker.horizontalLayout_3->addWidget(statisticsDock->widgetDocker.Words);
  statisticsDock->widgetDocker.horizontalLayout_3->addWidget(statisticsDock->widgetDocker.count_words);

  statisticsDock->widgetDocker.horizontalLayout_3->addWidget(statisticsDock->widgetDocker.Sentences);
  statisticsDock->widgetDocker.horizontalLayout_3->addWidget(statisticsDock->widgetDocker.count_sentences);
  
  statisticsDock->widgetDocker.horizontalLayout_3->addWidget(statisticsDock->widgetDocker.Syllables);
  statisticsDock->widgetDocker.horizontalLayout_3->addWidget(statisticsDock->widgetDocker.count_syllables);

  statisticsDock->widgetDocker.horizontalLayout_3->addWidget(statisticsDock->widgetDocker.Lines);
  statisticsDock->widgetDocker.horizontalLayout_3->addWidget(statisticsDock->widgetDocker.count_lines);

  statisticsDock->widgetDocker.horizontalLayout_3->addWidget(statisticsDock->widgetDocker.spaces);
  statisticsDock->widgetDocker.horizontalLayout_3->addWidget(statisticsDock->widgetDocker.count_spaces);

  statisticsDock->widgetDocker.horizontalLayout_3->addWidget(statisticsDock->widgetDocker.nospaces);
  statisticsDock->widgetDocker.horizontalLayout_3->addWidget(statisticsDock->widgetDocker.count_nospaces);

  statisticsDock->widgetDocker.horizontalLayout_3->addWidget(statisticsDock->widgetDocker.Cjkchars);
  statisticsDock->widgetDocker.horizontalLayout_3->addWidget(statisticsDock->widgetDocker.count_cjkchars);

  statisticsDock->widgetDocker.horizontalLayout_3->addWidget(statisticsDock->widgetDocker.Flesch);
  statisticsDock->widgetDocker.horizontalLayout_3->addWidget(statisticsDock->widgetDocker.count_flesch);

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


