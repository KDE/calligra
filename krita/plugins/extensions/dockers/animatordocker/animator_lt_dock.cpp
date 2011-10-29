/*
 *  Light table docker
 *  Copyright (C) 2011 Torio Mlshi <mlshi@lavabit.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <KLocale>

#include <KoDockRegistry.h>

#include <cstdlib>

#include "animator_lt_dock.h"

#include "animator_manager_factory.h"
#include "animator_lt_updater.h"

AnimatorLTDock::AnimatorLTDock() : QDockWidget(i18n("Light table"))
{
    setupUI();
}

AnimatorLTDock::~AnimatorLTDock()
{
}

void AnimatorLTDock::setupUI()
{
    m_view = new AnimatorLTView(this);
    setWidget(m_view);
}

void AnimatorLTDock::setCanvas(KoCanvasBase* canvas)
{
    AnimatorManager* manager = AnimatorManagerFactory::instance()->getManager(dynamic_cast<KisCanvas2*>(canvas));
    m_view->setLT(qobject_cast<AnimatorLTUpdater*>(manager->getUpdater())->getLT());
}

void AnimatorLTDock::unsetCanvas()
{
}
