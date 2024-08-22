/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWDebugDocker.h"

#include <KWCanvas.h>

#include <dockers/KWDebugWidget.h>

#include <KLocalizedString>

KWDebugDocker::KWDebugDocker()
    : m_debugWidget(new KWDebugWidget(this))
{
    setWindowTitle(i18n("Debug"));

    setWidget(m_debugWidget);
}

KWDebugDocker::~KWDebugDocker() = default;

void KWDebugDocker::setCanvas(KoCanvasBase *_canvas)
{
    KWCanvas *canvas = dynamic_cast<KWCanvas *>(_canvas);
    m_debugWidget->setCanvas(canvas);
}

void KWDebugDocker::unsetCanvas()
{
    m_debugWidget->unsetCanvas();
}
