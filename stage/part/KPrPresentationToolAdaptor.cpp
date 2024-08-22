/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrPresentationToolAdaptor.h"

#include "KPrPresentationBlackStrategy.h"
#include "KPrPresentationBlackWidget.h"
#include "KPrPresentationDrawStrategy.h"
#include "KPrPresentationDrawWidget.h"
#include "KPrPresentationHighlightStrategy.h"
#include "KPrPresentationHighlightWidget.h"
#include "KPrPresentationStrategyBase.h"
#include "KPrPresentationTool.h"
#include "KPrViewModePresentation.h"

#include <QMouseEvent>

KPrPresentationToolAdaptor::KPrPresentationToolAdaptor(KPrPresentationTool *tool)
    : QDBusAbstractAdaptor(tool)
    , m_tool(tool)
    , m_viewModePresentation(tool->viewModePresentation())
{
}

KPrPresentationToolAdaptor::~KPrPresentationToolAdaptor() = default;

void KPrPresentationToolAdaptor::blankPresentation()
{
    if (m_viewModePresentation.isActivated() && !dynamic_cast<KPrPresentationBlackStrategy *>(m_tool->strategy())) {
        m_tool->blackPresentation();
    }
}

void KPrPresentationToolAdaptor::highlightPresentation(int pointx, int pointy)
{
    if (m_viewModePresentation.isActivated()) {
        if (!dynamic_cast<KPrPresentationHighlightStrategy *>(m_tool->strategy())) {
            m_tool->highlightPresentation();
        }

        QPoint point(pointx, pointy);
        QMouseEvent event(QEvent::MouseMove, point, Qt::NoButton, Qt::LeftButton, Qt::NoModifier);

        m_tool->strategy()->widget()->receiveMouseMoveEvent(&event);
    }
}

void KPrPresentationToolAdaptor::startDrawPresentation(int pointx, int pointy, int penSize, const QString &color)
{
    if (m_viewModePresentation.isActivated()) {
        if (!dynamic_cast<KPrPresentationDrawStrategy *>(m_tool->strategy())) {
            m_tool->drawOnPresentation();
            KPrPresentationDrawWidget *widget = static_cast<KPrPresentationDrawWidget *>(m_tool->strategy()->widget());
            widget->updateSize(penSize);
            widget->updateColor(color);
        }

        QPoint point(pointx, pointy);
        QMouseEvent event(QEvent::MouseButtonPress, point, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

        m_tool->strategy()->widget()->receiveMousePressEvent(&event);
    }
}

void KPrPresentationToolAdaptor::drawOnPresentation(int pointx, int pointy)
{
    if (m_viewModePresentation.isActivated() && dynamic_cast<KPrPresentationDrawStrategy *>(m_tool->strategy())) {
        QPoint point(pointx, pointy);
        QMouseEvent event(QEvent::MouseMove, point, Qt::NoButton, Qt::LeftButton, Qt::NoModifier);

        m_tool->strategy()->widget()->receiveMouseMoveEvent(&event);
    }
}

void KPrPresentationToolAdaptor::stopDrawPresentation()
{
    if (m_viewModePresentation.isActivated()) {
        QPoint point(0, 0);
        QMouseEvent event(QEvent::MouseButtonRelease, point, Qt::NoButton, Qt::LeftButton, Qt::NoModifier);

        m_tool->strategy()->widget()->receiveMouseReleaseEvent(&event);
    }
}

void KPrPresentationToolAdaptor::normalPresentation()
{
    m_tool->normalPresentation();
}
