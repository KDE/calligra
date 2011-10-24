/*
 *  Control docker
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

#include "animator_control_dock.h"

#include <KLocale>

#include <QToolBar>

#include <kis_canvas2.h>

#include "animator_manager_factory.h"
#include "animator_loader.h"

AnimatorControlDock::AnimatorControlDock() : QDockWidget(i18n("Animator control"))
{
    setupUI();
}

void AnimatorControlDock::setCanvas(KoCanvasBase* canvas)
{
    KisCanvas2* kcanvas = dynamic_cast<KisCanvas2*>(canvas);
    KisImageSP image = kcanvas->image();
    m_manager = AnimatorManagerFactory::instance()->getManager(image.data(), kcanvas);
//     m_manager->getLoader()->loadAll();
}

void AnimatorControlDock::unsetCanvas()
{

}

void AnimatorControlDock::setupUI()
{
    QToolBar* tb = new QToolBar(this);
    tb->addAction(SmallIcon(""), i18n("Load layers"), this, SLOT(loadLayers()));
    setWidget(tb);
}

void AnimatorControlDock::loadLayers()
{
    m_manager->getLoader()->loadAll();
}
