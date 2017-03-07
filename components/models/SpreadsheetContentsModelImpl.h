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

#ifndef CALLIGRA_COMPONENTS_SPREADSHEETCONTENTSMODELIMPL_H
#define CALLIGRA_COMPONENTS_SPREADSHEETCONTENTSMODELIMPL_H

#include "ContentsModelImpl.h"

class KoDocument;
namespace Calligra {
namespace Components {

class SpreadsheetContentsModelImpl : public ContentsModelImpl
{
public:
    explicit SpreadsheetContentsModelImpl(KoDocument* document);
    ~SpreadsheetContentsModelImpl();

    virtual int rowCount() const Q_DECL_OVERRIDE;
    virtual QVariant data(int index, Calligra::Components::ContentsModel::Role role) const Q_DECL_OVERRIDE;
    virtual void setThumbnailSize(const QSize& size) Q_DECL_OVERRIDE;
    virtual QImage thumbnail(int index, int width) const Q_DECL_OVERRIDE;

private:
    class Private;
    Private* const d;
};
}
}

#endif // CALLIGRA_COMPONENTS_SPREADSHEETCONTENTSMODELIMPL_H
