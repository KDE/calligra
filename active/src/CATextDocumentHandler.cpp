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

#include <KMimeType>
#include <KMimeTypeTrader>
#include <KDebug>
#include <KActionCollection>
#include <KoSelection.h>

#include <QTextDocument>

class CATextDocumentHandler::Private
{
public:
    Private() {
        document = 0;
        findText = 0;
    }
    KWDocument* document;
    KWPage currentTextDocPage;
    QString searchString;
    KoFindText* findText;
};

CATextDocumentHandler::CATextDocumentHandler (CADocumentController* documentController)
    : CAAbstractDocumentHandler (documentController)
    , d (new Private())
{
    QList<QTextDocument*> texts;
    d->findText = new KoFindText(this);
    connect (d->findText, SIGNAL (updateCanvas()), SLOT (updateCanvas()));
    connect (d->findText, SIGNAL (matchFound (KoFindMatch)), SLOT (findMatchFound (KoFindMatch)));
    connect (d->findText, SIGNAL (noMatchFound()), SLOT (findNoMatchFound()));
}

CATextDocumentHandler::~CATextDocumentHandler()
{
    delete d;
}

QStringList CATextDocumentHandler::supportedMimetypes()
{
    QStringList supportedTypes;
    supportedTypes << "application/vnd.oasis.opendocument.text" << "application/msword";
    return supportedTypes;
}

bool CATextDocumentHandler::openDocument (const QString& uri)
{
    QString error;
    QString mimetype = KMimeType::findByPath (uri)->name();
    KoPart *part = KMimeTypeTrader::createInstanceFromQuery<KoPart>(mimetype,
                      QLatin1String("CalligraPart"), 0, QString(), QVariantList(), &error);

    if (!part) {
        kDebug() << "Doc can't be openend" << error;
        return false;
    }

    d->document = static_cast<KWDocument*> (part->document());
    d->document->openUrl (KUrl (uri));

    setCanvas (dynamic_cast<KoCanvasBase*> (part->canvasItem()));
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
    KoZoomController* zoomController =
        new KoZoomController (dynamic_cast<KoCanvasController*> (documentController()->canvasController()),
                              zoomHandler, part->actionCollection());
    documentController()->canvasController()->setZoomController (zoomController);
    d->currentTextDocPage = d->document->pageManager()->begin();
    zoomController->setPageSize (d->currentTextDocPage.rect().size());
    zoomController->setZoom (KoZoomMode::ZOOM_CONSTANT, 1.0);

    if (kwCanvasItem) {
        kwCanvasItem->updateSize();

        // whenever the size of the document viewed in the canvas changes, inform the zoom controller
        connect (kwCanvasItem, SIGNAL (documentSize (QSizeF)), zoomController, SLOT (setDocumentSize (QSizeF)));
        // update the canvas whenever we scroll, the canvas controller must emit this signal on scrolling/panning
        connect (documentController()->canvasController()->canvasControllerProxyObject(), SIGNAL (moveDocumentOffset (const QPoint&)),
                 kwCanvasItem, SLOT (setDocumentOffset (QPoint)));
        kwCanvasItem->updateSize();
    }

    connect (documentController()->canvasController(), SIGNAL (needsCanvasResize (QSizeF)), SLOT (resizeCanvas (QSizeF)));
    connect (documentController()->canvasController(), SIGNAL (needCanvasUpdate()), SLOT (updateCanvas()));

    documentController()->canvasController()->zoomToFit();

    QList<QTextDocument*> texts;
    KoFindText::findTextInShapes(kwCanvasItem->shapeManager()->shapes(), texts);
    d->findText->setDocuments(texts);

    KAction *action = part->actionCollection()->addAction(KStandardAction::Copy,  "edit_copy", 0, 0);
    new KoCopyController(canvas(), action);

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
     d->currentTextDocPage = d->document->pageManager()->page(pageNumber);
     documentController()->canvasController()->ensureVisible(d->currentTextDocPage.rect(), true);
}

void CATextDocumentHandler::findNoMatchFound()
{
    kDebug() << "Match for " << searchString() << " not found";
}

QString CATextDocumentHandler::bottomToolbarSource() const
{
    return "TextDocumentFindToolbar.qml";
}

QString CATextDocumentHandler::topToolbarSource() const
{
    return "TextDocumentEditingToolbar.qml";
}

QString CATextDocumentHandler::leftToolbarSource() const
{
    return "TextDocumentLeftToolbar.qml";
}

void CATextDocumentHandler::copy()
{
    document()->documentPart()->actionCollection()->action("edit_copy")->activate(QAction::Trigger);
}

#include "CATextDocumentHandler.moc"
