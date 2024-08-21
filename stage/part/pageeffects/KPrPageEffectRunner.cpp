/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrPageEffectRunner.h"
#ifdef HAVE_OPENGL
#include <QGLFormat>
#endif
#include "KPrPageEffect.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>

KPrPageEffectRunner::KPrPageEffectRunner(const QPixmap &oldPage, const QPixmap &newPage, QWidget *w, KPrPageEffect *effect)
    : m_effect(effect)
    , m_data(oldPage, newPage, w)
{
    if (m_effect->useGraphicsView()) {
        m_data.m_scene = new QGraphicsScene();
        m_data.m_graphicsView = new QGraphicsView(m_data.m_scene, m_data.m_widget);
        m_data.m_graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_data.m_graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_data.m_graphicsView->resize(m_data.m_widget->size());
        m_data.m_graphicsView->setFrameShape(QFrame::Panel);
        m_data.m_graphicsView->setLineWidth(0);

        m_data.m_oldPageItem = new QGraphicsPixmapItem(m_data.m_oldPage, nullptr);
        m_data.m_scene->addItem(m_data.m_oldPageItem);
        m_data.m_newPageItem = new QGraphicsPixmapItem(m_data.m_newPage, nullptr);
        m_data.m_scene->addItem(m_data.m_newPageItem);
        m_data.m_oldPageItem->hide();
        m_data.m_newPageItem->hide();

        // set a black background
        m_data.m_graphicsView->setBackgroundBrush(Qt::black);

        m_data.m_graphicsView->show();

        // Do some optimization
        m_data.m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);
        // m_data.m_graphicsView->setOptimizationFlag(QGraphicsView::DontClipPainter, true);
        m_data.m_graphicsView->setOptimizationFlag(QGraphicsView::DontSavePainterState, true);
        m_data.m_graphicsView->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
        m_data.m_graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

        // If computer support openGL
#ifdef HAVE_OPENGL
        if (QGLFormat::hasOpenGL()) {
            m_data.m_graphicsView->setRenderHints(QPainter::SmoothPixmapTransform);
            m_data.m_graphicsView->setRenderHints(QPainter::Antialiasing);
            m_data.m_graphicsView->setRenderHints(QPainter::HighQualityAntialiasing);
            QGLWidget *openGLWidget = new QGLWidget();
            openGLWidget->format().setSwapInterval(0);
            m_data.m_graphicsView->setViewport(openGLWidget);
        }
#endif
    }
    m_effect->setup(m_data, m_data.m_timeLine);
}

KPrPageEffectRunner::~KPrPageEffectRunner()
{
    delete m_data.m_graphicsView;
    delete m_data.m_scene;
}

bool KPrPageEffectRunner::paint(QPainter &painter)
{
    return m_effect->paint(painter, m_data);
}

void KPrPageEffectRunner::next(int currentTime)
{
    m_data.m_lastTime = m_data.m_currentTime;
    m_data.m_currentTime = currentTime;

    m_effect->next(m_data);
}

void KPrPageEffectRunner::finish()
{
    m_data.m_finished = true;
    m_effect->finish(m_data);
}

bool KPrPageEffectRunner::isFinished()
{
    return m_data.m_finished;
}

const QPixmap &KPrPageEffectRunner::oldPage() const
{
    return m_data.m_oldPage;
}

const QPixmap &KPrPageEffectRunner::newPage() const
{
    return m_data.m_newPage;
}

void KPrPageEffectRunner::setOldPage(const QPixmap &oldPage)
{
    m_data.m_oldPage = oldPage;
}

void KPrPageEffectRunner::setNewPage(const QPixmap &newPage)
{
    m_data.m_newPage = newPage;
}
