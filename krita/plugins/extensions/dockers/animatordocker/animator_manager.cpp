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
#include "animator_lt_updater.h"
#include "animator_player.h"
#include "animator_exporter.h"

#include "animator_config.h"

#include "normal_animated_layer.h"
#include "control_animated_layer.h"
#include "view_animated_layer.h"

AnimatorManager::AnimatorManager(KisImage* image)
{
    m_loader = new AnimatorLoader(this);
    m_switcher = new AnimatorSwitcher(this);
    m_updater = new AnimatorLTUpdater(this);
    m_player = new AnimatorPlayer(this);
    m_exporter = new AnimatorExporter(this);
    
    m_image = image;
    m_nodeManager = 0;
    
    m_framesNumber = 0;
    
    m_info = new AnimatorMetaInfo(1, 3);
    
    connect(this, SIGNAL(layerFramesNumberChanged(AnimatedLayer*,int)), SLOT(framesNumberCheck(AnimatedLayer*,int)));
    
    connect(m_switcher, SIGNAL(frameChanged(int,int)), m_updater, SLOT(update(int,int)));
}

AnimatorManager::~AnimatorManager()
{
}

void AnimatorManager::setCanvas(KoCanvasBase* canvas)
{
    m_nodeManager = dynamic_cast<KisCanvas2*>(canvas)->view()->nodeManager();
    m_exporter->setCanvas(canvas);
#if LOAD_ON_START
    initLayers();
#endif
}

void AnimatorManager::unsetCanvas()
{
    m_nodeManager = 0;
}


bool AnimatorManager::ready()
{
    bool isReady = m_nodeManager;
    if (!isReady)
    {
        warnKrita << "animator manager is not ready";
    }
    return isReady;
}


AnimatorLoader* AnimatorManager::getLoader()
{
    return m_loader;
}

AnimatorSwitcher* AnimatorManager::getSwitcher()
{
    return m_switcher;
}

AnimatorUpdater* AnimatorManager::getUpdater()
{
    return m_updater;
}

AnimatorPlayer* AnimatorManager::getPlayer()
{
    return m_player;
}

AnimatorExporter* AnimatorManager::getExporter()
{
    return m_exporter;
}


KisImage* AnimatorManager::image()
{
    return m_image;
}


void AnimatorManager::setKraMetaInfo(AnimatorMetaInfo* info)
{
    KisNodeSP first = m_image->root()->lastChild();
    QString lname = "_animator_"+QString::number(info->getMajor())+"_"+QString::number(info->getMinor())+"_Please don't move this";
    if (kraMetaInfo()->getMajor() < 0)
    {
        m_nodeManager->createNode("KisGroupLayer");
        first = m_nodeManager->activeNode();
        m_nodeManager->moveNodeAt(first, m_image->root(), m_image->root()->childCount());
    }
    first->setName(lname);
}

void AnimatorManager::setKraMetaInfo()
{
    setKraMetaInfo(metaInfo());
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


void AnimatorManager::initLayers()
{
    getLoader()->loadAll();
    getUpdater()->fullUpdate();
    getSwitcher()->goFrame(0);
}


void AnimatorManager::setFrameContent(SimpleFrameLayer* frame, KisNode* content)
{
    if (!ready())
        return;
    
    if (!content)
        return;
    
    if (frame->getContent())
        m_nodeManager->removeNode(frame->getContent());
    
    putNodeAt(content, frame, 0);
    if (! content->name().startsWith("_"))
        content->setName("_");
}

void AnimatorManager::putNodeAt(KisNodeSP node, KisNodeSP parent, int index)
{
    if (node->parent())
        m_nodeManager->moveNodeAt(node, parent, index);
    else
        m_nodeManager->insertNode(node, parent, index);
}

void AnimatorManager::removeFrame(KisNode* frame)
{
    m_nodeManager->removeNode(frame);
}

void AnimatorManager::insertLayer(AnimatedLayer* layer, KisNodeSP parent, int index)
{
    putNodeAt(layer, parent, index);
    layerAdded(layer);
}

void AnimatorManager::removeLayer(KisNode* layer)
{
    if (!layer)
        return;
    m_nodeManager->removeNode(layer);
    layerRemoved(dynamic_cast<AnimatedLayer*>(layer));
}

void AnimatorManager::createGroupLayer(KisNodeSP parent)
{
    m_nodeManager->activateNode(parent);
    m_nodeManager->createNode("KisGroupLayer");
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
    if (number == m_framesNumber)
        return;
    
    if (number > m_framesNumber)
    {
        m_maxFrameLayer = layer;
        m_framesNumber = number;
        emit framesNumberChanged(m_framesNumber);
        return;
    }
    
    if (layer == m_maxFrameLayer)
    {
        // We do not know layer with max frames now
        calculateFramesNumber();
    }
}

void AnimatorManager::calculateFramesNumber()
{
    m_framesNumber = 0;
    AnimatedLayer* layer;
    foreach (layer, m_layers)
    {
        if (layer->dataEnd() > m_framesNumber)
        {
            m_framesNumber = layer->dataEnd();
            m_maxFrameLayer = layer;
        }
    }
    emit framesNumberChanged(m_framesNumber);
}


void AnimatorManager::activate(int frameNumber, KisNode* node)
{
    if (frameNumber >= 0)
        getSwitcher()->goFrame(frameNumber);
    
    if (node)
    {
        SimpleFrameLayer* frame = qobject_cast<SimpleFrameLayer*>(node);
        if (frame)
            node = frame->getContent();
        m_nodeManager->activateNode(node);
    }
    
    AnimatedLayer* alayer = qobject_cast<AnimatedLayer*>(activeLayer());
    emit animatedLayerActivated(alayer);
}

void AnimatorManager::activateKeyFrame(AnimatedLayer* alayer, int frameNumber)
{
    FramedAnimatedLayer* flayer = qobject_cast<FramedAnimatedLayer*>(alayer);
    if (!flayer)
        return;
    activate(frameNumber, flayer->frameAt(flayer->getPreviousKey(frameNumber)));
}


void AnimatorManager::layerAdded(AnimatedLayer* layer)
{
    if (!layer)
        return;
    m_layers.append(layer);
    
    framesNumberCheck(layer, layer->dataEnd());
}

void AnimatorManager::layerRemoved(AnimatedLayer* layer)
{
    if (!layer)
        return;
    int i = m_layers.indexOf(layer);
    if (i >= 0)
        m_layers.removeAt(i);
    
    if (layer == m_maxFrameLayer)
        calculateFramesNumber();
}

QList< AnimatedLayer* > AnimatorManager::layers()
{
    return m_layers;
}


AnimatedLayer* AnimatorManager::getAnimatedLayerByChild(KisNode* child)
{
    KisNode* node = child;
    bool found = false;
    while (node->parent() && !found)
    {
        if (node->inherits("AnimatedLayer"))
            found = true;
        else
            node = node->parent().data();
    }
    return qobject_cast<AnimatedLayer*>(node);
}

KisNode* AnimatorManager::activeLayer()
{
    KisNode* node = m_nodeManager->activeNode().data();
    AnimatedLayer* l = getAnimatedLayerByChild(node);
    if (l)
        return l;
    return node;
}

template <class CustomAnimatedLayer>
void AnimatorManager::createAnimatedLayer()
{
    CustomAnimatedLayer* newLayer = new CustomAnimatedLayer(image(), "", 255);
    newLayer->setAName("New layer");
    
    KisNode* activeNode = activeLayer();
    AnimatedLayer* alayer = qobject_cast<AnimatedLayer*>(activeNode); //getAnimatedLayerByChild(activeNode);
    if (alayer)
    {
        KisNode* parent = alayer->parent().data();
        m_nodeManager->insertNode(newLayer, parent, parent->index(alayer));
    } else
    {
        m_nodeManager->insertNode(newLayer, image()->root(), 0);
        m_nodeManager->moveNode(newLayer, activeNode);
    }
    
    layerAdded(newLayer);
    m_nodeManager->activateNode(newLayer);
}

void AnimatorManager::createNormalLayer()
{
    createAnimatedLayer<NormalAnimatedLayer>();
}

void AnimatorManager::createControlLayer()
{
    createAnimatedLayer<ControlAnimatedLayer>();
}

void AnimatorManager::convertToViewLayer(int from, int to)
{
    KisNode* activeNode = activeLayer();
    if (qobject_cast<AnimatedLayer*>(activeNode))
        return;
    
    createAnimatedLayer<ViewAnimatedLayer>();
    ViewAnimatedLayer* vlayer = qobject_cast<ViewAnimatedLayer*>(activeLayer());
    if (!vlayer)
        return;
    putNodeAt(activeNode, vlayer, 0);
    
    vlayer->setStart(from);
    vlayer->setEnd(to);
    vlayer->save();
}


void AnimatorManager::removeLayer()
{
    AnimatedLayer* alayer = getAnimatedLayerByChild(m_nodeManager->activeNode().data());
    removeLayer(alayer);
}

void AnimatorManager::renameLayer(KisNode* layer, const QString& name)
{
    AnimatedLayer* alayer = qobject_cast<AnimatedLayer*>(layer);
    if (alayer)
        alayer->setAName(name);
    else if (layer)
        layer->setName(name);
}

void AnimatorManager::renameLayer(const QString& name)
{
    renameLayer(activeLayer(), name);
}

void AnimatorManager::createFrame(AnimatedLayer* layer, const QString& ftype, bool iskey)
{
    FramedAnimatedLayer* alayer = qobject_cast<FramedAnimatedLayer*>(layer);
    if (!alayer)
    {
        warnKrita << "Could not determine animated layer or frame adding is not supported, no frame created";
        return;
    }
    
    int frameNumber = getSwitcher()->currentFrame();
    if (alayer->frameAt(frameNumber))
    {
        warnKrita << "Have frame already, no frame created";
        return;
    }
    
    SimpleFrameLayer* frame = new SimpleFrameLayer(image(), alayer->getNameForFrame(frameNumber, iskey), 255);
    alayer->insertFrame(frame);
    if (ftype != "")
    {
        m_nodeManager->createNode(ftype);
        frame->setContent(m_nodeManager->activeNode().data());
    }
    
    // TODO: don't do full update
    getUpdater()->fullUpdateLayer(alayer);
    getUpdater()->updateLayer(alayer, frameNumber, frameNumber);
}

void AnimatorManager::createFrame(AnimatedLayer* layer, const QString& ftype)
{
    createFrame(layer, ftype, true);
}

void AnimatorManager::createFrame(const QString& ftype, bool iskey)
{
    AnimatedLayer* alayer = qobject_cast<AnimatedLayer*>(activeLayer());
    createFrame(alayer, ftype, iskey);
}

void AnimatorManager::createFrame(const QString& ftype)
{
    createFrame(ftype, true);
}

void AnimatorManager::interpolate()
{
    FramedAnimatedLayer* alayer = qobject_cast<FramedAnimatedLayer*>(activeLayer());
    if (!alayer)
        return;
    int fnum = m_switcher->currentFrame();
    activateKeyFrame(alayer, fnum);
    m_nodeManager->createNode("KisCloneLayer");
    KisNode* content = m_nodeManager->activeNode().data();
    createFrame(alayer, "");
    SimpleFrameLayer* frame = qobject_cast<SimpleFrameLayer*>(alayer->frameAt(fnum));
    if (!frame)
        return;
    setFrameContent(frame, content);
    getUpdater()->updateLayer(alayer, fnum, fnum);
}


void AnimatorManager::createLoopFrame(int target, int repeat)
{
    ControlAnimatedLayer* clayer = qobject_cast<ControlAnimatedLayer*>(activeLayer());
    if (!clayer)
        return;
    clayer->setLoop(target, m_switcher->currentFrame(), repeat);
}


void AnimatorManager::removeFrame()
{
    FramedAnimatedLayer* alayer = qobject_cast<FramedAnimatedLayer*>(activeLayer());
    if (!alayer)
    {
        warnKrita << "Could not determine active animated layer or frame removing is not supported, no frame removed";
        return;
    }
    
    int frameNumber = getSwitcher()->currentFrame();
    if (! alayer->frameAt(frameNumber))
    {
        warnKrita << "No frame, nothing to remove";
        return;
    }
    
    alayer->removeFrameAt(frameNumber);
    
    activate(frameNumber, alayer);
    
    getUpdater()->updateLayer(alayer, frameNumber, frameNumber);
}

void AnimatorManager::moveFrame(int relPos)
{
    FramedAnimatedLayer* alayer = qobject_cast<FramedAnimatedLayer*>(activeLayer());
    if (!alayer)
        return;
    int fnum = getSwitcher()->currentFrame();
    FrameLayer* tFrame = alayer->frameAt(fnum);
    if (!tFrame)
        return;
    bool isKey;
    alayer->getFrameFromName(tFrame->name(), isKey);
    tFrame->setName(alayer->getNameForFrame(fnum+relPos, isKey));
    alayer->insertFrame(tFrame);
}
