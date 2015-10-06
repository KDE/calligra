/* This file is part of the KDE project
 * Copyright (C) 2014 Denis Kuplyakov <dener.kup@gmail.com>
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

#include "KWNavigationDocker.h"

#include <KoCanvasResourceManager.h>
#include <KWCanvas.h>

#include <dockers/KWNavigationWidget.h>

#include <klocalizedstring.h>

KWNavigationDocker::KWNavigationDocker()
    : m_canvasReset(false)
    , m_navigationWidget(new KWNavigationWidget(this))
{
    setWindowTitle(i18n("Navigation"));

    setWidget(m_navigationWidget);
}

KWNavigationDocker::~KWNavigationDocker()
{
}

void KWNavigationDocker::setCanvas(KoCanvasBase *_canvas)
{
    KWCanvas *canvas = dynamic_cast<KWCanvas*>(_canvas);
    m_navigationWidget->setCanvas(canvas);
}

void KWNavigationDocker::unsetCanvas()
{
    m_navigationWidget->unsetCanvas();
}
