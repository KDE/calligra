/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "SpreadsheetThumbnailModelImpl.h"

using namespace Calligra::Components;

class SpreadsheetThumbnailModelImpl::Private
{
public:
    Private()
    { }

};

SpreadsheetThumbnailModelImpl::SpreadsheetThumbnailModelImpl()
    : d{new Private}
{

}

SpreadsheetThumbnailModelImpl::~SpreadsheetThumbnailModelImpl()
{
    delete d;
}

QVariant SpreadsheetThumbnailModelImpl::data(int index, Calligra::Components::ThumbnailModel::Role role) const
{
    return QVariant();
}

int SpreadsheetThumbnailModelImpl::rowCount() const
{
    return 0;
}
