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
#include "control_animated_layer.h"
#include "control_frame_layer.h"
#include "view_animated_layer.h"

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
    
    bool needLoading = true;
    
    if (kmajor < pmajor)
    {
        warnKrita << "trying to import from older format";
        needLoading = loadLegacy(kmajor);
        if (needLoading)
            m_manager->setKraMetaInfo();
    } else if (kmajor > pmajor)
    {
        warnKrita << "this file was created by newer version of plugin, please update it";
        return;
    }
    
    if (!needLoading)
        return;
    
    int kminor = kra_info->getMinor();
    int pminor = plugin_info->getMinor();
    
    if (kminor < pminor)
    {
        m_manager->setKraMetaInfo();
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
    
    for (int i = 0; i < rootNode->childCount(); ++i)
    {
        KisNodeSP child = rootNode->at(i);
        loadLayers(child);
    }
}

void AnimatorLoader::loadLayer(KisNodeSP node)
{
    if (node->name().startsWith("_ani_"))
    {
        loadFramedLayer<NormalAnimatedLayer, SimpleFrameLayer>(node);
    } else if (node->name().startsWith("_anicontrol_"))
    {
        ControlAnimatedLayer* clayer = loadFramedLayer<ControlAnimatedLayer, ControlFrameLayer>(node);
        clayer->reset();
    } else if (node->name().startsWith("_aniview_"))
    {
        KisGroupLayer* gl = qobject_cast<KisGroupLayer*>(node.data());
        if (!gl || !gl->at(0))
            return;
        ViewAnimatedLayer* vlayer = new ViewAnimatedLayer(*gl);
        KisNodeSP parent = gl->parent();
        m_manager->insertLayer(vlayer, parent, parent->index(gl));
        m_manager->putNodeAt(gl->at(0), vlayer, 0);
        m_manager->removeLayer(gl);
        vlayer->load();
    } else
    {
        warnKrita << "only normal framed and control layers are implemented";
    }
}

template <class CustomAnimatedLayer, class CustomFrameLayer>
CustomAnimatedLayer* AnimatorLoader::loadFramedLayer(KisNodeSP node)
{
    KisGroupLayer* gl = qobject_cast<KisGroupLayer*>(node.data());
    FramedAnimatedLayer* al = new CustomAnimatedLayer(*gl);

    KisNodeSP parent = gl->parent();
    m_manager->insertLayer(al, parent, parent->index(gl));

    int chcount = gl->childCount();
    for (int i = 0; i < chcount; ++i)
    {
        KisNodeSP child = gl->at(0);
        if (qobject_cast<KisGroupLayer*>(child.data()) && child->name().startsWith("_frame_"))
        {
            SimpleFrameLayer* frame = new CustomFrameLayer(* qobject_cast<KisGroupLayer*>(child.data()));
            al->insertFrame(frame);
            frame->setContent(child->at(0).data());
            m_manager->removeNode(child);
        }
    }

    m_manager->removeLayer(gl);
    
    return qobject_cast<CustomAnimatedLayer*>(al);
}

// Compatibility code
bool AnimatorLoader::loadLegacy(int majorv)
{
    if (majorv == -1)
    {
        return convertAll();
    }
    return true;
}

bool AnimatorLoader::convertAll()
{
    return convertLayers(m_manager->image()->root());
}

bool AnimatorLoader::convertLayers(KisNodeSP rootNode)
{
    bool isAnimated = false;
    for (int i = 0; i < rootNode->childCount(); ++i)
    {
        KisNodeSP node = rootNode->at(i);
        if (node->name().startsWith("_ani_"))
        {
            isAnimated = true;
            convertLayer(node);
        }
    }
    return isAnimated;
}

void AnimatorLoader::convertLayer(KisNodeSP node)
{
    for (int i = 0; i < node->childCount(); ++i)
    {
        KisNode* fr = node->at(i).data();
        fr->setVisible(true);
        fr->setOpacity(255);
        
        m_manager->createGroupLayer(node);
        KisGroupLayer* nf = dynamic_cast<KisGroupLayer*>(m_manager->activeLayer());
        
        m_manager->putNodeAt(fr, nf, 0);                       // This is VERY slow
        
        nf->setName(fr->name());
        fr->setName("_");
    }
}
