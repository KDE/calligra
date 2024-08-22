/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrPreviewWidget.h"

#include <KoShapeContainer.h>
#include <KoShapePainter.h>
#include <KoZoomHandler.h>
#include <QMouseEvent>
#include <QPainter>

#include "KPrPage.h"
#include "StageDebug.h"
#include "pageeffects/KPrPageEffect.h"
#include "pageeffects/KPrPageEffectRunner.h"

KPrPreviewWidget::KPrPreviewWidget(QWidget *parent)
    : QWidget(parent)
    , m_pageEffect(nullptr)
    , m_pageEffectRunner(nullptr)
    , m_page(nullptr)
{
    connect(&m_timeLine, &QTimeLine::valueChanged, this, &KPrPreviewWidget::animate);
}

KPrPreviewWidget::~KPrPreviewWidget() = default;

void KPrPreviewWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    if (m_pageEffectRunner && m_timeLine.state() == QTimeLine::Running) {
        m_pageEffectRunner->paint(p);
    } else if (m_page && !m_newPage.isNull()) {
        p.drawPixmap(rect().topLeft(), m_newPage);
    } else {
        p.drawLine(rect().topLeft(), rect().bottomRight());
        p.drawLine(rect().topRight(), rect().bottomLeft());
    }

    QPen pen(Qt::SolidLine);
    pen.setWidth(0);
    pen.setColor(palette().color(QPalette::Mid));
    p.setPen(pen);
    QRect framerect = rect();
    framerect.setWidth(framerect.width() - 1);
    framerect.setHeight(framerect.height() - 1);
    p.drawRect(framerect);

    QWidget::paintEvent(event);
}

void KPrPreviewWidget::resizeEvent(QResizeEvent *event)
{
    if (m_page)
        updatePixmaps();

    if (m_pageEffectRunner) {
        m_pageEffectRunner->setOldPage(m_oldPage);
        m_pageEffectRunner->setNewPage(m_newPage);
    }

    QWidget::resizeEvent(event);
}

void KPrPreviewWidget::setPageEffect(KPrPageEffect *pageEffect, KPrPage *page, KPrPage *prevpage)
{
    delete m_pageEffect;
    m_pageEffect = pageEffect;
    delete m_pageEffectRunner;
    m_pageEffectRunner = nullptr;

    m_page = page;
    m_prevpage = prevpage;

    if (m_page) {
        updatePixmaps();

        if (m_pageEffect) {
            m_pageEffectRunner = new KPrPageEffectRunner(m_oldPage, m_newPage, this, m_pageEffect);
        }
    }

    update();
}

void KPrPreviewWidget::animate()
{
    if (m_pageEffectRunner) {
        m_pageEffectRunner->next(m_timeLine.currentTime());
    }
}

void KPrPreviewWidget::runPreview()
{
    if (m_pageEffect) {
        m_timeLine.setDuration(m_pageEffect->duration());
        m_timeLine.setCurrentTime(0);
        m_timeLine.start();
    }
}

void KPrPreviewWidget::updatePixmaps()
{
    if (!m_page || !isVisible())
        return;

    m_newPage = m_page->thumbnail(size());

    if (m_newPage.isNull())
        return;

    if (m_prevpage && m_prevpage != m_page) {
        m_oldPage = m_prevpage->thumbnail(size());
    } else {
        QPixmap oldPage(size());
        oldPage.fill(QColor(Qt::black));
        m_oldPage = oldPage;
    }
}

void KPrPreviewWidget::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    runPreview();
}
