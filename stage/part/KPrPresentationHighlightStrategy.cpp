/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Alexia Allanic <alexia_allanic@yahoo.fr>
 * SPDX-FileCopyrightText: 2009 Jérémy Lugagne <jejewindsurf@hotmail.com>
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrPresentationHighlightStrategy.h"

#include <QKeyEvent>

#include <KoPACanvasBase.h>

#include "KPrPresentationHighlightWidget.h"
#include "KPrPresentationTool.h"

KPrPresentationHighlightStrategy::KPrPresentationHighlightStrategy(KPrPresentationTool *tool)
    : KPrPresentationStrategyBase(tool)
{
    m_widget = new KPrPresentationHighlightWidget(canvas());
    setToolWidgetParent(m_widget);
    m_widget->show();
    m_widget->installEventFilter(m_tool);
}

KPrPresentationHighlightStrategy::~KPrPresentationHighlightStrategy()
{
    setToolWidgetParent(canvas()->canvasWidget());
}

bool KPrPresentationHighlightStrategy::keyPressEvent(QKeyEvent *event)
{
    bool handled = true;
    switch (event->key()) {
    case Qt::Key_Escape:
        activateDefaultStrategy();
        break;
    case Qt::Key_P:
        handled = false;
        break;
    }
    return handled;
}
