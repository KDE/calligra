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

#include "TextDocumentThumbnailModelImpl.h"

using namespace Calligra::Components;

class TextDocumentThumbnailModelImpl::Private
{
public:
    Private()
    { }

};

TextDocumentThumbnailModelImpl::TextDocumentThumbnailModelImpl()
    : d{new Private}
{

}

TextDocumentThumbnailModelImpl::~TextDocumentThumbnailModelImpl()
{
    delete d;
}

QVariant TextDocumentThumbnailModelImpl::data(int index, Calligra::Components::ThumbnailModel::Role role) const
{
    return QVariant();
}

int TextDocumentThumbnailModelImpl::rowCount() const
{
    return 0;
}
