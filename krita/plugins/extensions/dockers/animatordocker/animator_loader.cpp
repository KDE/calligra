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

#include "normal_animated_layer.h"
#include "simple_frame_layer.h"
#include "animator_meta_info.h"

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
    AnimatorMetaInfo* kra_info = m_manager->kraMetaInfo();
    AnimatorMetaInfo* plugin_info = m_manager->metaInfo();
    
    int kmajor = kra_info->getMajor();
    int pmajor = plugin_info->getMajor();
    
    if (kmajor < pmajor)
    {
        warnKrita << "importing from older formats is not supported yet";
        return;
    } else if (kmajor > pmajor)
    {
        warnKrita << "this file was created by newer version of plugin, please update it";
        return;
    }
    
    int kminor = kra_info->getMinor();
    int pminor = plugin_info->getMinor();
    
    if (kminor < pminor)
    {
        m_manager->setKraMetaInfo(plugin_info);
    } else if (kminor > pminor)
    {
        warnKrita << "this file can use some features of newer version of animator";
    }
    
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
    
//     KisNodeSP child = rootNode->firstChild();
//     while (child)
    for (int i = 0; i < rootNode->childCount(); ++i)
    {
        KisNodeSP child = rootNode->at(i);
        loadLayers(child);
//         child = child->nextSibling();
    }
}

void AnimatorLoader::loadLayer(KisNodeSP node)
{
    if (node->name().startsWith("_ani_"))
    {
        KisGroupLayer* gl = qobject_cast<KisGroupLayer*>(node.data());
        FramedAnimatedLayer* al = new NormalAnimatedLayer(*gl);
        
        KisNodeSP parent = gl->parent();
        m_manager->insertLayer(al, parent, parent->index(gl));
        
//         KisNodeSP child = al->firstChild();
//         while (child)
        int chcount = gl->childCount();
        for (int i = 0; i < chcount; ++i)
        {
            KisNodeSP child = gl->at(0);
            if (qobject_cast<KisGroupLayer*>(child.data()) && child->name().startsWith("_frame_"))
            {
                SimpleFrameLayer* frame = new SimpleFrameLayer(* qobject_cast<KisGroupLayer*>(child.data()));
                m_manager->insertFrame(frame, al);
                m_manager->setFrameContent(frame, child->at(0).data());
                m_manager->removeFrame(child.data());
//                 child = frame->nextSibling();
//             } else {
//                 child = child->nextSibling();
            }
        }
        
        al->init();
        
//         m_manager->insertLayer(al, m_manager->image()->root(), 0);
        m_manager->removeLayer(gl);
    } else
    {
        warnKrita << "only normal framed layers are implemented";
    }
}
