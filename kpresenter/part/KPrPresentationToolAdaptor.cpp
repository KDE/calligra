/* This file is part of the KDE project
 * Copyright (C) 2010 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
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

#include "KPrPresentationToolAdaptor.h"

#include "KPrPresentationStrategyBase.h"
#include "KPrPresentationTool.h"
#include "KPrPresentationDrawWidget.h"
#include "KPrPresentationDrawStrategy.h"
#include "KPrPresentationHighlightWidget.h"
#include "KPrPresentationHighlightStrategy.h"
#include "KPrPresentationBlackWidget.h"
#include "KPrPresentationBlackStrategy.h"
#include "KPrViewModePresentation.h"

#include <QMouseEvent>

KPrPresentationToolAdaptor::KPrPresentationToolAdaptor(KPrPresentationTool* tool)
: QDBusAbstractAdaptor(tool)
, m_tool(tool)
, m_viewModePresentation(tool->viewModePresentation())
{

}

KPrPresentationToolAdaptor::~KPrPresentationToolAdaptor()
{

}

void KPrPresentationToolAdaptor::blankPresentation()
{
    if (m_viewModePresentation.isActivated() && ! dynamic_cast<KPrPresentationBlackStrategy *>(m_tool->strategy())) {
        m_tool->blackPresentation();
    }
}

void KPrPresentationToolAdaptor::highlightPresentation(int pointx, int pointy)
 {
    if (m_viewModePresentation.isActivated()) {
        if (! dynamic_cast<KPrPresentationHighlightStrategy *>(m_tool->strategy())) {
            m_tool->highlightPresentation();
        }

        QPoint point(pointx,pointy);
        QMouseEvent event(QEvent::MouseMove, point, Qt::NoButton, Qt::LeftButton, Qt::NoModifier);

        m_tool->strategy()->widget()->receiveMouseMoveEvent(&event);
    }
}

void KPrPresentationToolAdaptor::startDrawPresentation(int pointx, int pointy, int penSize, QString color)
{
    if (m_viewModePresentation.isActivated()) {
        if (! dynamic_cast< KPrPresentationDrawStrategy* >(m_tool->strategy())) {
            m_tool->drawOnPresentation();
            KPrPresentationDrawWidget *widget=dynamic_cast< KPrPresentationDrawWidget *>(m_tool->strategy()->widget());
            widget->updateSize(penSize);
            widget->updateColor(color);
            QPoint point(pointx,pointy);
            QMouseEvent event(QEvent::MouseButtonPress, point, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

            m_tool->strategy()->widget()->receiveMousePressEvent(&event);
        }
    }
}

void KPrPresentationToolAdaptor::drawOnPresentation(int pointx,int pointy)
{
    if (m_viewModePresentation.isActivated() && dynamic_cast<KPrPresentationDrawStrategy*>(m_tool->strategy()) ) {
        QPoint point(pointx,pointy);
        QMouseEvent event(QEvent::MouseMove, point, Qt::NoButton, Qt::LeftButton, Qt::NoModifier);

        m_tool->strategy()->widget()->receiveMouseMoveEvent(&event);
    }
}

void KPrPresentationToolAdaptor::stopDrawPresentation() {
    if (m_viewModePresentation.isActivated()) {
        QPoint point(0,0);
        QMouseEvent event(QEvent::MouseButtonRelease, point, Qt::NoButton, Qt::LeftButton, Qt::NoModifier);

        m_tool->strategy()->widget()->receiveMouseReleaseEvent(&event);
    }
}

void KPrPresentationToolAdaptor::normalPresentation()
{
    m_tool->normalPresentation();
}


#include "KPrPresentationToolAdaptor.moc"
