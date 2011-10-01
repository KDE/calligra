/*
 *  Model
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

#include <QVariant>

#include <KoProperties.h>

#include "kis_layer.h"
#include "kis_group_layer.h"
#include "kis_paint_layer.h"
#include "kis_base_node.h"
#include "kis_image.h"

#include "animator_model.h"

#include <sstream>

#include <iostream>

#include "normal_animated_layer.h"

AnimatorModel::AnimatorModel(QObject* parent): QAbstractTableModel(parent)
{
    init();
    setLightTable(0);
    connect(this, SIGNAL(frameChanged(int)), SLOT(updateImage()));
}

void AnimatorModel::init()
{
    m_visible = false;
    
    this->blockSignals(true);
    goFrame(0);
    
    setOnionNext(0);
    setOnionPrevious(0);
    
    setOnionOpacity(127);
    
    enableOnion();                      // Onion should be only disabled while exporting/playing
    
    m_ext_lighttable = false;
    
//     setLightTable(0);
    
    m_layers.clear();
    
    forceFramesNumber(12);
    
    this->blockSignals(false);
    
//     connect(this, SIGNAL(frameChanged(int)), SLOT(updateImage()));
}

AnimatorModel::~AnimatorModel()
{
//     delete m_source;
}

void AnimatorModel::loadLayers()
{
//     blockSignals(true);
    
//     return;
    
    if (! m_enabled)
        return;
    
    if (m_updating)
        return;
    
    m_updating = true;
    
    KisNodeModel* nodes = sourceModel();
    
    m_layers.clear();
    
    if (! nodes || nodes->rowCount() == 0 )
        return;
    
    // First, we need to check if we've get animated document
    KisNode* meta_layer = nodes->nodeFromIndex(nodes->index(0, 0)).data();
    int major = getMajor(meta_layer);
    if (major == -1)
        return;                         // Not even an animated image of new format
    
    if (major != getMajor())
        return;                         // Major versions are not compataible
    
    int minor = getMinor(meta_layer);
//     if (minor > getMinor());
//         this means that some features may be bad represented and probably in some cases data could be lost.
    
    if (minor < getMinor())
        minor = getMinor();
    
    // Now write info
    setVersion(major, minor);
    
    int fnum = 0;
    
    for (quint32 i = 1; i < nodes->rowCount(); ++i)
    {
        QModelIndex index = sourceModel()->index(i, 0);

        const KisNode* cnode = sourceModel()->nodeFromIndex(index);
        KisNode* node = const_cast<KisNode*>( cnode );
        
        AnimatedLayer* lay = 0;
        
        if (node->inherits("AnimatedLayer")) // && dynamic_cast<AnimatedLayer*>(node)->isValid())
        {
            lay = dynamic_cast<AnimatedLayer*>(node);
        } else if (node->inherits("KisGroupLayer") && node->name().startsWith("_ani"))
        {
            if (node->name().startsWith("_ani_"))       // Normal
            {
                KisGroupLayer* gnode = dynamic_cast<KisGroupLayer*>(node);
//                 lay = new SimpleAnimatedLayer(*gnode);
                lay = new NormalAnimatedLayer(*gnode);
                
                lay->setName(gnode->name());
                
                // CHECK
                lay->setNodeManager(m_nodeman);
                
                const KisNode* pn = gnode->parent();
                int nindex = pn->index(gnode);
//                 m_nodeman->activateNode();
                
                
                m_nodeman->removeNode(gnode);
                m_nodeman->insertNode(lay, const_cast<KisNode*>( pn ), nindex);
                
                // TODO: move to AnimatedLayer constructor
                for (int i = 0; i < lay->childCount(); ++i)
                {
                    const KisNode* nnode = lay->at(i);
                    KisNode* ncnode = const_cast<KisNode*>(nnode);
                    QString tmp = ncnode->name();
                    ncnode->setName(tmp.mid(14, tmp.size()-15));        // This means: delete "Duplicate of " from name
                }
            }
        }
        
        if (lay)
        {
//             while (!lay->loaded())
            lay->loadFrames();
            
            m_layers.append(lay);
            
            if (lay->dataEnd() > fnum)
            {
                fnum = lay->dataEnd();
            }
            
            connect(lay, SIGNAL(destroyed(QObject*)), this, SLOT(nodeDestroyed(QObject*)));
        }
    }
    
    setFramesNumber(fnum);
    
    
    // CHECK if this is necessary
    emit frameChanged(m_frame);
    emit framesNumberChanged(m_frame_num);
    
    emit layoutChanged();
    
    m_updating = false;
}

void AnimatorModel::convertLayers()
{
//     AnimatedLayer* lay;
//     foreach (lay, m_layers)
//     {
//         if (lay->inherits("SimpleAnimatedLayer"))
//             dynamic_cast<SimpleAnimatedLayer*>(lay)->convertFrames();
//     }
    KisNodeModel* nodes = sourceModel();

    for (int i = 0; i < nodes->rowCount(); ++i)
    {
        KisNode* node = nodes->nodeFromIndex(nodes->index(i, 0)).data();
        if (node->name().startsWith("_ani_"))
        {
            convertLayer(node);
        }
    }
}

void AnimatorModel::convertLayer(KisNode* node)
{
    KisNode* fr;
    for (int i = 0; i < node->childCount(); ++i)
    {
        fr = node->at(i).data();
        
    }
}

// void AnimatorModel::setFramesNumber(int num)
// {
//     m_frame_num = num;
// }

int AnimatorModel::previousFrame()
{
    return m_previous_frame;
}

void AnimatorModel::setSourceModel(KisNodeModel* model)
{
    m_source = model;
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(loadLayers()));
}

KisNodeModel* AnimatorModel::sourceModel() const
{
    return m_source;
}

void AnimatorModel::setLightTable(AnimatorLightTable* table)
{
    m_light_table = table;
    connect(m_light_table, SIGNAL(nearChanged(int)), this, SLOT(lightTableUpdate()));
    connect(m_light_table, SIGNAL(visibilityChanged()), this, SLOT(lightTableUpdate()));
    connect(m_light_table, SIGNAL(opacityChanged()), this, SLOT(lightTableUpdate()));
}

void AnimatorModel::forceFramesNumber(int num)
{
    m_forced_frame_num = num;
    if (m_frame_num < m_forced_frame_num)
        setFramesNumber(num);
    
//     emit 
//     loadLayers();
}

QVariant AnimatorModel::data(const QModelIndex& index, int role) const
{
    QVariant result = QVariant();
    FrameLayer* fl = getCachedFrame(index);
    if (fl && isKey(index))
        result = fl->getVision(role, m_frame == index.column());
    
    if (result == QVariant())
        return getAnimatedLayer(index.row())->getVision(role, m_frame == index.column());
    
    return result;
}

QVariant AnimatorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant t;
    
    return t;
    
    if (orientation == Qt::Vertical) {
        if (role == Qt::DisplayRole)
        {
//             return QVariant( QString("ddd") );
            AnimatedLayer* n = getAnimatedLayer(section);
            
            if (! n)
            {
                return t;
            }
            
            
            QString ts = n->name();
            
            t = ts.mid(5);
        }
    } else      // orientation == Qt::Horizontal
    {
        return 0;
        if (role == Qt::DisplayRole)
        {
            if (section % 6 == 0)
            {
                return QString::number(section);
            }
        } else if (role == Qt::BackgroundRole)
        {
            // This is probably a slowness factor
//             if (section == m_frame)
//             {
//                 return QBrush(QColor(10, 10, 10, 127));
//             }
        }
    }
    
    return t;
}

// void AnimatorModel::testSlot()
// {
//     std::cout << "headerDataChanged()" << std::endl;
// }


int AnimatorModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_frame_num;
}

int AnimatorModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_layers.size();
}

void AnimatorModel::setNodeManager(KisNodeManager* nodeman)
{
    m_nodeman = nodeman;
}

void AnimatorModel::setFramesNumber(int frames)
{
    setFramesNumber(frames, true);
}

void AnimatorModel::setFramesNumber(int frames, bool up)
{
    if (m_frame_num == frames)
        return;
    m_frame_num = frames;
    if (up)
    {
        emit layoutChanged();
    }
    emit framesNumberChanged(frames);
}


void AnimatorModel::updateImage()
{
    if (!m_visible)
        return;
    
//     if (m_updating)
//         return;
    
//     m_updating = true;
    
    for (qint32 i = 0; i < rowCount(); ++i)
    {
        KisNode* frame = getAnimatedLayer(i)->getUpdatedFrame(m_frame);
        KisNode* old_frame = getAnimatedLayer(i)->getCachedFrame(m_previous_frame);
        
        if (frame == old_frame)
            continue;
        
        if (old_frame)
        {
            old_frame->setVisible(false);
            m_image->updateProjection(old_frame, old_frame->exactBounds());
        }
        if (frame)
        {
            frame->setOpacity(255);
            frame->setVisible(true);
            m_image->updateProjection(frame, /*m_image->bounds()*/ /*old_frame?*/ frame->exactBounds() /*| old_frame->exactBounds() : frame->exactBounds()*/);
        }
        
        if (m_onion_en && !m_ext_lighttable && 0)
        {
            // Onion support --> move to animator_light_table
            int nxt = m_onion_nxt;
            quint8 opacity = m_onion_opacity;
            
    //         int b = ;
            int fr = m_frame;
            while (nxt)
            {
                KisNode* node = const_cast<KisNode*>( nextFrame(createIndex(i, fr)) );
                if (!node)
                    break;
                
                node->setOpacity(opacity);
                node->setVisible(true);
                
                m_image->updateProjection(node, m_image->bounds());
                
                fr = indexFromNode(node).column()+1;
                opacity *= m_onion_opacity;
                opacity /= 255;
                nxt--;
            }
            
            
            int prv = m_onion_prv;
            opacity = m_onion_opacity;
            
            fr = m_frame;
            while (prv)
            {
                KisNode* node = const_cast<KisNode*>( previousFrame(createIndex(i, fr)) );
                if (!node)
                    break;
                
                node->setOpacity(opacity);
                node->setVisible(true);
                
                m_image->updateProjection(node, m_image->bounds());
                
                fr = indexFromNode(node).column()-1;
                opacity *= m_onion_opacity;
                opacity /= 255;
                prv--;
            }
        }
    }
    
    // ENABLE THIS
    lightTableUpdate();
    
    if (m_nodeman)
    {
//         m_nodeman->nodesUpdated();
// //         m_nodeman->updateGUI();
    }

//     emit headerDataChanged(Qt::Horizontal, 0, columnCount()-1);
    
//     if (rec)
//     updateCanvas();
    
//     m_updating = false;
}

void AnimatorModel::lightTableUpdate()
{
    if (!m_visible)
        return;
    
    if (!m_ext_lighttable || !m_onion_en)
        return;
    
    for (int i = 0; i < rowCount(); ++i)
    {
        for (int fnum = -m_light_table->getNear(); fnum <= m_light_table->getNear(); ++fnum)
        {
            int old_f = previousFrame();
            
            // Clear old
            KisNode* ofnode = const_cast<KisNode*>( nodeFromIndex(createIndex(i, old_f+fnum)) );
            if (ofnode && isKey(i, old_f+fnum))
            {
                ofnode->setVisible(false);
                m_image->updateProjection(ofnode, ofnode->exactBounds());
            }
        }
        
        for (int fnum = -m_light_table->getNear(); fnum <= m_light_table->getNear(); ++fnum)
        {
//          // Show new
            KisNode* fnode = const_cast<KisNode*>( nodeFromIndex(createIndex(i, m_frame+fnum)) );
            if (fnode && isKey(i, m_frame+fnum))
            {
                double dop = m_light_table->getOpacity(fnum);
                quint8 op = dop*255.0;
//                 std::cout << dop << " " << (int)op << std::endl;
                fnode->setOpacity( op );
                fnode->setVisible(m_light_table->getVisibility(fnum));
                m_image->updateProjection(fnode, fnode->exactBounds());
            }
        }
    }
    
//     updateCanvas();
}

void AnimatorModel::setCanvas(KisCanvas2* canvas)
{
    m_canvas = canvas;
}

void AnimatorModel::setImage(KisImageWSP image)
{
    const KisImage* timg = image;
//     KisImage* img = 
    
    if (timg != m_image)
    {
        init();
    }
    
    m_image = const_cast<KisImage*>( timg );
}


void AnimatorModel::activateLayer(const QModelIndex& index)
{
    goFrame(index.column());
    
    if (nodeFromIndex(index))
        m_nodeman->activateNode( dynamic_cast<FrameLayer*>( const_cast<KisNode*>( nodeFromIndex(index) ) )->getContent() );
    
    // DO NOT DO THIS!!!
        
//     emit dataChanged(createIndex(0, 0), createIndex(rowCount(), columnCount()));
//     emit headerDataChanged(Qt::Horizontal, 0, columnCount());
}

//void AnimatorModel::updateCanvas()
//{
//    if (!m_image || !m_canvas)
//        return;
//    
//    m_canvas->startUpdateCanvasProjection(m_image->bounds());
//}

const KisNode* AnimatorModel::nodeFromIndex(const QModelIndex& index) const
{
    return dynamic_cast<KisNode*>(getCachedFrame(index));
//     if (rowCount() > index.row())
//     {
//         return m_layers[index.row()]->getCachedFrame(index.column());
//     }
//     return 0;
}

const KisNode* AnimatorModel::nextFrame(const QModelIndex& index) const
{
    return getAnimatedLayer(index.row())->getNextKeyFrame(index.column());
}

//const KisNode* AnimatorModel::getCachedFrame(const QModelIndex& index) const
//{
//    return m_layers[index.row()]->getKeyKisNode(index.column());
//}

bool AnimatorModel::activateAtIndex(QModelIndex index)
{
    if (isKey(index))
    {
        return false;
    }
    
    KisNode* node = dynamic_cast<KisNode*>(getCachedFrame(index.row(), index.column()));
    if (node)
    {
        m_nodeman->activateNode(node);
    } else
    {
        m_nodeman->activateNode(getAnimatedLayer(index.row()));
    }
    
    return true;
}

bool AnimatorModel::activateBeforeIndex(QModelIndex index)
{
    if (isKey(index))
    {
        return false;
    }
    
    KisNode* node = dynamic_cast<KisNode*>(getCachedFrame(index));
    if (node)
    {
        m_nodeman->activateNode(node);
    } else
    {
        // CHECK
        m_nodeman->activateNode(getAnimatedLayer(index.row()));
        return false;
    }
    
    return true;
}

bool AnimatorModel::activateAfterIndex(QModelIndex index)
{
    if (isKey(index))
        return false;
    
    KisNode* node = const_cast<KisNode*>( nextFrame(index) );
    
    if (node)
    {
        m_nodeman->activateNode(node);
    } else
    {
        m_nodeman->activateNode(getAnimatedLayer(index.row()));
        return false;
    }
    return true;
}

void AnimatorModel::createFrame(QModelIndex index, char* layer_type)
{
    if (activateAtIndex(index))
    {
        if (m_nodeman->activeNode()->inherits("KisGroupLayer"))
        {
            m_nodeman->activateNode(getAnimatedLayer(index.row()));
        }
        
        m_updating = true;
        
        m_nodeman->createNode("KisGroupLayer");
        m_nodeman->activeNode()->setName(getAnimatedLayer(index.row())->getNameForFrame(index.column(), true));
        
        m_nodeman->createNode(layer_type);
        m_nodeman->activeNode()->setName("_");
        
        m_updating = false;
        
        loadLayers();
        updateImage();
    } else
    {
        std::cout << "Has node already: may be clear instead?" << std::endl;
    }
}

void AnimatorModel::copyFramePrevious(QModelIndex index)
{
    if (activateBeforeIndex(index))
    {
        m_nodeman->duplicateActiveNode();
        
        
        KisNodeSP node;
        foreach (node, m_layers[index.row()]->childNodes(QStringList(), KoProperties()))
        {
            if (node->name().startsWith("Duplicate"))
            {
//                 KisNode* nn = const_cast<KisNode*>(node.data());
                m_nodeman->activateNode(node);
            }
        }
        
        m_nodeman->activeNode()->setName(getAnimatedLayer(index.row())->getNameForFrame(index.column(), true));
        
        loadLayers();
        updateImage();
    } else
    {
        std::cout << "Has node already or no frames: can't copy" << std::endl;
    }
}

void AnimatorModel::copyFrameNext(QModelIndex index)
{
    if (activateAfterIndex(index))
    {
        m_nodeman->duplicateActiveNode();               // Should activate new layer, but it doesn't
        
        KisNodeSP node;
        foreach (node, m_layers[index.row()]->childNodes(QStringList(), KoProperties()))
        {
            if (node->name().startsWith("Duplicate"))
            {
//                 KisNode* nn = const_cast<KisNode*>(node.data());
                m_nodeman->activateNode(node);
            }
        }
        
        m_nodeman->activeNode()->setName(getAnimatedLayer(index.row())->getNameForFrame(index.column(), true));
        
        loadLayers();
        updateImage();
    } else
    {
        std::cout << "Has node already or no frames: can't copy" << std::endl;
    }
}

void AnimatorModel::clonePrevious(QModelIndex index)
{
    if (activateBeforeIndex(index))
    {
        m_nodeman->createNode("KisCloneLayer");
	m_nodeman->activeNode()->setName(getAnimatedLayer(index.row())->getNameForFrame(index.column(), true));
        
        loadLayers();
        updateImage();
    }
}

void AnimatorModel::clearFrames(QModelIndexList frames)
{
    foreach(QModelIndex frame, frames)
    {
        clearFrame(frame);
    }
}

void AnimatorModel::clearFrame(QModelIndex index)
{
    KisNode* node = dynamic_cast<KisNode*>( getCachedFrame(index) );
    if (node)
    {
        m_nodeman->removeNode(node);
        loadLayers();
        updateImage();
    } else
    {
        std::cout << "Already clear" << std::endl;
        return;
    }
}

void AnimatorModel::nodeDestroyed(QObject* node)
{
    for (int i = 0; i < rowCount(); ++i) {
        AnimatedLayer* layer = m_layers[i];
        if (layer == node)
        {
            m_layers[i] = 0;
        }
    }
}

AnimatedLayer* AnimatorModel::getActiveAnimatedLayer() const
{
    return getAnimatedLayerByChild(dynamic_cast<KisNode*>( m_nodeman->activeNode().data() ));
}

QModelIndex AnimatorModel::createLayer()
{
    int pos = 0;
    
    KisNode* current = getActiveAnimatedLayer();
    if (current)
    {
        for (int i = 0; i < rowCount(); ++i)
        {
            if (current == getAnimatedLayer(i))
            {
                pos = i;
                break;
            }
        }
    }

    KisNode* node = new KisGroupLayer(m_image, "_ani_New Animated Layer", 255);
    m_nodeman->insertNode(node, m_image->root(), 0);

    loadLayers();

    return createIndex(rowCount()-1, 0);
}

void AnimatorModel::deleteLayer()
{
    KisNode* current = getActiveAnimatedLayer();
    if (current)
        m_nodeman->removeNode(current);
}

void AnimatorModel::layerDown()
{
    KisNode* current = getActiveAnimatedLayer();
    if (current)
    {
        m_nodeman->activateNode(current);
        m_nodeman->raiseNode();
    }
}

void AnimatorModel::layerUp()
{
    KisNode* current = getActiveAnimatedLayer();
    if (current)
    {
        m_nodeman->activateNode(current);
        m_nodeman->lowerNode();
    }
}

QString& AnimatorModel::getLayerName(QModelIndex index)
{
    return getLayerName(index.row());
}

QString& AnimatorModel::getLayerName(int l_num)
{
    QString* s = new QString(getAnimatedLayer(l_num)->name().mid(5));
    return *s;
}

void AnimatorModel::setLayerName(QModelIndex index, QString& name)
{
    setLayerName(index.row(), name);
}

void AnimatorModel::setLayerName(int l_num, QString& name)
{
    if (l_num < rowCount() && l_num >= 0)
    {
        // TODO: move this to *AnimatedLayer classes
        getAnimatedLayer(l_num)->setName(QString("_ani_")+name);
    }
}

FrameLayer* AnimatorModel::getCachedFrame(const QModelIndex& index) const
{
    return getCachedFrame(index.row(), index.column());
}

FrameLayer* AnimatorModel::getCachedFrame(quint32 l, quint32 f) const
{
    AnimatedLayer* al = const_cast<AnimatorModel*>(this)->getAnimatedLayer(l);
    FrameLayer* fl = al->getCachedFrame(f);
    return fl;
}

FrameLayer* AnimatorModel::getUpdatedFrame(const QModelIndex& index)
{
    return getUpdatedFrame(index.row(), index.column());
}

FrameLayer* AnimatorModel::getUpdatedFrame(quint32 l, quint32 f)
{
    return getAnimatedLayer(l)->getUpdatedFrame(f);
}

AnimatedLayer* AnimatorModel::getAnimatedLayer(quint32 num) const
{
    if (num < rowCount())
        return m_layers[num];
    return 0;
}

AnimatedLayer* AnimatorModel::getAnimatedLayerByChild(const KisNode* node) const
{
    if (rowCount() == 0)
    {
        return 0;
    }
    
//     const KisNode* tnode = m_nodeman->activeNode();
    const KisNode* tnode;
//     KisNode* current_layer = const_cast<KisNode*>( tnode );

    KisNode* current_layer = dynamic_cast<KisNode*>( const_cast<KisNode*>( node ) );
    
    bool node_ok = false;
    
    // Check node, than parents
    while (current_layer && current_layer->parent() != 0 && !node_ok)
    {
        AnimatedLayer* lay;
        foreach (lay, m_layers)
        {
            if (lay == current_layer) // || lay->source() == current_layer)
            {
                node_ok = true;
                break;
            }
        }
        
        if (!node_ok)
        {
            tnode = current_layer->parent();
            current_layer = const_cast<KisNode*>(tnode);
        }
    }
    if (!current_layer || current_layer->parent() == 0)
    {
        current_layer = m_layers[0];
//         return 0;
    }
    return dynamic_cast<AnimatedLayer*>( current_layer );
}

void AnimatorModel::setOnionNext(int n)
{
    m_onion_nxt = n;
    emit frameChanged(m_frame);
}

void AnimatorModel::setOnionPrevious(int n)
{
    m_onion_prv = n;
    emit frameChanged(m_frame);
}

void AnimatorModel::setOnionOpacity(quint8 op)
{
    m_onion_opacity = op;
    emit frameChanged(m_frame);
}

void AnimatorModel::disableOnion()
{
    m_onion_en = false;
}

void AnimatorModel::enableOnion()
{
    m_onion_en = true;
}

const QModelIndex& AnimatorModel::indexFromNode(KisNode* node) const
{
    if (! node)
        return QModelIndex();
    
    int i, j;
    for (i = 0; i < rowCount(); ++i)
    {
        for (j = 0; j < m_layers[i]->childCount(); ++j)
        {
            if (node == m_layers[i]->at(j))
            {
                return createIndex(i, j);
            }
        }
    }
    return QModelIndex();
}


// For player
void AnimatorModel::goNext()
{
    // TODO: check for control layers
    goFrame(m_frame+1);
}

void AnimatorModel::goFirst()
{
    goFrame(0);
}

void AnimatorModel::goFrame(int num)
{
    m_previous_frame = m_frame;
    if (num == m_frame)
        return;
    
    int old_f = previousFrame();
    
    if ((num >= 0 && num < columnCount()) || num == 0)
    {
        m_frame = num;
        emit frameChanged(m_frame);
        emit dataChanged(createIndex(old_f, 0), createIndex(old_f, rowCount()));
        emit dataChanged(createIndex(m_frame, 0), createIndex(m_frame, rowCount()));
    }

}


bool AnimatorModel::isLast()
{
    return m_frame >= columnCount()-1;
}

void AnimatorModel::toggleExtLTable(bool val)
{
    m_ext_lighttable = val;
    updateImage();
}

void AnimatorModel::setEnabled(bool val)
{
    m_enabled = val;
    if (val)
    {
        loadLayers();
    } else
    {
        m_layers.clear();
    }
}

void AnimatorModel::setVisible(bool val)
{
    m_visible = val;
    if (val)
    {
        loadLayers();
        visibleAll(false);
        updateImage();
    } //else
//         visibleAll();
}

void AnimatorModel::framesDelete(int n)
{
    framesDelete(m_frame, n);
}

void AnimatorModel::framesDelete(unsigned int src, int n)
{
    framesClear(src, n);
    framesMove(src+n, -1, src);
}

void AnimatorModel::framesInsert(int n)
{
    framesInsert(n, m_frame);
}

void AnimatorModel::framesInsert(int n, unsigned int dst)
{
//    if (n <= 0)
//        return;
    framesMove(dst, -1, dst+n);

//    loadLayers();
}

void AnimatorModel::frameMoveTo(QModelIndex& index, QModelIndex& moveto)
{
    frameMoveTo(index.row(), index.column(), moveto.row(), moveto.column());
}

void AnimatorModel::frameMoveTo(int l, int f, int tl, int tf)
{
    if (l == tl)
    {
        if (isKey(l, f))
            getAnimatedLayer(l)->getKeyFrame(f)->setName(getAnimatedLayer(l)->getNameForFrame(tf, true));
//        if (isKey(tl, tf))
//            getAnimatedLayer(l)->getKeyFrame(tf)->setName(getAnimatedLayer(l)->getNameForFrame(f, true));
    }
    else
    {
        // Moving between layers is not implemented yet
    }
}

void AnimatorModel::framesMove(unsigned int src, int n, unsigned int dst)
{
    QModelIndexList list;
    for (int i = src; n != 0 && i < columnCount(); ++i)
    {
        for (int row = 0; row < rowCount(); ++row)
        {
            list.append(createIndex(row, i));
        }
        if (n != -1)        // Move all frames
            --n;
    }
    framesMove(list, dst-src);
}

void AnimatorModel::framesMove(QModelIndexList fl, int move)
{
//    if (frames.selection().empty())
    if (fl.empty())
    {
        return ;
    }
    
    QModelIndex frame;
//    QModelIndexList fl = frames.selectedIndexes();
    
    foreach (frame, fl)
    {
        QModelIndex mvto = createIndex(frame.row(), frame.column()+move);
        // be sure to not swap with already moving frame
        if (!fl.contains(mvto))
            frameMoveTo(mvto, frame);
        frameMoveTo(frame, mvto);
    }
    
    loadLayers();
}

void AnimatorModel::framesClear(unsigned int src, int n)
{
    if (n < 0)
    {
        n = columnCount()-src;
    }
    for (int i = src; i < src + n; ++i)
    {
        for (int j = 0; j < rowCount(); ++j)
        {
            clearFrame(index(j, i));
        }
    }
    
    loadLayers();
}



void AnimatorModel::frameLeft()
{
    // cant move left
    if (m_frame == 0)
        return;
    framesMove(m_frame, 1, m_frame-1);
    goFrame(m_frame-1);
}

void AnimatorModel::frameRight()
{
    framesMove(m_frame, 1, m_frame+1);
    goFrame(m_frame+1);
}



bool AnimatorModel::isKey(const QModelIndex& index) const
{
    return isKey(index.row(), index.column());
}

bool AnimatorModel::isKey(int l, int f) const
{
    return const_cast<AnimatorModel*>(this)->getAnimatedLayer(l)->isKeyFrame(f);
}


void AnimatorModel::visibleAll(bool v)
{
    // TODO
}


// Version control
int AnimatorModel::getMajor(const KisNode* meta_node)
{
    QString t = meta_node->name();
    
    if (! t.startsWith("_animator_"))
        return -1;
    
    std::stringstream ns;
    ns << t.mid(10).toAscii().data();
    int result;
    ns >> result;
    
    return result;
}

int AnimatorModel::getMajor()
{
    return 1;
}

int AnimatorModel::getMinor(const KisNode* meta_node)
{
    QString t = meta_node->name();
    
    if (! t.startsWith("_animator_"))
        return -1;
    
    std::stringstream ns;
    ns << t.mid(10).toAscii().data();
    int m, result;
    char tmp;
    ns >> m >> tmp >> result;             // major_minor (e.x. 2_5)
    
    return result;
}

int AnimatorModel::getMinor()
{
    return 0;
}

void AnimatorModel::setVersion(int major, int minor)
{
    m_nodeman->createNode("KisGroupLayer");
    
    KisGroupLayer* meta_node = dynamic_cast<KisGroupLayer*>(m_nodeman->activeLayer().data());
    
    meta_node->setName(QString("_animator_")+QString::number(major)+QString("_")+QString::number(minor)+QString("_Please don\'t move this!"));
    m_nodeman->moveNodeAt(meta_node, 0, 0);
}
