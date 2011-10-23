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

#include "animator_manager.h"

#include <kis_canvas2.h>
#include <kis_view2.h>

#include "animator_loader.h"
#include "animator_switcher.h"

#include <kis_debug.h>

AnimatorManager::AnimatorManager(KisImage* image)
{
    m_loader = new AnimatorLoader(this);
    m_switcher = new AnimatorSwitcher(this);
    
    m_image = image;
    m_nodeManager = 0;
    
    m_framesNumber = 0;
    
    m_info = new AnimatorMetaInfo(1, 1);
    
    connect(this, SIGNAL(layerFramesNumberChanged(AnimatedLayer*,int)), SLOT(framesNumberCheck(AnimatedLayer*,int)));
}

AnimatorManager::~AnimatorManager()
{
}

void AnimatorManager::setCanvas(KoCanvasBase* canvas)
{
    m_nodeManager = dynamic_cast<KisCanvas2*>(canvas)->view()->nodeManager();
}

void AnimatorManager::unsetCanvas()
{
    m_nodeManager = 0;
}


bool AnimatorManager::ready()
{
    return m_nodeManager != 0;
}


AnimatorLoader* AnimatorManager::getLoader()
{
    return m_loader;
}

AnimatorSwitcher* AnimatorManager::getSwitcher()
{
    return m_switcher;
}


KisImage* AnimatorManager::image()
{
    return m_image;
}

AnimatorMetaInfo* AnimatorManager::kraMetaInfo()
{
    KisNodeSP first = m_image->root()->lastChild();
    return new AnimatorMetaInfo(dynamic_cast<KisGroupLayer*>(first.data()));
}

AnimatorMetaInfo* AnimatorManager::metaInfo()
{
    return m_info;
}


void AnimatorManager::setFrameContent(SimpleFrameLayer* frame, KisNode* content)
{
    if (content->parent())
        m_nodeManager->moveNodeAt(content, frame, 0);
    else
        m_nodeManager->insertNode(content, frame, 0);
    if (! content->name().startsWith("_"))
        content->setName("_");
}

void AnimatorManager::insertFrame(SimpleFrameLayer* frame, FramedAnimatedLayer* layer)
{
    m_nodeManager->insertNode(frame, layer, 0);
}

void AnimatorManager::removeFrame(KisNode* frame)
{
    m_nodeManager->removeNode(frame);
}

void AnimatorManager::moveFrames(KisGroupLayerSP to, KisGroupLayerSP from)
{
    KisNodeSP child = from->firstChild();
    int i = 0;
    while (child)
    {
        m_nodeManager->moveNodeAt(child, to, i++);
        child = from->firstChild();
    }
}

void AnimatorManager::insertLayer(AnimatedLayer* layer, KisNodeSP parent, int index)
{
    m_nodeManager->insertNode(layer, parent, index);
    m_nodeManager->moveNodeAt(layer, parent, index);
}

void AnimatorManager::removeLayer(KisNode* layer)
{
    m_nodeManager->removeNode(layer);
}


int AnimatorManager::framesNumber() const
{
    return m_framesNumber;
}


void AnimatorManager::layerFramesNumberChange(AnimatedLayer* layer, int number)
{
    emit layerFramesNumberChanged(layer, number);
}

void AnimatorManager::framesNumberCheck(AnimatedLayer* layer, int number)
{
    // FIXME: now frames number can only grow
    if (number > m_framesNumber)
        m_framesNumber = number;
}
