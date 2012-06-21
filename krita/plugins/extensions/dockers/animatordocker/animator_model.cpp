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

#define BASE_COLUMNS_NUMBER 3

#include <KIconLoader>

#include "framed_animated_layer.h"
#include "animator_manager_factory.h"
#include "animator_switcher.h"
#include "view_animated_layer.h"

AnimatorModel::AnimatorModel(KisImageWSP image): QAbstractItemModel(0)
{
    m_image = image;

    connect(m_image, SIGNAL(sigLayersChangedAsync()), SLOT(layoutChangedSlot()));
    connect(m_image, SIGNAL(sigNodeAddedAsync(KisNodeSP)), SLOT(layoutChangedSlot()));
    connect(m_image, SIGNAL(sigRemoveNodeAsync(KisNodeSP)), SLOT(layoutChangedSlot()));

    connect(m_image, SIGNAL(sigNodeChanged(KisNodeSP)), SLOT(dataChangedSlot(KisNodeSP)));

    connect(m_image, SIGNAL(sigAboutToBeDeleted()), SLOT(removeThis()));

    AnimatorManager* manager = AnimatorManagerFactory::instance()->getManager(image);

    connect(manager, SIGNAL(framesNumberChanged(int)), SLOT(layoutChangedSlot()));

    m_frameWidth = 8;
    m_showThumbs = false;
}

AnimatorModel::~AnimatorModel()
{
}

void AnimatorModel::removeThis()
{
    delete this;
}


void AnimatorModel::dataChangedSlot(KisNodeSP node)
{
    emit dataChanged(indexFromNode(node), indexFromNode(node));
}

void AnimatorModel::dataChangedSlot(int from, int to)
{
    int last = rowCount(QModelIndex());
    emit dataChanged(index(0, from, QModelIndex()), index(last, from, QModelIndex()));
    emit dataChanged(index(0, to, QModelIndex()), index(last, to, QModelIndex()));
}

void AnimatorModel::layoutChangedSlot()
{
    emit layoutChanged();
}


KisImageWSP AnimatorModel::image() const
{
    return m_image;
}


QVariant AnimatorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
    {
        if (frameNumber(section) >= 0)
        {
            if (role == Qt::DisplayRole)
                return QString::number(frameNumber(section));
            else if (role == Qt::SizeHintRole)
                return QSize(frameWidth(), 0);
        } else
        {
            if (section == 0 && role == Qt::DisplayRole)
                return "Layer";

            if (section == 1)
            {
                if (role == Qt::DecorationRole)
                    return SmallIcon("layer-visible-on");
                else if (role == Qt::DisplayRole)
                    return "";
            }

            if (section == 2)
            {
                if (role == Qt::DecorationRole)
                    return SmallIcon("dialog-ok-apply");
                else if (role == Qt::DisplayRole)
                    return "";
            }
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}


int AnimatorModel::frameWidth() const
{
    return m_frameWidth;
}

void AnimatorModel::setFrameWidth(int width)
{
    m_frameWidth = width;
    emit layoutChanged();
}

bool AnimatorModel::showThumbs() const
{
    return m_showThumbs;
}

void AnimatorModel::setShowThumbs(bool val)
{
    m_showThumbs = val;
    emit layoutChanged();
}


QVariant AnimatorModel::data(const QModelIndex& ind, int role) const
{
    // layers, not frames
    if (ind.column() == 0) {
        KisNodeSP node = nodeFromIndex(ind);
        if (role == Qt::DisplayRole) {
            AnimatedLayerSP alayer = qobject_cast<AnimatedLayer*>(node.data());
            if (alayer)
                return alayer->animationName();
            return node ? node->name() : "nonode";
        }
        return QVariant();
    }
    else if (ind.column() == 1) {
        if (role == Qt::CheckStateRole) {
            KisNodeSP node = nodeFromIndex(index(ind.row(), 0, ind.parent()));
            return (node && node->visible())?Qt::Checked:Qt::Unchecked;
        }
    }
    else if (ind.column() == 2) {
        if (role == Qt::CheckStateRole) {
            KisNodeSP node = nodeFromIndex(index(ind.row(), 0, ind.parent()));
            AnimatedLayerSP layer = qobject_cast<AnimatedLayer*>(node.data());
            if (!layer) {
                return (node->visible()) ? Qt::Checked : Qt::Unchecked;
            }
            else {
                return (layer->enabled()) ? Qt::Checked : Qt::Unchecked;
            }
        }
    }
    else {                     // frames
        AnimatedLayerSP alayer = qobject_cast<AnimatedLayer*>(nodeFromIndex(index(ind.row(), 0, ind.parent())).data());
        FrameLayer* frame = 0;
        if (alayer) {
            FramedAnimatedLayerSP flayer = qobject_cast<FramedAnimatedLayer*>(alayer.data());
            if (flayer) {
                frame = flayer->frameAt(frameNumber(ind));
                if (frame) {
                    SimpleFrameLayer* sframe = qobject_cast<SimpleFrameLayer*>(frame);
                    if (flayer->hasPreview() && showThumbs()) {
                        if (role == Qt::DecorationRole) {
                            if (sframe && sframe->getContent()) {
                                KisNodeSP node = sframe->getContent();
                                QImage thumb = node->createThumbnail(frameWidth(), frameWidth());
                                if (!thumb.isNull()) {
                                    return thumb;
                                }
                            }
                        }
                    }
                    else {
                        if (role == Qt::DisplayRole) {
                            return "x";
                        }
                    }
                }
            }

            ViewAnimatedLayerSP vlayer = qobject_cast<ViewAnimatedLayer*>(alayer.data());
            if (vlayer) {
                if (role == Qt::DecorationRole && vlayer->enabled()) {
                    return vlayer->getThumbnail(frameNumber(ind), frameWidth());
                }
            }
        }
        if (role == Qt::BackgroundRole) {
            int curFrame = AnimatorManagerFactory::instance()->getManager(image())->getSwitcher()->currentFrame();
            if (frameNumber(ind) == curFrame) {
                return QBrush(QColor(127, 127, 127));
            }
        }
    }

    return QVariant();
}

bool AnimatorModel::setData(const QModelIndex& ind, const QVariant& value, int role)
{
    if (role == Qt::CheckStateRole) {
        if (ind.column() == 1) {
            KisNodeSP node = nodeFromIndex(index(ind.row(), 0, parent(ind)));
            if (node) {
                node->setVisible(value == Qt::Checked);
                node->setDirty();
            }
        } else if (ind.column() == 2) {
            AnimatedLayer *layer = qobject_cast<AnimatedLayer*>(nodeFromIndex(index(ind.row(), 0, parent(ind))).data());
            if (layer) {
                layer->setEnabled(value == Qt::Checked);
            }
        }
        return true;
    }
    return false;
}

Qt::ItemFlags AnimatorModel::flags(const QModelIndex &ind) const
{
    if (ind.column() == 1 || ind.column() == 2) {
        return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
    }
    return QAbstractItemModel::flags(ind);
}

int AnimatorModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    int fnum = AnimatorManagerFactory::instance()->getManager(m_image)->framesNumber();
    fnum += 12;
    return BASE_COLUMNS_NUMBER + fnum;
}

int AnimatorModel::rowCount(const QModelIndex& parent) const
{
    KisNodeSP pnode = nodeFromIndex(parent);

    if (!parent.isValid() || (parent.column() == 0 && pnode && !qobject_cast< AnimatedLayer*>(pnode.data()))) {
        return pnode->childCount();
    }
    return 0;
}

QModelIndex AnimatorModel::parent(const QModelIndex& child) const
{
    KisNode *pnode = reinterpret_cast<KisNode*>(child.internalPointer());
    return indexFromNode(pnode);
}

QModelIndex AnimatorModel::index(int row, int column, const QModelIndex& parent) const
{
    KisNodeSP pnode = nodeFromIndex(parent);
    return createIndex(row, column, pnode.data());
}


int AnimatorModel::frameNumber(const QModelIndex& index) const
{
    return frameNumber(index.column());
}

int AnimatorModel::frameNumber(int column) const
{
    return column - BASE_COLUMNS_NUMBER;
}


KisNodeSP AnimatorModel::nodeFromIndex(const QModelIndex& index) const
{
    if (!index.isValid())
        return m_image->root();
    KisNodeSP parent = reinterpret_cast<KisNode*>(index.internalPointer());
    if (!parent)
        return 0;
    KisNodeSP node = parent->at(parent->childCount()-index.row()-1);
    if (index.column() != 0) {
        if (node->inherits("FramedAnimatedLayer")) {
            FramedAnimatedLayer *al = qobject_cast<FramedAnimatedLayer*>(node.data());
            node = al->frameAt(frameNumber(index));
        }
        else {
            return 0;
        }
    }
    return node;
}

QModelIndex AnimatorModel::indexFromNode(const KisNodeSP node) const
{
    if (!node)
        return QModelIndex();
    KisNodeSP pnode = node->parent().data();
    if (!pnode)
        return QModelIndex();
    int row = pnode->childCount() - pnode->index(node) - 1;
    return createIndex(row, 0, (void*)pnode.data());
}
