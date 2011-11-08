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

#include "animator_importer.h"

#include <KoCanvasBase.h>
#include <kis_canvas2.h>
#include <kis_view2.h>
#include "normal_animated_layer.h"
#include "animator_manager_factory.h"

AnimatorImporter::AnimatorImporter(AnimatorManager *manager)
{
    m_manager = manager;
}

AnimatorImporter::~AnimatorImporter()
{
}

void AnimatorImporter::unsetCanvas()
{

}

void AnimatorImporter::setCanvas(KoCanvasBase *canvas)
{
    KisCanvas2 *kisCanvas = dynamic_cast<KisCanvas2*>(canvas);
    m_imageManager = kisCanvas->view()->imageManager();
}


void AnimatorImporter::importUser()
{
    KisNodeSP from;
    KisNodeSP to;
    
    from = m_manager->createLayerAt("KisPaintLayer", m_manager->image()->root(), 0);
    to = m_manager->createLayerAt("KisPaintLayer", m_manager->image()->root(), 1);
    m_manager->activate(-1, from.data());
    
    m_imageManager->importImage();
    importBetween(from, to);
}

void AnimatorImporter::importBetween(KisNodeSP from, KisNodeSP to)
{
    m_manager->activate(-1, to.data());
    m_manager->createAnimatedLayer<NormalAnimatedLayer>();
    FramedAnimatedLayer *layer = qobject_cast<FramedAnimatedLayer*>(m_manager->activeLayer());
    
    KisNodeSP node = from->nextSibling();
    int frameNumber = 0;
    while (node != to) {
        warnKrita << frameNumber << node;
        
        KisNodeSP nextNode = node->nextSibling();
        
        layer->createFrame(frameNumber, true, node);
        
        ++frameNumber;
        node = nextNode;
    }
    
    m_manager->removeLayer(from.data());
    m_manager->removeLayer(to.data());
}
