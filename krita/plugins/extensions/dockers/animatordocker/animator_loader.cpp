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

#include "animator_loader.h"

#include <kis_debug.h>
#include "framed_animated_layer.h"
#include "simple_frame_layer.h"

AnimatorLoader::AnimatorLoader(AnimatorManager* manager): QObject(manager)
{
    m_manager = manager;
}

void AnimatorLoader::loadAll()
{
    if (! m_manager->ready())
    {
        warnKrita << "trying to load layers before AnimatorManager is ready";
        return;
    }
    
    KisImage* image = m_manager->image();
    KisNodeSP rootNode = image->root();
    
    // first, check signature
    
    loadLayers(rootNode);
}

void AnimatorLoader::loadLayers(KisNodeSP rootNode)
{
    if (rootNode->inherits("AnimatedLayer"))
        return;
    
    if (! rootNode->inherits("KisGroupLayer"))
        return;
    
    if (rootNode->name().startsWith("_ani"))
    {
        loadLayer(rootNode);
        return;
    }
    
    KisNodeSP child = rootNode->firstChild();
    while (child)
    {
        loadLayers(child);
        child = child->nextSibling();
    }
}

void AnimatorLoader::loadLayer(KisNodeSP node)
{
    if (node->name().startsWith("_ani_"))
    {
        KisGroupLayer* gl = dynamic_cast<KisGroupLayer*>(node.data());
        FramedAnimatedLayer* al = new FramedAnimatedLayer(*gl);
        m_manager->moveFrames(al, gl);
        
        KisNodeSP child = al->firstChild();
        while (child)
        {
            if (child->inherits("KisGroupLayer") && child->name().startsWith("_frame_"))
            {
                SimpleFrameLayer* frame = new SimpleFrameLayer(* dynamic_cast<KisGroupLayer*>(child.data()));
                m_manager->setFrameContent(frame, child->at(0).data());
                m_manager->insertFrame(al, frame);
                m_manager->removeFrame(child.data());
            }
        }
        
        al->init();
    } else
    {
        warnKrita << "only normal framed layers are implemented";
    }
}
