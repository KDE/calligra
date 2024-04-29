/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#include "SpreadsheetImpl.h"

#include <QGraphicsWidget>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextLayout>

#include <KoToolManager.h>

#include <KoFindText.h>
#include <KoShape.h>
#include <KoShapeContainer.h>
#include <KoViewConverter.h>
#include <KoZoomController.h>
#include <libs/textlayout/KoTextShapeData.h>
#include <sheets/core/Map.h>
#include <sheets/core/Sheet.h>
#include <sheets/engine/Damages.h>
#include <sheets/part/CanvasItem.h>
#include <sheets/part/Doc.h>
#include <sheets/part/Part.h>

using namespace Calligra::Components;

class SpreadsheetImpl::Private
{
public:
    Private()
        : part{nullptr}
        , document{nullptr}
    {
    }

    Calligra::Sheets::Part *part;
    Calligra::Sheets::Doc *document;
    Calligra::Sheets::CanvasItem *canvas;
    int currentSheet;

    QList<QPair<QRectF, QUrl>> links;

    QList<KoShape *> deepShapeFind(const QList<KoShape *> &shapes)
    {
        QList<KoShape *> allShapes;
        for (KoShape *shape : shapes) {
            allShapes.append(shape);
            KoShapeContainer *container = dynamic_cast<KoShapeContainer *>(shape);
            if (container) {
                allShapes.append(deepShapeFind(container->shapes()));
            }
        }
        return allShapes;
    }

    void updateLinkTargets()
    {
        links.clear();

        if (!canvas || !canvas->activeSheet())
            return;
        const auto shapes = canvas->activeSheet()->shapes();
        for (const KoShape *shape : shapes) {
            if (!shape->hyperLink().isEmpty()) {
                QRectF rect = shape->boundingRect();
                for (KoShapeContainer *parent = shape->parent(); parent; parent = parent->parent()) {
                    rect.translate(parent->position());
                }
                links.append(QPair<QRectF, QUrl>(rect, QUrl(shape->hyperLink())));
            }
        }

        QList<QTextDocument *> texts;
        KoFindText::findTextInShapes(canvas->activeSheet()->shapes(), texts);
        QList<KoShape *> allShapes = deepShapeFind(canvas->activeSheet()->shapes());
        for (QTextDocument *text : std::as_const(texts)) {
            QTextBlock block = text->rootFrame()->firstCursorPosition().block();
            for (; block.isValid(); block = block.next()) {
                block.begin();
                QTextBlock::iterator it;
                for (it = block.begin(); !(it.atEnd()); ++it) {
                    QTextFragment fragment = it.fragment();
                    if (fragment.isValid()) {
                        QTextCharFormat format = fragment.charFormat();
                        if (format.isAnchor()) {
                            // This is an anchor, store target and position...
                            QRectF rect = getFragmentPosition(block, fragment);
                            for (KoShape *shape : std::as_const(allShapes)) {
                                KoTextShapeData *shapeData = dynamic_cast<KoTextShapeData *>(shape->userData());
                                if (!shapeData)
                                    continue;
                                if (shapeData->document() == text) {
                                    rect.translate(shape->position());
                                    for (KoShapeContainer *parent = shape->parent(); parent; parent = parent->parent()) {
                                        rect.translate(parent->position());
                                    }
                                    break;
                                }
                            }
                            links.append(QPair<QRectF, QUrl>(canvas->viewConverter()->documentToView(rect), QUrl(format.anchorHref())));
                        }
                    }
                }
            }
        }
    }

    QRectF getFragmentPosition(QTextBlock block, QTextFragment fragment)
    {
        // TODO this only produces a position for the first part, if the link spans more than one line...
        // Need to sort that somehow, unfortunately probably by slapping this code into the above function.
        // For now leave it like this, more important things are needed.
        QTextLayout *layout = block.layout();
        QTextLine line = layout->lineForTextPosition(fragment.position() - block.position());
        if (!line.isValid()) {
            // fragment has no valid position and consequently no line...
            return QRectF();
        }
        qreal top = line.position().y() + (line.height() / 2);
        qreal bottom = top + line.height();
        qreal left = line.cursorToX(fragment.position() - block.position());
        qreal right = line.cursorToX((fragment.position() - block.position()) + fragment.length());
        QRectF fragmentPosition(QPointF(left, top), QPointF(right, bottom));
        return fragmentPosition.adjusted(layout->position().x(), layout->position().y(), 0, 0);
    }

    static const float wiggleFactor;
};

const float Calligra::Components::SpreadsheetImpl::Private::wiggleFactor{4.f};

SpreadsheetImpl::SpreadsheetImpl(QObject *parent)
    : DocumentImpl{parent}
    , d{new Private}
{
    setDocumentType(DocumentType::Spreadsheet);
}

SpreadsheetImpl::~SpreadsheetImpl()
{
    delete d;
}

bool SpreadsheetImpl::load(const QUrl &url)
{
    delete d->part;
    delete d->document;

    d->part = new Calligra::Sheets::Part{this};
    d->document = new Calligra::Sheets::Doc{d->part};
    setKoDocument(d->document);
    d->part->setDocument(d->document);

    bool retval = d->document->openUrl(url);

    d->canvas = static_cast<Calligra::Sheets::CanvasItem *>(d->part->canvasItem(d->document));

    createAndSetCanvasController(d->canvas);
    createAndSetZoomController(d->canvas);
    connect(d->canvas, &Calligra::Sheets::CanvasItem::documentSizeChanged, this, &SpreadsheetImpl::updateDocumentSize);

    Calligra::Sheets::SheetBase *bsheet = d->document->map()->sheet(0);
    Calligra::Sheets::Sheet *sheet = dynamic_cast<Calligra::Sheets::Sheet *>(bsheet);
    if (sheet) {
        updateDocumentSize(sheet->documentSize().toSize());
    }

    setCanvas(d->canvas);

    d->updateLinkTargets();

    return retval;
}

int SpreadsheetImpl::currentIndex()
{
    if (d->document && d->document->map() && d->canvas->activeSheet()) {
        return d->document->map()->indexOf(d->canvas->activeSheet());
    } else {
        return -1;
    }
}

void SpreadsheetImpl::setCurrentIndex(int newValue)
{
    if (newValue != currentIndex()) {
        Calligra::Sheets::SheetBase *bsheet = d->document->map()->sheet(newValue);
        Calligra::Sheets::Sheet *sheet = dynamic_cast<Calligra::Sheets::Sheet *>(bsheet);
        d->canvas->setActiveSheet(sheet);
        d->updateLinkTargets();
        Q_EMIT currentIndexChanged();
    }
}

void SpreadsheetImpl::updateDocumentSize(const QSize &size)
{
    QRectF activeRect =
        d->canvas->viewConverter()->documentToView(d->canvas->activeSheet()->cellCoordinatesToDocument(d->canvas->activeSheet()->usedArea(true)));
    zoomController()->setDocumentSize(activeRect.size(), false);
    setDocumentSize(activeRect.size().toSize());
}

int SpreadsheetImpl::indexCount() const
{
    return d->document->map()->count();
}

QUrl SpreadsheetImpl::urlAtPoint(QPoint point)
{
    for (const QPair<QRectF, QUrl> &link : d->links) {
        QRectF hitTarget{link.first.x() - Private::wiggleFactor,
                         link.first.y() - Private::wiggleFactor,
                         link.first.width() + Private::wiggleFactor * 2,
                         link.first.height() + Private::wiggleFactor * 2};

        if (hitTarget.contains(point)) {
            return link.second;
        }
    }
    return QUrl();
}

QObject *SpreadsheetImpl::part() const
{
    return d->part;
}
