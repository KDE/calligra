/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Alexia Allanic <alexia_allanic@yahoo.fr>
 * SPDX-FileCopyrightText: 2009 Jérémy Lugagne <jejewindsurf@hotmail.com>
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KPrPresentationDrawStrategy.h"

#include <QApplication>
#include <QKeyEvent>
#include <QStandardPaths>

#include <KoPACanvasBase.h>

#include "KPrPresentationDrawWidget.h"
#include "KPrPresentationTool.h"

KPrPresentationDrawStrategy::KPrPresentationDrawStrategy(KPrPresentationTool *tool)
    : KPrPresentationStrategyBase(tool)
{
    m_widget = new KPrPresentationDrawWidget(canvas());
    QPixmap pix(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "calligrastage/cursors/pen.png"));
    const qreal factor = 1.2;
    const int oldWidth = pix.width();
    const int oldHeight = pix.height();
    pix = pix.scaled(oldWidth * factor, oldHeight * factor);
    QCursor cur(pix);
    QApplication::setOverrideCursor(cur);

    setToolWidgetParent(m_widget);
    m_widget->show();
    m_widget->installEventFilter(m_tool);
}

KPrPresentationDrawStrategy::~KPrPresentationDrawStrategy()
{
    setToolWidgetParent(canvas()->canvasWidget());
    QApplication::restoreOverrideCursor();
}

bool KPrPresentationDrawStrategy::keyPressEvent(QKeyEvent *event)
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
