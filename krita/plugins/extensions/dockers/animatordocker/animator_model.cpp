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

#include <iostream>

AnimatorModel::AnimatorModel(KisImage* image): QAbstractItemModel(0)
{
    std::cout << "AnimatorModel(" << (void*) this << ", " << (void*) image << ")" << std::endl;
    
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
    std::cout << "~AnimatorModel(" << (void*) this << ")" << std::endl;
}

void AnimatorModel::removeThis()
{
    delete this;
}


// void AnimatorModel::setSourceModel(KisNodeModel* model)
// {
//     m_nodeModel = model;
//     connect(model, SIGNAL(layoutChanged()), this, SLOT(layoutChangedSlot()));
//     connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(dataChangedSlot(QModelIndex,QModelIndex)));
//     connect(m_nodeModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)), SIGNAL(rowsInserted(const QModelIndex&, int, int)));
// //     connect(m_nodeModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)), SLOT(updateUI()));
// //     connect(m_nodeModel, SIGNAL(rowsMoved(const QModelIndex&, int, int, const QModelIndex&, int)), SLOT(updateUI()));
//     connect(m_nodeModel, SIGNAL(modelReset()), SIGNAL(modelReset()));
    
    
//     connect(m_nodeModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)), SLOT(layoutChangedSlot()));
//     connect(m_nodeModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)), SLOT(layoutChangedSlot()));
//     connect(m_nodeModel, SIGNAL(rowsMoved(const QModelIndex&, int, int, const QModelIndex&, int)), SLOT(layoutChangedSlot()));
//     connect(m_nodeModel, SIGNAL(modelReset()), SLOT(layoutChangedSlot()));
    
    
// }

void AnimatorModel::dataChangedSlot(KisNode* node)
{
    std::cout << "dataChanged" << std::endl;
    emit dataChanged(indexFromNode(node), indexFromNode(node));
}

void AnimatorModel::layoutChangedSlot()
{
    std::cout << "layoutChanged" << std::endl;
    emit layoutChanged();
}


QVariant AnimatorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant AnimatorModel::data(const QModelIndex& index, int role) const
{
    // layers, not frames
    if (index.column() == 0)
    {
        KisNode* node = nodeFromIndex(index);
        if (role == Qt::DisplayRole)
            return node->name();
        return QVariant();
    } else                      // frames
    {
//         AnimatedLayer* alayer = nodeFromIndex(index(index.row(), 0, index.parent()));
//         if (alayer->inherits("FramedAnimatedLayer"))
//         {
//             FrameLayer* frame = dynamic_cast<FramedAnimatedLayer*>(alayer)->frameAt(index.column()+1);
//             if (frame)
//             {
//                 if (role == Qt::DisplayRole)
//                     return "x";
//             }
//         }
    }
    
    return QVariant();
}

int AnimatorModel::columnCount(const QModelIndex& parent) const
{
    if (! nodeFromIndex(parent)->inherits("AnimatedLayer"))
        return 1;
    return 12;
}

int AnimatorModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid() || parent.column() == 0)
    {
        return nodeFromIndex(parent)->childCount();
    }
    return 0;
}

QModelIndex AnimatorModel::parent(const QModelIndex& child) const
{
    if (child.column() == 0)
    {
        KisNode* pnode = nodeFromIndex(child)->parent().data();
        
        if (pnode == m_image->root())
            return QModelIndex();
        
        KisNode* ppnode = pnode->parent().data();
        int row = ppnode->childCount()-ppnode->index(pnode)-1;
        
        return createIndex(row, 0, pnode);
    }
    return QModelIndex();
}

QModelIndex AnimatorModel::index(int row, int column, const QModelIndex& parent) const
{
    KisNode* pnode = nodeFromIndex(parent);
    KisNode* node = pnode->at(pnode->childCount()-row-1).data();
    return createIndex(row, column, node);
}

KisNode* AnimatorModel::nodeFromIndex(const QModelIndex& index) const
{
    if (!index.isValid())
        return (KisNode*)m_image->root().data();
    return (KisNode*)index.internalPointer();
}

QModelIndex AnimatorModel::indexFromNode(const KisNode* node) const
{
    KisNode* pnode = node->parent().data();
    if (!pnode)
        return QModelIndex();
    int row = pnode->childCount()-pnode->index(const_cast<KisNode*>(node))-1;
    return createIndex(row, 0, (void*)node);
}
