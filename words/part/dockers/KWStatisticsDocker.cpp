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

#include <QDebug>
#include <klocale.h>
#include <kdebug.h>
#include <KoToolManager.h>
#include <KoShapeManager.h>
#include <KoCanvasResourceManager.h>
#include "KWCanvas.h"

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
    } else {
        m_canvasReset = false;
    }
 
    m_statisticsDock = new KWStatistics(canvas->resourceManager(),
                                        canvas->document(),
                                        canvas->shapeManager()->selection(),
                                        this);
    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(ondockLocationChanged(Qt::DockWidgetArea)));
    setWidget(m_statisticsDock->m_statsWidget);
    initLayout();
    m_statisticsDock->m_statsWidget->setLayout(m_mainBox);
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
        m_mainBox->setDirection(QBoxLayout::LeftToRight);
    } else {
        m_mainBox->setDirection(QBoxLayout::TopToBottom);
    }
}

void KWStatisticsDocker::initLayout()
{
    m_mainBox = new QBoxLayout(QBoxLayout::LeftToRight, m_statisticsDock->m_statsWidget);
    m_wordsLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_wordsLayout);
    m_wordsLayout->addWidget(m_statisticsDock->m_wordsLabel);
    m_wordsLayout->addWidget(m_statisticsDock->m_countWords);

    m_sentencesLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_sentencesLayout);
    m_sentencesLayout->addWidget(m_statisticsDock->m_sentencesLabel);
    m_sentencesLayout->addWidget(m_statisticsDock->m_countSentences);

    m_syllablesLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_syllablesLayout);
    m_syllablesLayout->addWidget(m_statisticsDock->m_syllablesLabel);
    m_syllablesLayout->addWidget(m_statisticsDock->m_countSyllables);

    m_cjkcharsLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_cjkcharsLayout);
    m_cjkcharsLayout->addWidget(m_statisticsDock->m_cjkcharsLabel);
    m_cjkcharsLayout->addWidget(m_statisticsDock->m_countCjkchars);

    m_spacesLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_spacesLayout);
    m_spacesLayout->addWidget(m_statisticsDock->m_spacesLabel);
    m_spacesLayout->addWidget(m_statisticsDock->m_countSpaces);

    m_nospacesLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_nospacesLayout);
    m_nospacesLayout->addWidget(m_statisticsDock->m_nospacesLabel);
    m_nospacesLayout->addWidget(m_statisticsDock->m_countNospaces);

    m_fleschLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_fleschLayout);
    m_fleschLayout->addWidget(m_statisticsDock->m_fleschLabel);
    m_fleschLayout->addWidget(m_statisticsDock->m_countFlesch);

    m_linesLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_linesLayout);
    m_linesLayout->addWidget(m_statisticsDock->m_linesLabel);
    m_linesLayout->addWidget(m_statisticsDock->m_countLines);

    // The button that opens the preferences dialog.
    m_mainBox->addWidget(m_statisticsDock->m_preferencesButton);
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


