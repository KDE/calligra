/* This file is part of the KDE project
 * Copyright (C) 2007 Fredy Yanardi <fyanardi@gmail.com>
 * Copyright (C) 2010-2011 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2012 Shreya Pandit <shreya@shreyapandit.com>
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
    setWidget(statisticsDock->statsWidget);
    initLayout();
    statisticsDock->statsWidget->setLayout(mainBox);
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
    mainBox = new QBoxLayout(QBoxLayout::LeftToRight,statisticsDock->statsWidget);
    wordsLayout = new QHBoxLayout();
    mainBox->addLayout(wordsLayout);
    wordsLayout->addWidget(statisticsDock->words);
    wordsLayout->addWidget(statisticsDock->count_words);

    sentencesLayout = new QHBoxLayout();
    mainBox->addLayout(sentencesLayout);
    sentencesLayout->addWidget(statisticsDock->sentences);
    sentencesLayout->addWidget(statisticsDock->count_sentences);

    syllablesLayout = new QHBoxLayout();
    mainBox->addLayout(syllablesLayout);
    syllablesLayout->addWidget(statisticsDock->syllables);
    syllablesLayout->addWidget(statisticsDock->count_syllables);

    cjkcharsLayout = new QHBoxLayout();
    mainBox->addLayout(cjkcharsLayout);
    cjkcharsLayout->addWidget(statisticsDock->cjkchars);
    cjkcharsLayout->addWidget(statisticsDock->count_cjkchars);

    spacesLayout = new QHBoxLayout();
    mainBox->addLayout(spacesLayout);
    spacesLayout->addWidget(statisticsDock->spaces);
    spacesLayout->addWidget(statisticsDock->count_spaces);

    nospacesLayout = new QHBoxLayout();
    mainBox->addLayout(nospacesLayout);
    nospacesLayout->addWidget(statisticsDock->nospaces);
    nospacesLayout->addWidget(statisticsDock->count_nospaces);

    fleschLayout = new QHBoxLayout();
    mainBox->addLayout(fleschLayout);
    fleschLayout->addWidget(statisticsDock->flesch);
    fleschLayout->addWidget(statisticsDock->count_flesch);

    linesLayout = new QHBoxLayout();
    mainBox->addLayout(linesLayout);
    linesLayout->addWidget(statisticsDock->lines);
    linesLayout->addWidget(statisticsDock->count_lines);
    mainBox->addWidget(statisticsDock->preferencesButton);
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


