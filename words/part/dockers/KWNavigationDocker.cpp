/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWNavigationDocker.h"

#include <KWCanvas.h>
#include <KoCanvasResourceManager.h>

#include <dockers/KWNavigationWidget.h>

#include <KLocalizedString>

KWNavigationDocker::KWNavigationDocker()
    : m_canvasReset(false)
    , m_navigationWidget(new KWNavigationWidget(this))
{
    setWindowTitle(i18n("Navigation"));

    setWidget(m_navigationWidget);
}

KWNavigationDocker::~KWNavigationDocker() = default;

void KWNavigationDocker::setCanvas(KoCanvasBase *_canvas)
{
    KWCanvas *canvas = dynamic_cast<KWCanvas *>(_canvas);
    m_navigationWidget->setCanvas(canvas);
}

void KWNavigationDocker::unsetCanvas()
{
    m_navigationWidget->unsetCanvas();
}
