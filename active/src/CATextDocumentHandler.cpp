/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "CATextDocumentHandler.h"
#include "CADocumentController.h"
#include "CACanvasController.h"
#include "CATextDocumentModel.h"

#include <KWDocument.h>
#include <KWCanvasItem.h>
#include <KoCopyController.h>
#include <KoCanvasController.h>

#include <KoToolManager.h>
#include <KoZoomHandler.h>
#include <KoZoomController.h>
#include <KoFindText.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoPart.h>
#include <KoDocumentEntry.h>

#include <kmimetype.h>
#include <kdebug.h>
#include <kactioncollection.h>
#include <KoSelection.h>

#include <QTextDocument>

class CATextDocumentHandler::Private
{
public:
    Private() {
        document = 0;
        findText = 0;
        taTextDocumentModel = 0;
        copyAction = 0;
    }
    KWDocument* document;
    KWPage currentTextDocPage;
    QString searchString;
    KoFindText* findText;
    CATextDocumentModel *taTextDocumentModel;
    QAction *copyAction;
};

CATextDocumentHandler::CATextDocumentHandler (CADocumentController* documentController)
    : CAAbstractDocumentHandler (documentController)
    , d (new Private())
{
    QList<QTextDocument*> texts;
    d->findText = new KoFindText(this);
    connect (d->findText, SIGNAL(updateCanvas()), SLOT(updateCanvas()));
    connect (d->findText, SIGNAL(matchFound(KoFindMatch)), SLOT(findMatchFound(KoFindMatch)));
    connect (d->findText, SIGNAL(noMatchFound()), SLOT(findNoMatchFound()));
}

CATextDocumentHandler::~CATextDocumentHandler()
{
    delete d;
}

QStringList CATextDocumentHandler::supportedMimetypes()
{
    // keep in sync with textdocument related mimetypes in calligraactive.desktop
    const QStringList supportedTypes =
        QString::fromLatin1("application/vnd.oasis.opendocument.text-master;application/vnd.oasis.opendocument.text;application/vnd.oasis.opendocument.text-template;application/msword;application/rtf;text/plain;application/x-mswrite;application/vnd.openxmlformats-officedocument.wordprocessingml.document;application/vnd.openxmlformats-officedocument.wordprocessingml.template;application/vnd.ms-works;application/vnd.wordperfect;").split(';', QString::SkipEmptyParts);
    return supportedTypes;
}

KoZoomMode::Mode CATextDocumentHandler::preferredZoomMode() const
{
    return KoZoomMode::ZOOM_WIDTH;
}

bool CATextDocumentHandler::openDocument (const QString& uri)
{
    QString error;
    QString mimetype = KMimeType::findByPath (uri)->name();
    KoDocumentEntry documentEntry = KoDocumentEntry::queryByMimeType(mimetype);
    KoPart *part = documentEntry.createKoPart(&error);

    if (!part) {
        kDebug() << "Doc can't be openend" << error;
        return false;
    }

    d->document = static_cast<KWDocument*> (part->document());
    d->document->openUrl (KUrl (uri));

    setCanvas (dynamic_cast<KoCanvasBase*> (part->canvasItem(d->document)));
    KoToolManager::instance()->addController (dynamic_cast<KoCanvasController*> (documentController()->canvasController()));
    KoSelection *sel = canvas()->shapeManager()->selection();

    KoShape *textShape = 0;
    foreach (KoShape *shape, canvas()->shapeManager()->shapes()) {
        if (shape->shapeId() == "TextShapeID") {
            textShape = shape;
        }
    }
    if (textShape) {
        sel->select(textShape);
        KoToolManager::instance()->switchToolRequested("TextToolFactory_ID");
    }

    KWCanvasItem* kwCanvasItem = dynamic_cast<KWCanvasItem*> (canvas());

    if (!kwCanvasItem) {
        kDebug() << "Failed to get KWCanvasItem";
        return false;
    }

    KoZoomHandler* zoomHandler = static_cast<KoZoomHandler*> (kwCanvasItem->viewConverter());
    documentController()->canvasController()->setZoomHandler (zoomHandler);
    KoZoomController* zoomController = documentController()->canvasController()->zoomController();
    d->currentTextDocPage = d->document->pageManager()->begin();
    zoomController->setPageSize (d->currentTextDocPage.rect().size());
    zoomController->setZoom (KoZoomMode::ZOOM_CONSTANT, 1.0);

    if (kwCanvasItem) {
        // whenever the size of the document viewed in the canvas changes, inform the zoom controller
        connect (kwCanvasItem, SIGNAL(documentSize(QSizeF)), zoomController, SLOT(setDocumentSize(QSizeF)));
        // update the canvas whenever we scroll, the canvas controller must emit this signal on scrolling/panning
        connect (documentController()->canvasController()->canvasControllerProxyObject(), SIGNAL(moveDocumentOffset(QPoint)),
                 kwCanvasItem, SLOT(setDocumentOffset(QPoint)));
        kwCanvasItem->updateSize();
        kDebug() << "HANDLEEEE " << kwCanvasItem->geometry();
    }

    connect (documentController()->canvasController(), SIGNAL(needsCanvasResize(QSizeF)), SLOT(resizeCanvas(QSizeF)));
    connect (documentController()->canvasController(), SIGNAL(needCanvasUpdate()), SLOT(updateCanvas()));

    documentController()->canvasController()->zoomToFit();

    QList<QTextDocument*> texts;
    KoFindText::findTextInShapes(kwCanvasItem->shapeManager()->shapes(), texts);
    d->findText->setDocuments(texts);

    d->copyAction = new QAction(i18n("Copy"), this);
    new KoCopyController(canvas(), d->copyAction);

    d->taTextDocumentModel = new CATextDocumentModel(this, d->document, canvas()->shapeManager());

    return true;
}

KoDocument* CATextDocumentHandler::document()
{
    return d->document;
}

void CATextDocumentHandler::updateCanvas()
{
    KWCanvasItem* kwCanvasItem = dynamic_cast<KWCanvasItem*> (canvas());
    kwCanvasItem->update();
}

QString CATextDocumentHandler::documentTypeName()
{
    return "textdocument";
}

void CATextDocumentHandler::resizeCanvas (const QSizeF& canvasSize)
{
    qreal width = documentController()->canvasController()->width();
    qreal height = documentController()->canvasController()->height();
    KWPage currentPage = d->document->pageManager()->page (qreal (documentController()->canvasController()->cameraY()));
    if (currentPage.isValid()) {
        documentController()->canvasController()->zoomHandler()->setZoom (canvasSize.width() / currentPage.width() * 0.75);
    }
    canvas()->canvasItem()->setGeometry (0, 0, width, height);
    dynamic_cast<KWCanvasItem*> (canvas())->updateSize();
}

QString CATextDocumentHandler::searchString() const
{
    return d->searchString;
}

void CATextDocumentHandler::setSearchString (const QString& searchString)
{
    d->searchString = searchString;
    d->findText->find(searchString);

    emit searchStringChanged();
}

void CATextDocumentHandler::findNext()
{
    d->findText->findNext();
}

void CATextDocumentHandler::findPrevious()
{
    d->findText->findPrevious();
}

void CATextDocumentHandler::findMatchFound (const KoFindMatch& match)
{
    QTextCursor cursor = match.location().value<QTextCursor>();
    canvas()->canvasItem()->update();

    canvas()->resourceManager()->setResource (KoText::CurrentTextAnchor, cursor.anchor());
    canvas()->resourceManager()->setResource (KoText::CurrentTextPosition, cursor.position());
}

int CATextDocumentHandler::totalPages() const {
    return d->document->pageManager()->pageCount();
}

void CATextDocumentHandler::gotoPage(int pageNumber)
{
    if (pageNumber == d->currentTextDocPage.pageNumber())
        return;
    d->currentTextDocPage = d->document->pageManager()->page(pageNumber);
    QRectF rect = documentController()->canvasController()->zoomHandler()->documentToView(d->currentTextDocPage.rect());
    documentController()->canvasController()->alignTopWith(rect.top());
}

void CATextDocumentHandler::findNoMatchFound()
{
    kDebug() << "Match for " << searchString() << " not found";
}

QString CATextDocumentHandler::bottomToolbarSource() const
{
    return "FindToolbar.qml";
}

QString CATextDocumentHandler::topToolbarSource() const
{
    return "TextDocumentEditingToolbar.qml";
}

QString CATextDocumentHandler::centerOverlaySource() const
{
    return "TextDocumentCenterOverlay.qml";
}

CATextDocumentModel* CATextDocumentHandler::paTextDocumentModel() const {
    return d->taTextDocumentModel;
}

void CATextDocumentHandler::copy()
{
    d->copyAction->activate(QAction::Trigger);
}

CAAbstractDocumentHandler::FlickModes CATextDocumentHandler::flickMode() const
{
    return FlickVertically;
}

#include "CATextDocumentHandler.moc"
