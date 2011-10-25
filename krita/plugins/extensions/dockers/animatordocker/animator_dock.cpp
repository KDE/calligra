/*
 *  
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

#include <KoCanvasBase.h>

#include <kis_canvas2.h>
#include <kis_doc2.h>
#include <kis_image.h>
#include <kis_node_manager.h>
#include <kis_view2.h>

#include "animator_dock.h"
#include "animator_model.h"
#include "animator_manager_factory.h"
#include "animator_view.h"

AnimatorDock::AnimatorDock( ) : QDockWidget(i18n("Animator"))
{
    m_mainModel = 0;
    
    setupUI();
}

void AnimatorDock::setupUI()
{
    m_view = new AnimatorView;
//     m_view->setModel(m_mainModel);
    
    setWidget(m_view);
}

void AnimatorDock::setCanvas(KoCanvasBase* canvas)
{
    KisCanvas2* kcanvas = dynamic_cast<KisCanvas2*>(canvas);
    KisImage* image = kcanvas->image().data();
    
    m_mainModel = new AnimatorModel(image);
    m_mainModel->setFrameWidth(10);             // TODO: make this configurable
    
    m_view->setModel(m_mainModel);
    
    m_manager = AnimatorManagerFactory::instance()->getManager(image, kcanvas);
}

void AnimatorDock::unsetCanvas()
{
    delete m_mainModel;
    m_mainModel = 0;
}

#include "animator_dock.moc"
