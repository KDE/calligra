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

#include <QMainWindow>

KoModeBoxDocker::KoModeBoxDocker(KoModeBox *modeBox)
    : m_modeBox(modeBox)
{
    setWidget(modeBox);
    setFeatures(QDockWidget::NoDockWidgetFeatures);
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setObjectName("ModeBox");
    setTitleBarWidget(new QWidget(this));
    connect(modeBox, &KoModeBox::switchTabsSide, [this](KoModeBox::HorizontalTabsSide side) {
        auto mainWindow = qobject_cast<QMainWindow *>(parent());
        if (!mainWindow) {
            return;
        }
        if (side == KoModeBox::LeftSide) {
            mainWindow->addDockWidget(Qt::LeftDockWidgetArea, this);
        } else {
            mainWindow->addDockWidget(Qt::RightDockWidgetArea, this);
        }
    });
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
