/*
 * SPDX-FileCopyrightText: 2005-2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Peter Simonsson <peter.simonsson@gmail.com>
 * SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoModeBoxDocker_p.h"
#include "KoModeBox_p.h"
#include <KoCanvasController.h>

#include <WidgetsDebug.h>

KoModeBoxDocker::KoModeBoxDocker(KoModeBox *modeBox)
    : m_modeBox(modeBox)
{
    setWidget(modeBox);
    setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    setWindowTitle("");
    setObjectName("ModeBox");

    connect(this, &QDockWidget::dockLocationChanged, this, &KoModeBoxDocker::locationChanged);
}

void KoModeBoxDocker::setCanvas(KoCanvasBase *canvas)
{
    setEnabled(canvas != nullptr);
    m_modeBox->setCanvas(canvas);
}

void KoModeBoxDocker::unsetCanvas()
{
    setEnabled(false);
    m_modeBox->unsetCanvas();
}

void KoModeBoxDocker::locationChanged(Qt::DockWidgetArea area)
{
    m_modeBox->locationChanged(area);
}
