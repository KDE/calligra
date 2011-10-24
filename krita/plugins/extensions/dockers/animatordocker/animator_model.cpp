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

#include "animator_model.h"

#define BASE_COLUMNS_NUMBER 1

#include "framed_animated_layer.h"
#include "animator_manager_factory.h"

AnimatorModel::AnimatorModel(KisImage* image): QAbstractItemModel(0)
{    
//     m_nodeModel = new KisNodeModel(this);
//     m_nodeModel->setImage(image);
    m_image = image;
    
    connect(m_image, SIGNAL(sigNodeHasBeenAdded(KisNode*,int)), SLOT(layoutChangedSlot()));
    connect(m_image, SIGNAL(sigNodeHasBeenMoved(KisNode*,int,int)), SLOT(layoutChangedSlot()));
    connect(m_image, SIGNAL(sigNodeHasBeenRemoved(KisNode*,int)), SLOT(layoutChangedSlot()));

    connect(m_image, SIGNAL(sigNodeChanged(KisNode*)), SLOT(dataChangedSlot(KisNode*)));
    
    connect(m_image, SIGNAL(sigAboutToBeDeleted()), SLOT(removeThis()));
}

AnimatorModel::~AnimatorModel()
{
}

void AnimatorModel::removeThis()
{
    delete this;
}


void AnimatorModel::dataChangedSlot(KisNode* node)
{
    emit dataChanged(indexFromNode(node), indexFromNode(node));
}

void AnimatorModel::layoutChangedSlot()
{
    emit layoutChanged();
}


KisImage* AnimatorModel::image() const
{
    return m_image;
}


QVariant AnimatorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole)
        {
            if (frameNumber(section) >= 0)
                return QString::number(frameNumber(section));
            else
                return "Layer";
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant AnimatorModel::data(const QModelIndex& ind, int role) const
{
    // layers, not frames
    if (ind.column() == 0)
    {
        KisNode* node = nodeFromIndex(ind);
        if (role == Qt::DisplayRole)
            return node ? node->name() : "nonode";
        return QVariant();
    } else                      // frames
    {
        AnimatedLayer* alayer = dynamic_cast<AnimatedLayer*>(nodeFromIndex(index(ind.row(), 0, ind.parent())));
        if (alayer && alayer->inherits("FramedAnimatedLayer"))
        {
            FrameLayer* frame = dynamic_cast<FramedAnimatedLayer*>(alayer)->frameAt(frameNumber(ind));
            if (frame)
            {
                if (role == Qt::DisplayRole)
                    return "x";
            }
        }
    }
    
//     if (role == Qt::DisplayRole)
//         return "-";
    
    return QVariant();
}

int AnimatorModel::columnCount(const QModelIndex& parent) const
{
//     KisNode* pnode = nodeFromIndex(parent);
    return AnimatorManagerFactory::instance()->getManager(m_image)->framesNumber() + BASE_COLUMNS_NUMBER;
}

int AnimatorModel::rowCount(const QModelIndex& parent) const
{
    KisNode* pnode = nodeFromIndex(parent);
    
    if (!parent.isValid() || (parent.column() == 0 && pnode && !qobject_cast<AnimatedLayer*>(pnode)))
    {
        return pnode->childCount();
    }
    return 0;
}

QModelIndex AnimatorModel::parent(const QModelIndex& child) const
{
    KisNode* pnode = (KisNode*) child.internalPointer();
    return indexFromNode(pnode);
}

QModelIndex AnimatorModel::index(int row, int column, const QModelIndex& parent) const
{
    KisNode* pnode = nodeFromIndex(parent);
    return createIndex(row, column, pnode);
}


int AnimatorModel::frameNumber(const QModelIndex& index) const
{
    return frameNumber(index.column());
}

int AnimatorModel::frameNumber(int column) const
{
    return column - BASE_COLUMNS_NUMBER;
}


KisNode* AnimatorModel::nodeFromIndex(const QModelIndex& index) const
{
    if (!index.isValid())
        return (KisNode*)m_image->root().data();
    KisNode* parent = (KisNode*)index.internalPointer();
    if (!parent)
        return 0;
    KisNode* node = parent->at(parent->childCount()-index.row()-1).data();
    if (index.column() != 0)
    {
        FramedAnimatedLayer* al = dynamic_cast<FramedAnimatedLayer*>(node);
        if (!al)
            return 0;
        node = al->frameAt(frameNumber(index));
    }
    return node;
}

QModelIndex AnimatorModel::indexFromNode(const KisNode* node) const
{
    if (!node)
        return QModelIndex();
    KisNode* pnode = node->parent().data();
    if (!pnode)
        return QModelIndex();
    int row = pnode->childCount()-pnode->index(const_cast<KisNode*>(node))-1;
    return createIndex(row, 0, (void*)pnode);
}
