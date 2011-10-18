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
    connect(image, SIGNAL(sigAboutToBeDeleted()), this, SLOT(removeThis()));
}

AnimatorModel::~AnimatorModel()
{
    std::cout << "~AnimatorModel(" << (void*) this << ")" << std::endl;
}

void AnimatorModel::removeThis()
{
    delete this;
}


QVariant AnimatorModel::data(const QModelIndex& index, int role) const
{
    return QVariant();
}

int AnimatorModel::columnCount(const QModelIndex& parent) const
{
    return 0;
}

int AnimatorModel::rowCount(const QModelIndex& parent) const
{
    return 0;
}

QModelIndex AnimatorModel::parent(const QModelIndex& child) const
{
    return QModelIndex();
}

QModelIndex AnimatorModel::index(int row, int column, const QModelIndex& parent) const
{
    return QModelIndex();
}
