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

#include <iostream>

AnimatorModel::AnimatorModel(QObject* parent): QAbstractTableModel(parent)
{
    init();
    setLightTable(0);
    connect(this, SIGNAL(frameChanged(int)), SLOT(frameUpdate()));
}

void AnimatorModel::init()
{
    m_enabled = false;
    
    this->blockSignals(true);
    setFramesNumber(12, false);
    setFrame(0);
    
    setOnionNext(0);
    setOnionPrevious(0);
    
    setOnionOpacity(127);
    
    enableOnion();                      // Onion should be only disabled while exporting/playing
    
    m_ext_lighttable = false;
    
//     setLightTable(0);
    
    m_main_layers.clear();
    QList<KisNode*> nl;
    foreach (nl, m_frames_layers)
    {
        nl.clear();
    }
    m_frames_layers.clear();
    
    this->blockSignals(false);
    
//     std::cout << m_frame << std::endl;
//     connect(this, SIGNAL(frameChanged(int)), SLOT(frameUpdate()));
}

AnimatorModel::~AnimatorModel()
{
//    delete m_main_layers;
    delete m_source;
}

void AnimatorModel::realUpdate()
{
//     blockSignals(true);
    
    KisNodeModel* nodes = sourceModel();
    
    if (! nodes )
        return;
    
//     beginRemoveRows(QModelIndex(), 0, rowCount());
    
    m_main_layers.clear();
    
//     endRemoveRows();
    
    for (qint32 j = 0; j < m_frames_layers.length(); ++j)
    {
        m_frames_layers[j].clear();
    }
    
    m_frames_layers.clear();
    
    for (qint32 i = 0; i < nodes->rowCount(); ++i)
    {
//         beginInsertRows(QModelIndex(), i, i+1);
        QModelIndex index = sourceModel()->index(i, 0);

        const KisNode* cnode = sourceModel()->nodeFromIndex(index);
        KisNode* node = const_cast<KisNode*>( cnode );
        
        if (node->inherits("KisGroupLayer") && node->name().startsWith("_ani_"))
        {
            QList< KisNode* > nlist;
            
            for ( qint32 i = 0; i < node->childCount(); ++i )
            {
                const KisNode* chnode = node->childNodes(QStringList(),  KoProperties())[i];
                if (chnode->name().startsWith("_frame_"))
                {
                    KisNode* mchnode = const_cast<KisNode*>(chnode);
                    QString ts = chnode->name();
                    
                    qint32 fnum = ts.mid(7).toLong();
                    
                    if (fnum+1 > columnCount()) {
                        setFramesNumber(fnum+1, false);
                    }
                    
                    if (fnum == nlist.length())
                    {
                        nlist.append(mchnode);
                    } else {
//                         std::cout << "Look here " << fnum << " " << nlist.length() << std::endl;
                        if (fnum > nlist.length())
                        {
                            for (qint32 fc = fnum-nlist.length()+1; fc; --fc)
                            {
//                                 std::cout << "aaa " << fc << " " << fnum << " " << nlist.length() << std::endl;
                                nlist.append(0);
                            }
                        }
//                         std::cout << "And here " << fnum << " " << nlist.length() << std::endl;
                        
                        nlist[fnum] = mchnode;
                    }
                }
            }
            
//             if (nlist.length() == 0 || nlist[0] == 0)
//             {
//                 if (nlist.length() == 0)
//                     nlist.append(0);
//                 nlist[0] = ;
//             }
            
            m_main_layers.append(node);
            m_frames_layers.append(nlist);
        }
        
//         endInsertRows();
    }
    
    KisNode* node;
    foreach (node, m_main_layers)
    {
        connect(node, SIGNAL(destroyed(QObject*)), this, SLOT(nodeDestroyed(QObject*)));
    }
    
//     blockSignals(false);
}

void AnimatorModel::afterUpdate()
{
//     std::cout << "Before emmiting " << m_frame << std::endl;
    
    emit frameChanged(m_frame);
    emit framesNumberChanged(m_frame_num);
    
    emit dataChanged(createIndex(0, 0), createIndex(rowCount(), columnCount()));
    emit layoutChanged();
}

void AnimatorModel::update()
{
    realUpdate();
    afterUpdate();
}

void AnimatorModel::setSourceModel(KisNodeModel* model)
{
    m_source = model;
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

QVariant AnimatorModel::data(const QModelIndex& index, int role) const
{
    QVariant t;

    if (role == Qt::DisplayRole)
    {
        if (nodeAtIndex(index))
        {
//             t = "x";
        }
//         else if (extrapolated(index))
//         {
//              t = "-";
//         }
    } else if (role == Qt::BackgroundRole)
    {
        // TODO: use constants and colors, not manually rgb...
        int r = 127, g = 127, b = 127;
        
        if (index.column() == m_frame)
        {
            r -= 64;
            g -= 64;
            b -= 64;
//             if (m_)
        }
        
        if (nodeAtIndex(index))
        {
            if (nodeAtIndex(index)->inherits("KisPaintLayer"))
            {
                g += 64;
                b -= 32;
                r -= 32;
            }
            else
            {
                b += 64;
                g -= 32;
                r -= 32;
            }
        }
        
        return QBrush(QColor(r, g, b, 127));
    }
    return t;
}

QVariant AnimatorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant t;
    
    if (orientation == Qt::Vertical) {
        if (role == Qt::DisplayRole)
        {
            
            if (section >= m_main_layers.length())
            {
                return t;
            }
            
            KisNode* n = m_main_layers[section];
            
            if (! n)
            {
                return t;
            }
            
            
            QString ts = n->name();
            
            t = ts.mid(5);
        }
    } else      // orientation == Qt::Horizontal
    {
        if (role == Qt::DisplayRole)
        {
            if (section % 6 == 0)
            {
                return QString::number(section);
            }
        } else if (role == Qt::BackgroundRole)
        {
            if (section == m_frame)
            {
                return QBrush(QColor(10, 10, 10, 127));
            }
        }
    }
    
    return t;
}


int AnimatorModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_frame_num;
}

int AnimatorModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_main_layers.size();
}

void AnimatorModel::setNodeManager(KisNodeManager* nodeman)
{
    m_nodeman = nodeman;
}

void AnimatorModel::setFrame(int frame)
{
    if (frame == m_frame)
        return;
    
    if ((frame >= 0 && frame < columnCount()) || frame == 0)
    {
        m_frame = frame;
        emit frameChanged(m_frame);
    }
}

void AnimatorModel::setFramesNumber(int frames, bool up)
{
    if (m_frame_num == frames)
        return;
    m_frame_num = frames;
    if (up)
    {
        emit layoutChanged();
//         update();
    }
    emit framesNumberChanged(frames);
}


void AnimatorModel::frameUpdate()
{
    if (!m_enabled)
        return;
    
    // Just old code without onion support
    for (qint32 i = 0; i < rowCount(); ++i)
    {
        KisNode* frame = 0;
        
        // Make all frames unvisible
        unvisibleLayer(i);
        
        frame = nodeFromIndex(createIndex(i, m_frame));
        if (frame)
        {
            frame->setOpacity(255);             // Restore after onion skin
            frame->setVisible(true);
            m_image->updateProjection(frame, m_image->bounds());
        }
        
        if (m_onion_en && !m_ext_lighttable)
        {
            // Onion support --> move to animator_light_table
            int nxt = m_onion_nxt;
            quint8 opacity = m_onion_opacity;
            
    //         int b = ;
            int fr = m_frame;
            while (nxt)
            {
                KisNode* node = nextFrame(createIndex(i, fr));
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
                KisNode* node = previousFrame(createIndex(i, fr));
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
    
    lightTableUpdate();
    
    if (m_nodeman)
    {
        m_nodeman->nodesUpdated();
        m_nodeman->updateGUI();
    }

    emit headerDataChanged(Qt::Vertical, 0, columnCount()-1);
    
//     if (rec)
    updateCanvas();
}

void AnimatorModel::lightTableUpdate()
{
    if (!m_enabled)
        return;
    
    if (!m_ext_lighttable || !m_onion_en)
        return;
    
    for (int i = 0; i < rowCount(); ++i)
    {
//         std::cout << "YES" << std::endl;
        // New extended lighttable
        for (int fnum = -m_light_table->getNear(); fnum <= m_light_table->getNear(); ++fnum)
        {
//             if (fnum == 0)
//                 continue;
            
            KisNode* fnode = nodeAtIndex(createIndex(i, m_frame+fnum));
            if (fnode)
            {
                double dop = m_light_table->getOpacity(fnum);
                quint8 op = dop*255.0;
//                 std::cout << dop << " " << (int)op << std::endl;
                fnode->setOpacity( op );
                fnode->setVisible(m_light_table->getVisibility(fnum));
                m_image->updateProjection(fnode, m_image->bounds());
            }
        }
    }
    
    updateCanvas();
}

void AnimatorModel::unvisibleLayer(int n)
{
    KisNode* frame;
    QList<KisNode*> b = m_frames_layers[n];
    foreach (frame, b)
    {
        if (frame)
        {
            frame->setVisible(false);
        }
    }
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


void AnimatorModel::activateLayer(QModelIndex index)
{
    setFrame(index.column());
    
    if (nodeFromIndex(index))
        m_nodeman->activateNode( nodeFromIndex(index) );
    
    afterUpdate();
}

void AnimatorModel::updateCanvas()
{
    if (!m_image || !m_canvas)
        return;
    
    m_canvas->startUpdateCanvasProjection(m_image->bounds());
}

KisNode* AnimatorModel::nodeFromIndex(const QModelIndex& index) const
{
    KisNode* result = nodeAtIndex(index);
    if (!result)
        result = previousFrame(index);
    return result;
}

KisNode* AnimatorModel::nextFrame(const QModelIndex& index) const
{
    qint32 base = index.row();
    qint32 fr = index.column()+1;
    
    KisNode* node = 0;
    
    if (m_frames_layers.length() > base)
    {
        while (!node && fr < m_frames_layers[base].length())
        {
            node = m_frames_layers[base][fr];
            ++fr;
        }
    }
    
    return node;
}

KisNode* AnimatorModel::previousFrame(const QModelIndex& index) const
{
    qint32 base = index.row();
    qint32 fr = index.column()-1;
    
    KisNode* node = 0;
    
    if (m_frames_layers.length() > base)
    {
        while (!node && fr >= 0)
        {
            if (fr < m_frames_layers[base].length())
                node = m_frames_layers[base][fr];
            --fr;
        }
    }
    
    return node;
}

KisNode* AnimatorModel::nodeAtIndex(const QModelIndex& index) const
{
    if (m_frames_layers.length() > index.row() && index.row() >= 0)
    {
        if (m_frames_layers[index.row()].length() > index.column() && index.column() >= 0)
        {
            return m_frames_layers[index.row()][index.column()];
        }
    }
    return 0;
}

bool AnimatorModel::activateAtIndex(QModelIndex index)
{
    if (nodeAtIndex(index))
    {
        return false;
    }
    
    KisNode* node = previousFrame(index);
    if (node)
    {
        m_nodeman->activateNode(node);
    } else
    {
        m_nodeman->activateNode(m_main_layers[index.row()]);
    }
    
    return true;
}

bool AnimatorModel::activateBeforeIndex(QModelIndex index)
{
    if (nodeAtIndex(index))
    {
        return false;
    }
    
    KisNode* node = previousFrame(index);
    if (node)
    {
        m_nodeman->activateNode(node);
    } else
    {
        // CHECK
        m_nodeman->activateNode(m_main_layers[index.row()]);
        return false;
    }
    
    return true;
}

bool AnimatorModel::activateAfterIndex(QModelIndex index)
{
    if (nodeAtIndex(index))
        return false;
    
    KisNode* node = nextFrame(index);
    
    if (node)
    {
        m_nodeman->activateNode(node);
    } else
    {
        m_nodeman->activateNode(m_main_layers[index.row()]);
        return false;
    }
    return true;
}

void AnimatorModel::createFrame(QModelIndex index, char* layer_type)
{
    if (activateAtIndex(index))
    {
//         if (layer_type == "KisGroupLayer")
//         {
//             m_nodeman->activateNode(getAnimatedLayerByChild(nodeFromIndex(index)));
//         }
        if (m_nodeman->activeNode()->inherits("KisGroupLayer"))
        {
            m_nodeman->activateNode(m_main_layers[index.row()]);
        }
        
        m_nodeman->createNode(layer_type);
        
        m_nodeman->activeNode()->setName("_frame_" + QString::number(index.column()));                  // TODO: Set name with zero-leading
        
        update();
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
        foreach (node, m_main_layers[index.row()]->childNodes(QStringList(), KoProperties()))
        {
            if (node->name().startsWith("Duplicate"))
            {
//                 KisNode* nn = const_cast<KisNode*>(node.data());
                m_nodeman->activateNode(node);
            }
        }
        
        m_nodeman->activeNode()->setName("_frame_" + QString::number(index.column()));
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
        foreach (node, m_main_layers[index.row()]->childNodes(QStringList(), KoProperties()))
        {
            if (node->name().startsWith("Duplicate"))
            {
//                 KisNode* nn = const_cast<KisNode*>(node.data());
                m_nodeman->activateNode(node);
            }
        }
        
        m_nodeman->activeNode()->setName("_frame_" + QString::number(index.column()));
    } else
    {
        std::cout << "Has node already or no frames: can't copy" << std::endl;
    }
}

void AnimatorModel::clearFrame(QModelIndex index)
{
    KisNode* node = nodeAtIndex(index);
    if (node)
    {
        m_nodeman->removeNode(node);
        update();
    } else
    {
        std::cout << "Already clear" << std::endl;
        return;
    }
}

void AnimatorModel::nodeDestroyed(QObject* node)
{
    for (int i = 0; i < m_main_layers.length(); ++i) {
        KisNode* layer = m_main_layers[i];
        if (layer == node)
        {
            m_main_layers[i] = 0;
        }
    }
}

void AnimatorModel::createLayer()
{
//     const KisNode* tnode = m_nodeman->activeNode();
//     KisNode* current_layer = const_cast<KisNode*>( tnode );
//     
//     if (current_layer->parent() == 0)
//     {
//         // Active layer is root
//         if (m_main_layers.length() > 0)
//         {
//             current_layer = m_main_layers[0];
//         } else
//         {
//             // No frame layers currently
//             
//         }
//     } else
//     {
//         // TODO: add layer just after current selected:
//         // Check current active layer -> if it's frame or frame layer than move new layer under it after creation
//         
//     }
    
    int pos = 0;
    
    KisNode* current = getAnimatedLayerByChild(m_nodeman->activeNode());
    if (current)
    {
        for (int i = 0; i < rowCount(); ++i)
        {
            if (current == m_main_layers[i])
            {
                pos = i;
                break;
            }
        }
    }

    KisNode* node = new KisGroupLayer(m_image, "_ani_NNN", 255);                // TODO: ask for name
//     m_nodeman->insertNode(node, m_image->root(), 0);
    m_nodeman->insertNode(node, m_image->root(), pos);
}

void AnimatorModel::deleteLayer()
{
    KisNode* current = getAnimatedLayerByChild(m_nodeman->activeNode());
    if (current)
        m_nodeman->removeNode(current);
}

void AnimatorModel::layerDown()
{
    KisNode* current = getAnimatedLayerByChild(m_nodeman->activeNode());
    if (current)
    {
        m_nodeman->activateNode(current);
        m_nodeman->raiseNode();
    }
}

void AnimatorModel::layerUp()
{
    KisNode* current = getAnimatedLayerByChild(m_nodeman->activeNode());
    if (current)
    {
        m_nodeman->activateNode(current);
        m_nodeman->lowerNode();
    }
}

void AnimatorModel::renameLayer(QModelIndex index, QString& name)
{
    renameLayer(index.row(), name);
}

void AnimatorModel::renameLayer(int l_num, QString& name)
{
    if (l_num < m_main_layers.length() && l_num >= 0)
    {
        m_main_layers[l_num]->setName(QString("_ani_")+name);
    }
}

KisNode* AnimatorModel::getAnimatedLayerByChild(const KisNode* node)
{
    if (rowCount() == 0)
    {
        return 0;
    }
    
//     const KisNode* tnode = m_nodeman->activeNode();
    const KisNode* tnode;
//     KisNode* current_layer = const_cast<KisNode*>( tnode );

    KisNode* current_layer = const_cast<KisNode*>( node );
    
    bool node_ok = false;
    
    // Check node, than parents
    while (current_layer->parent() != 0 && !node_ok)
    {
        KisNode* node;
        foreach (node, m_main_layers)
        {
            if (node == current_layer)
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
    if (current_layer->parent() == 0)
    {
        current_layer = m_main_layers[0];
//         return 0;
    }
    return current_layer;
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
        for (j = 0; j < m_frames_layers[i].length(); ++j)
        {
            if (node == m_frames_layers[i][j])
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
    setFrame(m_frame+1);
}

void AnimatorModel::toogleExtLTable(bool val)
{
    m_ext_lighttable = val;
    frameUpdate();
}

void AnimatorModel::visibleAll()
{
    QList<KisNode*> lay;
    foreach (lay, m_frames_layers)
    {
        KisNode* frame;
        foreach (frame, lay)
        {
            if (frame)
            {
                frame->setVisible(true);
            }
        }
    }
}

void AnimatorModel::setEnabled(bool en)
{
    m_enabled = en;
    if (en)
    {
        update();
        frameUpdate();
    } else
        visibleAll();
}


void AnimatorModel::framesDelete(unsigned int src, int n)
{
    framesClear(src, n);
    framesMove(src+n, -1, src);
    
    update();
}

void AnimatorModel::framesInsert(int n, unsigned int dst)
{
    if (n <= 0)
        return;
    
    update();
}

void AnimatorModel::framesMove(unsigned int src, int n, unsigned int dst)
{
    if (n < 0)
    {
        n = columnCount()-src;
    }
    for (int l = 0; l < m_frames_layers.length(); ++l)
    {
        for (int i = src, j = dst; i < src+n; ++i, ++j)
        {
//             printf("Hello l = %d, i = %d, j = %d\n", l, i, j);
            if (j < m_frames_layers[l].length() && m_frames_layers[l][j])
            {
//                 printf("way1\n");
                m_frames_layers[l][j]->setName("_frame_" + QString::number(i));
            }
            if (i < m_frames_layers[l].length() && m_frames_layers[l][i])
            {
//                 printf("way2\n");
                m_frames_layers[l][i]->setName("_frame_" + QString::number(j));
            }
        }
    }
    
    update();
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
    
    update();
}



void AnimatorModel::frameLeft()
{
    if (m_frame == 0)
        return;
    framesMove(m_frame, 1, m_frame-1);
    setFrame(m_frame-1);
}

void AnimatorModel::frameRight()
{
    framesMove(m_frame, 1, m_frame+1);
    setFrame(m_frame+1);
}
