/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef CALLIGRA_COMPONENTS_CONTENTSMODELIMPL_H
#define CALLIGRA_COMPONENTS_CONTENTSMODELIMPL_H

#include "ContentsModel.h"

class QImage;
class QVariant;
namespace Calligra
{
namespace Components
{

/**
 * \brief Defines an interface for handling specific document types in ThumbnailModel.
 *
 */

class ContentsModelImpl
{
public:
    ContentsModelImpl() = default;
    virtual ~ContentsModelImpl() = default;

    virtual int rowCount() const = 0;
    virtual QVariant data(int index, ContentsModel::Role role) const = 0;
    virtual void setThumbnailSize(const QSize &size) = 0;

    virtual QImage thumbnail(int index, int width) const = 0;

    virtual void setUseToC(bool newValue)
    {
        Q_UNUSED(newValue);
    }
};

} // Namespace Components
} // Namespace Calligra

#endif // CALLIGRA_COMPONENTS_THUMBNAILMODELIMPL_H
