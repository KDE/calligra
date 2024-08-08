/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#include "CQTextToCModel.h"
#include "CQTextDocumentCanvas.h"

#include <KWDocument.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutRootArea.h>
#include <KoTextPage.h>
#include <frames/KWTextFrameSet.h>
#include <styles/KoParagraphStyle.h>

#include <QTextCursor>
#include <QTextDocument>
#include <QTextObject>
#include <QTimer>

using namespace std::chrono;

Q_DECLARE_METATYPE(QTextDocument *)

struct TextToCModelEntry {
    TextToCModelEntry()
        : level(0)
        , pageNumber(0)
    {
    }
    QString title;
    int level;
    int pageNumber;
};

class CQTextToCModel::Private
{
public:
    Private()
        : canvas(0)
        , document(0)
        , documentLayout(0)
    {
    }

    QList<TextToCModelEntry *> entries;

    CQTextDocumentCanvas *canvas;
    QTextDocument *document;
    KoTextDocumentLayout *documentLayout;

    QTimer updateTimer;
    QTimer doneTimer;

    int resolvePageNumber(const QTextBlock &headingBlock)
    {
        KoTextDocumentLayout *layout = qobject_cast<KoTextDocumentLayout *>(document->documentLayout());
        KoTextLayoutRootArea *rootArea = layout->rootAreaForPosition(headingBlock.position());
        if (rootArea) {
            if (rootArea->page()) {
                return rootArea->page()->visiblePageNumber();
            } else {
                // had root but no page;
            }
        }
        return 0;
    }
};

CQTextToCModel::CQTextToCModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
    connect(&d->updateTimer, &QTimer::timeout, this, &CQTextToCModel::timeout);
    d->updateTimer.setInterval(5s);
    d->updateTimer.setSingleShot(true);

    connect(&d->doneTimer, &QTimer::timeout, this, &CQTextToCModel::updateToC);
    d->doneTimer.setInterval(1s);
    d->doneTimer.setSingleShot(true);
}

CQTextToCModel::~CQTextToCModel() = delete;

QHash<int, QByteArray> CQTextToCModel::roleNames() const
{
    return {
        {Title, "title"},
        {Level, "level"},
        {PageNumber, "pageNumber"},
    };
}

QVariant CQTextToCModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if (index.isValid()) {
        int row = index.row();
        if (row > -1 && row < d->entries.count()) {
            const TextToCModelEntry *entry = d->entries.at(row);
            switch (role) {
            case PageNumber:
                result.setValue<int>(entry->pageNumber);
                break;
            case Level:
                result.setValue<int>(entry->level);
                break;
            case Title:
            default:
                // Allowing the fallthrough here (explicitly mentioning our own Title entry)
                // means that the predefined Qt Quick roles are also allowed to be filled
                // with useful data
                result.setValue<QString>(entry->title);
                break;
            }
        }
    }
    return result;
}

int CQTextToCModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return d->entries.count();
}

void CQTextToCModel::requestGeneration()
{
    if (d->document->characterCount() < 2) {
        return;
    }
    d->updateTimer.stop();
    d->updateTimer.start();
}

void CQTextToCModel::startDoneTimer()
{
    // we delay acting on the finishedLayout signal by 1 second. This way we
    //  don't act on it until every header has had a chance to be layouted
    //  in words (we assume that a new finishedLayout signal will arrive within that
    //  1 second)
    d->doneTimer.start();
}

void CQTextToCModel::timeout()
{
    d->updateTimer.stop();
    d->documentLayout->scheduleLayout();
}

void CQTextToCModel::updateToC()
{
    beginResetModel();
    QTextBlock block = d->document->firstBlock();
    qDeleteAll(d->entries.begin(), d->entries.end());
    d->entries.clear();

    while (block.isValid()) {
        QTextBlockFormat format = block.blockFormat();
        if (format.hasProperty(KoParagraphStyle::OutlineLevel)) {
            TextToCModelEntry *entry = new TextToCModelEntry();
            entry->title = block.text();
            entry->level = format.intProperty(KoParagraphStyle::OutlineLevel);
            entry->pageNumber = d->resolvePageNumber(block);
            d->entries.append(entry);
        }
        block = block.next();
    }
    endResetModel();
}

QObject *CQTextToCModel::canvas() const
{
    return d->canvas;
}

void CQTextToCModel::setCanvas(QObject *newCanvas)
{
    beginResetModel();
    if (d->documentLayout) {
        d->documentLayout->disconnect(this);
    }
    d->canvas = 0;
    d->document = 0;
    d->documentLayout = 0;
    CQTextDocumentCanvas *canvas = qobject_cast<CQTextDocumentCanvas *>(newCanvas);
    if (canvas) {
        d->canvas = canvas;
        d->document = canvas->document()->mainFrameSet()->document();
        d->documentLayout = static_cast<KoTextDocumentLayout *>(d->document->documentLayout());

        // connect to layoutIsDirty
        connect(d->documentLayout, SIGNAL(layoutIsDirty()), this, SLOT(requestGeneration()));

        // connect to FinishedLayout
        connect(d->documentLayout, SIGNAL(finishedLayout()), this, SLOT(startDoneTimer()));
    }
    Q_EMIT canvasChanged();
    endResetModel();
}
