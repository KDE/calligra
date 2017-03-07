/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Shantanu Tushar <shantanu@kde.org>
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

#include "CQSpreadsheetCanvas.h"

#include <QStyleOptionGraphicsItem>

#include "CQCanvasController.h"
#include <KoPart.h>
#include <KoFindText.h>
#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoZoomController.h>
#include <KoZoomHandler.h>
#include <sheets/part/Doc.h>
#include <sheets/part/CanvasItem.h>
#include <sheets/Map.h>
#include <sheets/Sheet.h>
#include <KoShape.h>
#include <KActionCollection>
#include <QGraphicsWidget>
#include <QTextFrame>
#include <QTextLayout>

class CQSpreadsheetCanvas::Private
{
public:
    Private() : canvas(0), document(0), currentSheet(0) { }

    Calligra::Sheets::CanvasItem* canvas;
    Calligra::Sheets::Doc * document;

    int currentSheet;
    QObjectList linkTargets;

    void updateLinkTargets()
    {
        qDeleteAll(linkTargets);
        linkTargets.clear();

        if (!canvas) {
            return;
        }
        foreach(const KoShape* shape, canvas->activeSheet()->shapes()) {
            if (!shape->hyperLink().isEmpty()) {
                QObject * obj = new QObject(canvas);
                obj->setProperty("linkRect", shape->boundingRect());
                obj->setProperty("linkTarget", QUrl(shape->hyperLink()));
                linkTargets.append(obj);
            }
        }

        QList<QTextDocument*> texts;
        KoFindText::findTextInShapes(canvas->activeSheet()->shapes(), texts);
        foreach(QTextDocument* text, texts) {
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
                            QObject * obj = new QObject(canvas);
                            QRectF rect = getFragmentPosition(block, fragment);
                            obj->setProperty("linkRect", canvas->viewConverter()->documentToView(rect));
                            obj->setProperty("linkTarget", QUrl(format.anchorHref()));
                            linkTargets.append(obj);
                        }
                    }
                }
            }
        }
    }

    QRectF getFragmentPosition(const QTextBlock& block, const QTextFragment& fragment)
    {
        // TODO this only produces a position for the first part, if the link spans more than one line...
        // Need to sort that somehow, unfortunately probably by slapping this code into the above function.
        // For now leave it like this, more important things are needed.
        QTextLayout* layout = block.layout();
        QTextLine line = layout->lineForTextPosition(fragment.position() - block.position());
        if (!line.isValid()) {
            // fragment has no valid position and consequently no line...
            return QRectF();
        }
        qreal top = line.position().y();
        qreal bottom = line.position().y() + line.height();
        qreal left = line.cursorToX(fragment.position() - block.position());
        qreal right = line.cursorToX((fragment.position() - block.position()) + fragment.length());
        QRectF fragmentPosition(QPointF(top, left), QPointF(bottom, right));
        return fragmentPosition.adjusted(layout->position().x(), layout->position().y(), 0, 0);
    }
};

CQSpreadsheetCanvas::CQSpreadsheetCanvas(QDeclarativeItem* parent)
    : CQCanvasBase(parent), d(new Private)
{

}

CQSpreadsheetCanvas::~CQSpreadsheetCanvas()
{
    delete d;
}

int CQSpreadsheetCanvas::currentSheet() const
{
    return d->currentSheet;
}

Calligra::Sheets::Map* CQSpreadsheetCanvas::documentMap() const
{
    return d->document->map();
}

QObjectList CQSpreadsheetCanvas::linkTargets() const
{
    return d->linkTargets;
}

void CQSpreadsheetCanvas::setCurrentSheet(int sheet)
{
    sheet = qBound(0, sheet, d->document->map()->count() - 1);
    if (sheet != d->currentSheet) {
        d->currentSheet = sheet;
        d->canvas->setActiveSheet(d->document->map()->sheet(d->currentSheet));
        emit currentSheetChanged();
        d->updateLinkTargets();
        emit linkTargetsChanged();
    }
}

void CQSpreadsheetCanvas::render(QPainter* painter, const QRectF& target)
{
    QStyleOptionGraphicsItem option;
    option.exposedRect = target;
    option.rect = target.toAlignedRect();
    d->canvas->canvasItem()->paint(painter, &option);
}

void CQSpreadsheetCanvas::openFile(const QString& uri)
{
    KService::Ptr service = KService::serviceByDesktopName("sheetspart");
    if (service.isNull()) {
        qWarning("Unable to load Sheets plugin, aborting!");
        return;
    }

    KoPart* part = service->createInstance<KoPart>();
    d->document = static_cast<Calligra::Sheets::Doc*> (part->document());
    d->document->setAutoSave(0);
    d->document->setCheckAutoSaveFile(false);
    d->document->openUrl (QUrl (uri));

    d->canvas = dynamic_cast<Calligra::Sheets::CanvasItem*> (part->canvasItem(part->document()));
    createAndSetCanvasControllerOn(d->canvas);
    createAndSetZoomController(d->canvas);

    d->canvas->setParentItem(this);
    d->canvas->installEventFilter(this);
    d->canvas->setVisible(true);
    d->canvas->setGeometry(x(), y(), width(), height());

    d->updateLinkTargets();
    emit linkTargetsChanged();

    Calligra::Sheets::Sheet *sheet = d->document->map()->sheet(0);
    if (sheet) {
        updateDocumentSize(sheet->documentSize().toSize());
    }
}

void CQSpreadsheetCanvas::createAndSetCanvasControllerOn(KoCanvasBase* canvas)
{
    //TODO: pass a proper action collection
    CQCanvasController *controller = new CQCanvasController(new KActionCollection(this));
    setCanvasController(controller);
    controller->setCanvas(canvas);
    KoToolManager::instance()->addController (controller);
}

void CQSpreadsheetCanvas::createAndSetZoomController(KoCanvasBase* canvas)
{
    KoZoomHandler* zoomHandler = static_cast<KoZoomHandler*> (canvas->viewConverter());
    setZoomController(new KoZoomController(canvasController(),
                                           zoomHandler,
                                           new KActionCollection(this)));

    Calligra::Sheets::CanvasItem *canvasItem = dynamic_cast<Calligra::Sheets::CanvasItem*> (canvas);
    // update the canvas whenever we scroll, the canvas controller must emit this signal on scrolling/panning
    connect (canvasController()->proxyObject, SIGNAL(moveDocumentOffset(QPoint)), canvasItem, SLOT(setDocumentOffset(QPoint)));
    // whenever the size of the document viewed in the canvas changes, inform the zoom controller
    connect(canvasItem, SIGNAL(documentSizeChanged(QSize)), SLOT(updateDocumentSize(QSize)));
    canvasItem->update();
}

void CQSpreadsheetCanvas::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (d->canvas) {
        d->canvas->setGeometry(newGeometry);
    }
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

void CQSpreadsheetCanvas::updateDocumentSize(const QSize& size)
{
    zoomController()->setDocumentSize(size, false);
}
