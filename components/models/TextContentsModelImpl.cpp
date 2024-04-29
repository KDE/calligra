/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#include "TextContentsModelImpl.h"

#include <KWDocument.h>
#include <KWPage.h>
#include <KoCanvasBase.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutRootArea.h>
#include <frames/KWTextFrameSet.h>
#include <styles/KoParagraphStyle.h>

using namespace Calligra::Components;

struct ContentsEntry {
    ContentsEntry()
        : level{0}
        , pageNumber{0}
        , page{nullptr}
    {
    }
    QString title;
    int level;
    int pageNumber;
    KWPage *page;
};

class TextContentsModelImpl::Private
{
public:
    Private()
        : useToC{false}
        , document{nullptr}
        , textDocument{nullptr}
        , layout{nullptr}
        , canvas{nullptr}
    {
    }

    bool useToC;
    KWDocument *document;
    QTextDocument *textDocument;
    KoTextDocumentLayout *layout;
    KoCanvasBase *canvas;

    QHash<int, QImage> thumbnails;
    QSize thumbnailSize;

    QList<ContentsEntry> entries;
};

TextContentsModelImpl::TextContentsModelImpl(KoDocument *document, KoCanvasBase *canvas)
    : d{new Private}
{
    d->document = qobject_cast<KWDocument *>(document);
    Q_ASSERT(d->document);
    if (d->document->mainFrameSet() && d->document->mainFrameSet()->document()) {
        d->textDocument = d->document->mainFrameSet()->document();
        d->layout = qobject_cast<KoTextDocumentLayout *>(d->textDocument->documentLayout());
        connect(d->layout, &KoTextDocumentLayout::finishedLayout, this, &TextContentsModelImpl::documentLayoutFinished);
        d->layout->scheduleLayout();
    }
    d->canvas = canvas;
}

TextContentsModelImpl::~TextContentsModelImpl()
{
    delete d;
}

int TextContentsModelImpl::rowCount() const
{
    if (d->useToC && d->entries.count() > 0) {
        return d->entries.count();
    }

    return d->document->pageCount();
}

QVariant TextContentsModelImpl::data(int index, Calligra::Components::ContentsModel::Role role) const
{
    if (d->useToC && d->entries.count() > 0) {
        auto entry = d->entries.at(index);
        switch (role) {
        case ContentsModel::TitleRole:
            return entry.title;
        case ContentsModel::LevelRole:
            return entry.level;
        case ContentsModel::ThumbnailRole: {
            if (d->thumbnails.contains(entry.pageNumber)) {
                return d->thumbnails.value(entry.pageNumber);
            }

            if (d->thumbnailSize.isNull()) {
                return QImage{};
            }

            QImage thumb = entry.page->thumbnail(d->thumbnailSize, d->canvas->shapeManager());
            d->thumbnails.insert(entry.pageNumber, thumb);
            return thumb;
        }
        case ContentsModel::ContentIndexRole: {
            return entry.pageNumber - 1;
        }
        default:
            return QVariant();
        }
    }

    // Fallback behaviour when we don't have a ToC
    KWPage page = d->document->pageManager()->page(index + 1);
    if (!page.isValid())
        return QVariant();

    switch (role) {
    case ContentsModel::TitleRole:
        return i18n("Page %1", page.pageNumber());
    case ContentsModel::LevelRole:
        return 0;
    case ContentsModel::ThumbnailRole: {
        if (d->thumbnails.contains(index)) {
            return d->thumbnails.value(index);
        }

        if (d->thumbnailSize.isNull()) {
            return QImage{};
        }

        QImage thumb = page.thumbnail(d->thumbnailSize, d->canvas->shapeManager());
        d->thumbnails.insert(index, thumb);
        return thumb;
    }
    case ContentsModel::ContentIndexRole: {
        return index;
    }
    default:
        return QVariant();
    }
}

void TextContentsModelImpl::setThumbnailSize(const QSize &size)
{
    d->thumbnailSize = size;
    d->thumbnails.clear();
}

QImage TextContentsModelImpl::thumbnail(int index, int width) const
{
    KWPage page = d->document->pageManager()->page(index + 1);
    return page.thumbnail(QSize{width, int((page.height() / page.width()) * width)}, d->canvas->shapeManager());
}

void TextContentsModelImpl::setUseToC(bool newValue)
{
    d->useToC = newValue;
}

void TextContentsModelImpl::documentLayoutFinished()
{
    QTextBlock block = d->textDocument->firstBlock();
    d->entries.clear();

    while (block.isValid()) {
        QTextBlockFormat format = block.blockFormat();
        if (format.hasProperty(KoParagraphStyle::OutlineLevel)) {
            ContentsEntry entry;
            entry.title = block.text();
            entry.level = format.intProperty(KoParagraphStyle::OutlineLevel);

            auto rootArea = d->layout->rootAreaForPosition(block.position());
            if (rootArea) {
                if (rootArea->page()) {
                    entry.pageNumber = rootArea->page()->visiblePageNumber();
                    entry.page = static_cast<KWPage *>(rootArea->page());
                }
            }

            d->entries.append(entry);
        }
        block = block.next();
    }

    Q_EMIT listContentsCompleted();
}
