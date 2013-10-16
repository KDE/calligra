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

#include "ThumbnailModel.h"

#include "Document.h"
#include "Global.h"

#include "TextDocumentThumbnailModelImpl.h"
#include "PresentationThumbnailModelImpl.h"
#include "SpreadsheetThumbnailModelImpl.h"

using namespace Calligra::Components;

class ThumbnailModel::Private
{
public:
    Private() : document{nullptr}, impl{nullptr}
    { }

    void updateImpl();

    Document* document;
    ThumbnailModelImpl* impl;
};

ThumbnailModel::ThumbnailModel(QObject* parent)
    : QAbstractListModel{parent}, d{new Private}
{

}

ThumbnailModel::~ThumbnailModel()
{
    delete d;
}

QVariant ThumbnailModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid() || !roleNames().contains(role) || !d->impl) {
        return QVariant();
    }

    return d->impl->data(index.row(), static_cast<ThumbnailModel::Role>(role));
}

int ThumbnailModel::rowCount(const QModelIndex& parent) const
{
    if(d->impl && !parent.isValid()) {
        return d->impl->rowCount();
    }

    return 0;
}

Document* ThumbnailModel::document() const
{
    return d->document;
}

void ThumbnailModel::setDocument(Document* newDocument)
{
    if(newDocument != d->document) {
        d->document = newDocument;
        emit documentChanged();

        d->updateImpl();
    }
}

void ThumbnailModel::Private::updateImpl()
{
    if(impl) {
        delete impl;
    }

    if(document) {
        switch(document->documentType()) {
            case DocumentType::TextDocument:
                impl = new TextDocumentThumbnailModelImpl;
                break;
            case DocumentType::Spreadsheet:
                impl = new SpreadsheetThumbnailModelImpl;
                break;
            case DocumentType::Presentation:
                impl = new PresentationThumbnailModelImpl;
                break;
            default:
                impl = nullptr;
                break;
        }
    } else {
        impl = nullptr;
    }
}
