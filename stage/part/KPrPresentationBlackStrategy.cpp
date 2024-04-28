/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KPrPresentationBlackStrategy.h"

#include <QApplication>
#include <QKeyEvent>

#include <KoPACanvasBase.h>

#include "KPrPresentationBlackWidget.h"
#include "KPrPresentationTool.h"

KPrPresentationBlackStrategy::KPrPresentationBlackStrategy(KPrPresentationTool *tool)
    : KPrPresentationStrategyBase(tool)
{
    m_widget = new KPrPresentationBlackWidget(canvas());
    // TODO
    setToolWidgetParent(m_widget);
    m_widget->show();
    m_widget->installEventFilter(m_tool);
}

KPrPresentationBlackStrategy::~KPrPresentationBlackStrategy()
{
    setToolWidgetParent(canvas()->canvasWidget());
}

bool KPrPresentationBlackStrategy::keyPressEvent(QKeyEvent *event)
{
    bool handled = true;
    switch (event->key()) {
    case Qt::Key_Escape:
        activateDefaultStrategy();
        break;
    case Qt::Key_H:
        handled = false;
        break;
    }
    return handled;
}
