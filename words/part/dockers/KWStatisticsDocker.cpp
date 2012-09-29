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
//#include "ui_KWStatisticsDocker.h"
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
    count = 0;
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
    } else {
        m_canvasReset = false;
    }
 
    statisticsDock = new KWStatistics(canvas->resourceManager(),
                                                canvas->document(),
                                                canvas->shapeManager()->selection(),
                                                this);
    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(ondockLocationChanged(Qt::DockWidgetArea)));
    setWidget(statisticsDock->m_statsWidget);
    initLayout();
    statisticsDock->m_statsWidget->setLayout(mainBox);
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
    if (newArea == 8 || newArea == 4) {
        mainBox->setDirection(QBoxLayout::LeftToRight);
    } else {
        mainBox->setDirection(QBoxLayout::TopToBottom);
    }
}

void KWStatisticsDocker::initLayout()
{
    mainBox = new QBoxLayout(QBoxLayout::LeftToRight, statisticsDock->m_statsWidget);
    wordsLayout = new QHBoxLayout();
    mainBox->addLayout(wordsLayout);
    wordsLayout->addWidget(statisticsDock->m_wordsLabel);
    wordsLayout->addWidget(statisticsDock->m_countWords);

    sentencesLayout = new QHBoxLayout();
    mainBox->addLayout(sentencesLayout);
    sentencesLayout->addWidget(statisticsDock->m_sentencesLabel);
    sentencesLayout->addWidget(statisticsDock->m_countSentences);

    syllablesLayout = new QHBoxLayout();
    mainBox->addLayout(syllablesLayout);
    syllablesLayout->addWidget(statisticsDock->m_syllablesLabel);
    syllablesLayout->addWidget(statisticsDock->m_countSyllables);

    cjkcharsLayout = new QHBoxLayout();
    mainBox->addLayout(cjkcharsLayout);
    cjkcharsLayout->addWidget(statisticsDock->m_cjkcharsLabel);
    cjkcharsLayout->addWidget(statisticsDock->m_countCjkchars);

    spacesLayout = new QHBoxLayout();
    mainBox->addLayout(spacesLayout);
    spacesLayout->addWidget(statisticsDock->m_spacesLabel);
    spacesLayout->addWidget(statisticsDock->m_countSpaces);

    nospacesLayout = new QHBoxLayout();
    mainBox->addLayout(nospacesLayout);
    nospacesLayout->addWidget(statisticsDock->m_nospacesLabel);
    nospacesLayout->addWidget(statisticsDock->m_countNospaces);

    fleschLayout = new QHBoxLayout();
    mainBox->addLayout(fleschLayout);
    fleschLayout->addWidget(statisticsDock->m_fleschLabel);
    fleschLayout->addWidget(statisticsDock->m_countFlesch);

    linesLayout = new QHBoxLayout();
    mainBox->addLayout(linesLayout);
    linesLayout->addWidget(statisticsDock->m_linesLabel);
    linesLayout->addWidget(statisticsDock->m_countLines);
    mainBox->addWidget(statisticsDock->m_preferencesButton);
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


