/*
 * SPDX-FileCopyrightText: 2005-2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Peter Simonsson <peter.simonsson@gmail.com>
 * SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoDockRegistry.h"
#include "KoDockWidgetTitleBar.h"
#include "KoToolBoxDocker_p.h"
#include "KoToolBoxScrollArea_p.h"
#include "KoToolBox_p.h"
#include <KLocalizedString>
#include <QFontMetrics>
#include <QFrame>
#include <QLabel>

KoToolBoxDocker::KoToolBoxDocker(KoToolBox *toolBox)
    : QDockWidget(i18n("Toolbox"))
    , m_toolBox(toolBox)
    , m_scrollArea(new KoToolBoxScrollArea(toolBox, this))
{
    setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    setWidget(m_scrollArea);

    // create title bar
    KoDockWidgetTitleBar *titleBar = new KoDockWidgetTitleBar(this);
    titleBar->setTextVisibilityMode(KoDockWidgetTitleBar::TextCanBeInvisible);
    titleBar->setToolTip(i18n("Tools"));
    setTitleBarWidget(titleBar);

    connect(this, &QDockWidget::dockLocationChanged, this, &KoToolBoxDocker::updateToolBoxOrientation);
    connect(this, &QDockWidget::topLevelChanged, this, &KoToolBoxDocker::updateFloating);
}

void KoToolBoxDocker::setCanvas(KoCanvasBase *canvas)
{
    Q_UNUSED(canvas);
}

void KoToolBoxDocker::unsetCanvas()
{
}

void KoToolBoxDocker::resizeEvent(QResizeEvent *event)
{
    QDockWidget::resizeEvent(event);
    if (isFloating()) {
        if (m_scrollArea->width() > m_scrollArea->height()) {
            m_scrollArea->setOrientation(Qt::Horizontal);
        } else {
            m_scrollArea->setOrientation(Qt::Vertical);
        }
    }
}

void KoToolBoxDocker::updateToolBoxOrientation(Qt::DockWidgetArea area)
{
    if (area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea) {
        m_scrollArea->setOrientation(Qt::Horizontal);
    } else {
        m_scrollArea->setOrientation(Qt::Vertical);
    }
}

void KoToolBoxDocker::updateFloating(bool v)
{
    m_toolBox->setFloating(v);
}
