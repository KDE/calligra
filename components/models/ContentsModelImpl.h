/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef CALLIGRA_COMPONENTS_CONTENTSMODELIMPL_H
#define CALLIGRA_COMPONENTS_CONTENTSMODELIMPL_H

#include "ContentsModel.h"

class QImage;
class QVariant;
namespace Calligra {
namespace Components {

/**
 * \brief Defines an interface for handling specific document types in ThumbnailModel.
 *
 */

class ContentsModelImpl
{
public:
    ContentsModelImpl() { }
    virtual ~ContentsModelImpl() { }

    virtual int rowCount() const = 0;
    virtual QVariant data(int index, ContentsModel::Role role) const = 0;
    virtual void setThumbnailSize(const QSize& size) = 0;

    virtual QImage thumbnail(int index, int width) const = 0;

    virtual void setUseToC(bool newValue) { Q_UNUSED(newValue); }
};

} // Namespace Components
} // Namespace Calligra

#endif // CALLIGRA_COMPONENTS_THUMBNAILMODELIMPL_H
