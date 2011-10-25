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

#ifndef ANIMATOR_MODEL_H
#define ANIMATOR_MODEL_H

#include <QModelIndex>
#include <kis_image.h>
#include <kis_node_model.h>


class AnimatorModel : public QAbstractItemModel
{
    Q_OBJECT
    
public:
    AnimatorModel(KisImage* image);
    virtual ~AnimatorModel();
    
public slots:
    virtual void removeThis();
    
    virtual void layoutChangedSlot();
    virtual void dataChangedSlot(KisNode* node);
    
public:
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& child) const;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    
public:
    virtual int frameWidth() const;
    virtual void setFrameWidth(int width);
    
public:
    virtual KisImage* image() const;
    
public:
    virtual int frameNumber(const QModelIndex& index) const;
    virtual int frameNumber(int column) const;
    
    virtual KisNode* nodeFromIndex(const QModelIndex& index) const;
    
protected:
    virtual QModelIndex indexFromNode(const KisNode* node) const;
    
private:
    int m_frameWidth;
    
private:
    KisImage* m_image;
};

#endif // ANIMATOR_MODEL_H
