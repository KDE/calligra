/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#include "ContentsModel.h"

#include <QGraphicsWidget>
#include <QSize>

#include <KoCanvasBase.h>

#include "Document.h"
#include "PresentationContentsModelImpl.h"
#include "SpreadsheetContentsModelImpl.h"
#include "TextContentsModelImpl.h"

using namespace Calligra::Components;

class ContentsModel::Private
{
public:
    Private()
        : useToC{false}
        , impl{nullptr}
        , document{nullptr}
        , thumbnailSize{128, 128}
    {
    }

    bool useToC;
    ContentsModelImpl *impl;
    Document *document;
    QSize thumbnailSize;
};

ContentsModel::ContentsModel(QObject *parent)
    : QAbstractListModel{parent}
    , d{new Private}
{
}

ContentsModel::~ContentsModel()
{
    delete d;
}

QVariant ContentsModel::data(const QModelIndex &index, int role) const
{
    if (!d->impl || !index.isValid())
        return QVariant();

    return d->impl->data(index.row(), static_cast<Role>(role));
}

int ContentsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (d->impl) {
        return d->impl->rowCount();
    }

    return 0;
}

Document *ContentsModel::document() const
{
    return d->document;
}

void ContentsModel::setDocument(Document *newDocument)
{
    if (newDocument != d->document) {
        if (d->document) {
            disconnect(d->document, &Document::statusChanged, this, &ContentsModel::updateImpl);
        }

        d->document = newDocument;
        connect(d->document, &Document::statusChanged, this, &ContentsModel::updateImpl);

        updateImpl();

        Q_EMIT documentChanged();
    }
}

QSize ContentsModel::thumbnailSize() const
{
    return d->thumbnailSize;
}

void ContentsModel::setThumbnailSize(const QSize &newValue)
{
    if (newValue != d->thumbnailSize) {
        d->thumbnailSize = newValue;

        if (d->impl) {
            d->impl->setThumbnailSize(newValue);
            Q_EMIT dataChanged(index(0), index(d->impl->rowCount() - 1), QVector<int>{} << ThumbnailRole);
        }

        Q_EMIT thumbnailSizeChanged();
    }
}

void ContentsModel::setUseToC(bool newValue)
{
    beginResetModel();
    if (d->impl)
        d->impl->setUseToC(newValue);
    Q_EMIT useToCChanged();
    endResetModel();
}

bool ContentsModel::useToC() const
{
    return d->useToC;
}

QImage ContentsModel::thumbnail(int index, int width) const
{
    if (!d->impl)
        return QImage{};

    if (index < 0 || index >= d->impl->rowCount())
        return QImage{};

    return d->impl->thumbnail(index, width);
}

void ContentsModel::updateImpl()
{
    beginResetModel();
    delete d->impl;
    d->impl = nullptr;

    if (d->document && d->document->status() == DocumentStatus::Loaded) {
        switch (d->document->documentType()) {
        case DocumentType::TextDocument: {
            auto textImpl = new TextContentsModelImpl{d->document->koDocument(), dynamic_cast<KoCanvasBase *>(d->document->canvas())};
            d->impl = textImpl;
            connect(textImpl, &TextContentsModelImpl::listContentsCompleted, this, &ContentsModel::reset);
            break;
        }
        case DocumentType::Spreadsheet:
            d->impl = new SpreadsheetContentsModelImpl{d->document->koDocument()};
            break;
        case DocumentType::Presentation:
            d->impl = new PresentationContentsModelImpl{d->document->koDocument()};
            break;
        default:
            break;
        }
    }

    if (d->impl) {
        d->impl->setThumbnailSize(d->thumbnailSize);
        d->impl->setUseToC(d->useToC);
    }

    endResetModel();
}

void ContentsModel::reset()
{
    beginResetModel();
    endResetModel();
}

QHash<int, QByteArray> ContentsModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(TitleRole, "title");
    roleNames.insert(LevelRole, "level");
    roleNames.insert(ThumbnailRole, "thumbnail");
    roleNames.insert(ContentIndexRole, "contentIndex");
    return roleNames;
}
