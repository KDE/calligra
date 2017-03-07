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

#include "CQPresentationCanvas.h"
#include "CQPresentationView.h"
#include "CQCanvasController.h"

#include "gemini/ViewModeSwitchEvent.h"

#include <QStyleOptionGraphicsItem>

#include <KoPluginLoader.h>
#include <KoDocumentEntry.h>
#include <KoDocumentResourceManager.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoTextEditor.h>
#include <KoPart.h>
#include <KoFindText.h>
#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoZoomController.h>
#include <KoZoomHandler.h>
#include <KoPADocument.h>
#include <KoPACanvasItem.h>
#include <KoPAPageBase.h>
#include <stage/part/KPrDocument.h>

#include <KActionCollection>

#include <QPluginLoader>
#include <QMimeDatabase>
#include <QGraphicsWidget>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextLayout>
#include <QApplication>



class CQPresentationCanvas::Private
{
public:
    Private() : canvasBase(0), view(0), document(0), part(0), currentSlide(0) { }

    KoCanvasBase* canvasBase;
    CQPresentationView* view;
    KPrDocument* document;
    KoPart* part;

    int currentSlide;
    QSizeF pageSize;
    QObjectList linkTargets;

    void updateLinkTargets()
    {
        qDeleteAll(linkTargets);
        linkTargets.clear();

        if (!view) {
            return;
        }
        foreach(const KoShape* shape, view->activePage()->shapes()) {
            if (!shape->hyperLink().isEmpty()) {
                QObject * obj = new QObject(view);
                obj->setProperty("linkRect", shape->boundingRect());
                obj->setProperty("linkTarget", QUrl(shape->hyperLink()));
                linkTargets.append(obj);
            }
        }

        QList<QTextDocument*> texts;
        KoFindText::findTextInShapes(view->activePage()->shapes(), texts);
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
                            QObject * obj = new QObject(view);
                            QRectF rect = getFragmentPosition(block, fragment);
                            obj->setProperty("linkRect", canvasBase->viewConverter()->documentToView(rect));
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
        if (!line.isValid())
        {
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

CQPresentationCanvas::CQPresentationCanvas(QDeclarativeItem* parent)
    : CQCanvasBase(parent), d(new Private)
{

}

CQPresentationCanvas::~CQPresentationCanvas()
{
    d->part->removeMainWindow(d->part->currentMainwindow());
    KoToolManager::instance()->removeCanvasController(d->canvasBase->canvasController());
    delete d;
}

int CQPresentationCanvas::currentSlide() const
{
    return d->currentSlide;
}

int CQPresentationCanvas::slideCount() const
{
    return d->document->pageCount();
}

QObjectList CQPresentationCanvas::linkTargets() const
{
    return d->linkTargets;
}

KPrDocument* CQPresentationCanvas::document() const
{
    return d->document;
}

QObject* CQPresentationCanvas::doc() const
{
    return d->document;
}

QObject* CQPresentationCanvas::part() const
{
    return d->part;
}

QSizeF CQPresentationCanvas::pageSize() const
{
    return d->pageSize;
}

void CQPresentationCanvas::setCurrentSlide(int slide)
{
    slide = qBound(0, slide, d->document->pageCount() - 1);
    if (slide != d->currentSlide) {
        d->currentSlide = slide;
        d->view->doUpdateActivePage(d->document->pageByIndex(slide, false));
        d->pageSize = d->view->activePage()->size();
        emit currentSlideChanged();
        d->updateLinkTargets();
        emit linkTargetsChanged();
    }
}

void CQPresentationCanvas::render(QPainter* painter, const QRectF& target)
{
    QStyleOptionGraphicsItem option;
    option.exposedRect = target;
    option.rect = target.toAlignedRect();
    d->canvasBase->canvasItem()->paint(painter, &option);
}

QObject* CQPresentationCanvas::textEditor() const
{
    if (d->canvasBase) {
        return KoTextEditor::getTextEditorFromCanvas(d->canvasBase);
    }
    return 0;
}

void CQPresentationCanvas::deselectEverything()
{
    KoTextEditor* editor = KoTextEditor::getTextEditorFromCanvas(d->canvasBase);
    if (editor) {
        editor->clearSelection();
    }
    d->canvasBase->shapeManager()->selection()->deselectAll();
}

qreal CQPresentationCanvas::shapeTransparency() const
{
    if (d->canvasBase && d->canvasBase->shapeManager()) {
        KoShape* shape = d->canvasBase->shapeManager()->selection()->firstSelectedShape();
        if (shape) {
            return shape->transparency();
        }
    }
    return CQCanvasBase::shapeTransparency();
}

void CQPresentationCanvas::setShapeTransparency(qreal newTransparency)
{
    if (d->canvasBase && d->canvasBase->shapeManager()) {
        KoShape* shape = d->canvasBase->shapeManager()->selection()->firstSelectedShape();
        if (shape) {
            if (!qFuzzyCompare(1 + shape->transparency(), 1 + newTransparency)) {
                shape->setTransparency(newTransparency);
                CQCanvasBase::setShapeTransparency(newTransparency);
            }
        }
    }
}

void CQPresentationCanvas::openFile(const QString& uri)
{
    emit loadingBegun();

    KoDocumentEntry entry;
    QList<QPluginLoader*> pluginLoaders = KoPluginLoader::pluginLoaders("calligra/parts");
    Q_FOREACH (QPluginLoader *loader, pluginLoaders) {
        if (loader->fileName().contains(QLatin1String("stagepart"))) {
            entry = KoDocumentEntry(loader);
            pluginLoaders.removeOne(loader);
            break;
        }
    }
    qDeleteAll(pluginLoaders);
    if (entry.isEmpty()) {
        qWarning("Unable to load Stage plugin, aborting!");
        return;
    }

    // QT5TODO: ownership of d->part unclear
    d->part = entry.createKoPart();
    d->document = dynamic_cast<KPrDocument*>(d->part->document());
    d->document->setAutoSave(0);
    d->document->setCheckAutoSaveFile(false);
    if (uri.endsWith(QLatin1String("otp"), Qt::CaseInsensitive)) {
        QUrl url(uri);
        bool ok = d->document->loadNativeFormat(url.toLocalFile());
        d->document->setModified(false);
        d->document->undoStack()->clear();

        if (ok) {
            QString mimeType = QMimeDatabase().mimeTypeForUrl(url).name();
            // in case this is a open document template remove the -template from the end
            mimeType.remove( QRegExp( "-template$" ) );
            d->document->setMimeTypeAfterLoading(mimeType);
            d->document->resetURL();
            d->document->setEmpty();
        } else {
            // some kind of error reporting thing here... failed to load template, tell the user
            // why their canvas is so terribly empty.
            d->document->initEmpty();
        }
    } else {
        d->document->openUrl (QUrl (uri));
    }

    d->document->setModified(false);
    qApp->processEvents();

    KoPACanvasItem *paCanvasItem = static_cast<KoPACanvasItem*>(d->part->canvasItem(d->part->document()));
    d->canvasBase = paCanvasItem;
    createAndSetCanvasControllerOn(d->canvasBase);

    d->view = new CQPresentationView(canvasController(), static_cast<KoPACanvasBase*>(d->canvasBase), dynamic_cast<KPrDocument*>(d->document));
    paCanvasItem->setView(d->view);

    d->canvasBase->resourceManager()->setResource(KoDocumentResourceManager::HandleRadius, 9);
    d->canvasBase->resourceManager()->setResource(KoDocumentResourceManager::GrabSensitivity, 9);

    createAndSetZoomController(d->canvasBase);
    d->view->setZoomController(zoomController());
    d->view->connectToZoomController();

    QGraphicsWidget *graphicsWidget = dynamic_cast<QGraphicsWidget*>(d->canvasBase);
    graphicsWidget->setParentItem(this);
    graphicsWidget->installEventFilter(this);
    graphicsWidget->setVisible(true);
    graphicsWidget->setGeometry(x(), y(), width(), height());

    if (d->document->pageCount() > 0) {
        d->view->doUpdateActivePage(d->document->pageByIndex(0, false));
        d->pageSize = d->view->activePage()->size();
        emit currentSlideChanged();

        d->updateLinkTargets();
        emit linkTargetsChanged();
    }

    emit documentChanged();
    emit loadingFinished();
}

void CQPresentationCanvas::createAndSetCanvasControllerOn(KoCanvasBase* canvas)
{
    //TODO: pass a proper action collection
    CQCanvasController *controller = new CQCanvasController(new KActionCollection(this));
    setCanvasController(controller);
    controller->setCanvas(canvas);
    KoToolManager::instance()->addController (controller);
}

void CQPresentationCanvas::createAndSetZoomController(KoCanvasBase* canvas)
{
    KoZoomHandler* zoomHandler = static_cast<KoZoomHandler*> (canvas->viewConverter());
    setZoomController(new KoZoomController(canvasController(),
                                           zoomHandler,
                                           new KActionCollection(this)));

    KoPACanvasItem* canvasItem = static_cast<KoPACanvasItem*>(canvas);

    // update the canvas whenever we scroll, the canvas controller must emit this signal on scrolling/panning
    connect (canvasController()->proxyObject,
                SIGNAL(moveDocumentOffset(QPoint)), canvasItem, SLOT(slotSetDocumentOffset(QPoint)));
    // whenever the size of the document viewed in the canvas changes, inform the zoom controller
    connect (canvasItem, SIGNAL(documentSize(QSize)), this, SLOT(updateDocumentSize(QSize)));
    canvasItem->updateSize();
    canvasItem->update();
}

void CQPresentationCanvas::updateDocumentSize(const QSize& size)
{
    zoomController()->setDocumentSize(d->canvasBase->viewConverter()->viewToDocument(size), false);
}

bool CQPresentationCanvas::event(QEvent* event)
{    switch(static_cast<int>(event->type())) {
        case ViewModeSwitchEvent::AboutToSwitchViewModeEvent: {
            ViewModeSynchronisationObject* syncObject = static_cast<ViewModeSwitchEvent*>(event)->synchronisationObject();

            // Simplest of transfer - no zoom transfer for presentations, just current slide
            syncObject->currentSlide = d->currentSlide;
            syncObject->shapes = d->canvasBase->shapeManager()->shapes();
            syncObject->initialized = true;

            return true;
        }
        case ViewModeSwitchEvent::SwitchedToTouchModeEvent: {
            ViewModeSynchronisationObject* syncObject = static_cast<ViewModeSwitchEvent*>(event)->synchronisationObject();

            if (syncObject->initialized) {
                d->canvasBase->shapeManager()->setShapes(syncObject->shapes);

                zoomController()->setZoom(KoZoomMode::ZOOM_PAGE, 1.0);
                zoomController()->zoomAction()->zoomOut();

                setCurrentSlide(syncObject->currentSlide);
                qApp->processEvents();

                KoToolManager::instance()->switchToolRequested("InteractionTool");
            }

            return true;
        }
//         case KisTabletEvent::TabletPressEx:
//         case KisTabletEvent::TabletReleaseEx:
//             emit interactionStarted();
//             d->canvas->inputManager()->eventFilter(this, event);
//             return true;
//         case KisTabletEvent::TabletMoveEx:
//             d->tabletEventCount++; //Note that this will wraparound at some point; This is intentional.
// #ifdef Q_OS_X11
//             if (d->tabletEventCount % 2 == 0)
// #endif
//                 d->canvas->inputManager()->eventFilter(this, event);
//             return true;
        default:
            break;
    }
    return QDeclarativeItem::event( event );
}

void CQPresentationCanvas::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (d->canvasBase) {
        QGraphicsWidget *widget = dynamic_cast<QGraphicsWidget*>(d->canvasBase);
        if (widget) {
            widget->setGeometry(newGeometry);
        }
    }
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}
