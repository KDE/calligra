/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Shantanu Tushar <shantanu@kde.org>
 * Copyright (C) 2013 Sujith Haridasan <sujith.h@gmail.com>
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

#include "CQTextDocumentCanvas.h"
#include "CQCanvasController.h"
#include "CQTextDocumentModel.h"
#include "CQTextDocumentNotesModel.h"

#include "gemini/ViewModeSwitchEvent.h"

#include <KoPluginLoader.h>
#include <KoDocumentEntry.h>
#include <KoUnit.h>
#include <KoDocument.h>
#include <KoDocumentResourceManager.h>
#include <KoPart.h>
#include <KoFindText.h>
#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoZoomHandler.h>
#include <KoZoomController.h>
#include <KoShape.h>
#include <KoToolManager_p.h>
#include <KoToolBase.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoShapeRegistry.h>
#include <KoShapeAnchor.h>
#include <KoShapeContainer.h>
#include <KoTextEditor.h>
#include <KoProperties.h>
#include <KoColumns.h>
#include <KWDocument.h>
#include <KWPage.h>
#include <KWCanvasItem.h>
#include <commands/KWShapeCreateCommand.h>

#include <KActionCollection>

#include <QPluginLoader>
#include <QMimeDatabase>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsWidget>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextLayout>
#include <QGraphicsSceneMouseEvent>
#include <QTextDocumentFragment>
#include <QSvgRenderer>
#include <QApplication>
#include <QDebug>

class CQTextDocumentCanvas::Private
{
public:
    Private()
        : canvas(0),
          findText(0),
          documentModel(0),
          document(0),
          pageNumber(0),
          throttleTimer(new QTimer()),
          currentTool(0),
          notes(0),
          textEditor(0)
    {
        throttleTimer->setInterval(200);
        throttleTimer->setSingleShot(true);
    }

    KWCanvasItem *canvas;
    QString searchTerm;
    KoFindText *findText;
    CQTextDocumentModel *documentModel;
    KWDocument* document;
    KoPart* part;
    QSize documentSize;
    int pageNumber;
    QPoint currentPoint;
    QObjectList linkTargets;
    QTimer* throttleTimer;
    KoToolBase* currentTool;
    CQTextDocumentNotesModel* notes;
    KoTextEditor* textEditor;

    void updateLinkTargets()
    {
        qDeleteAll(linkTargets);
        linkTargets.clear();

        if (!canvas) {
            return;
        }

        foreach(const KoShape* shape, canvas->shapeManager()->shapes()) {
            if (!shape->hyperLink().isEmpty()) {
                QObject * obj = new QObject(documentModel);
                obj->setProperty("linkRect", shape->boundingRect());
                obj->setProperty("linkTarget", QUrl(shape->hyperLink()));
                linkTargets.append(obj);
            }
        }

        foreach(QTextDocument* text, findText->documents()) {
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
                            QObject * obj = new QObject(documentModel);
                            QRectF rect = getFragmentPosition(block, fragment);
                            KWPage page = document->pageManager()->page(rect.left());
                            rect.translate(page.topMargin(), page.rightMargin());
                            rect = canvas->viewMode()->documentToView(rect, canvas->viewConverter());
                            rect.translate(page.pageNumber() * (page.topMargin() + page.bottomMargin()) + 20, 0);
                            obj->setProperty("linkRect", rect);
                            obj->setProperty("linkTarget", QUrl(format.anchorHref()));
                            linkTargets.append(obj);
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

    QList<KoShape*> deepShapeFind(const QList<KoShape*>& shapes)
    {
        QList<KoShape*> allShapes;
        foreach(KoShape* shape, shapes) {
            allShapes.append(shape);
            KoShapeContainer *container = dynamic_cast<KoShapeContainer*>(shape);
            if (container) {
                allShapes.append(deepShapeFind(container->shapes()));
            }
        }
        return allShapes;
    }
    QRectF getCursorPosition(int position)
    {
        QPointF point;
        QTextBlock block = textEditor->document()->findBlock(position);
        QTextLayout* layout = block.layout();
        QTextLine line = layout->lineForTextPosition(position - block.position());
        if (!line.isValid()) {
            // fragment has no valid position and consequently no line...
            return QRectF();
        }
        qreal top = line.position().y();
        qreal left = line.cursorToX(position - block.position());
        point = QPointF(left + layout->position().y(), top + layout->position().x());

        KoShape* shape = canvas->shapeManager()->selection()->firstSelectedShape();
        point += shape->position();
        while(KoShapeContainer* parent = shape->parent()) {
            point += parent->position();
        }

        KWPage page = document->pageManager()->page(point.y());
//         point += QPointF(page.rightMargin(), page.pageNumber() * page.topMargin()
//                                            + (page.pageNumber() - 1) * page.bottomMargin());
//         if (page.pageNumber() > 1)
//             point += QPointF(0, 20);
        point += QPointF(0, (page.pageNumber() - 1) * (page.topMargin() + 20));
//        point = canvas->viewConverter()->documentToView(point);

        return canvas->viewConverter()->documentToView(QRectF(point, QSizeF(0, line.height())));
    }
};

CQTextDocumentCanvas::CQTextDocumentCanvas(QDeclarativeItem* parent)
    : CQCanvasBase(parent), d(new Private)
{
    setAcceptedMouseButtons(Qt::LeftButton);
    d->findText = new KoFindText(this);

    connect (d->findText, SIGNAL(updateCanvas()), SLOT(updateCanvas()));
    connect (d->findText, SIGNAL(matchFound(KoFindMatch)), SLOT(findMatchFound(KoFindMatch)));
    connect (d->findText, SIGNAL(noMatchFound()), SLOT(findNoMatchFound()));
    connect (KoToolManager::instance(), SIGNAL(changedTool(KoCanvasController*,int)), SLOT(currentToolChanged(KoCanvasController*,int)));
}

CQTextDocumentCanvas::~CQTextDocumentCanvas()
{
    d->part->removeMainWindow(d->part->currentMainwindow());
    KoToolManager::instance()->removeCanvasController(d->canvas->canvasController());
    delete d;
}

void CQTextDocumentCanvas::openFile(const QString& uri)
{
    emit loadingBegun();

    KoDocumentEntry entry;
    QList<QPluginLoader*> pluginLoaders = KoPluginLoader::pluginLoaders("calligra/parts");
    Q_FOREACH (QPluginLoader *loader, pluginLoaders) {
        if (loader->fileName().contains(QLatin1String("wordspart"))) {
            entry = KoDocumentEntry(loader);
            pluginLoaders.removeOne(loader);
            break;
        }
    }
    qDeleteAll(pluginLoaders);
    if (entry.isEmpty()) {
        qWarning("Unable to load Words plugin, aborting!");
        return;
    }

    // QT5TODO: ownership of d->part unclear
    d->part = entry.createKoPart();
    KoDocument* document = d->part->document();
    document->setAutoSave(0);
    document->setCheckAutoSaveFile(false);

    QUrl url(uri);
    if (url.scheme() == "newfile") {
        KWDocument* doc = qobject_cast<KWDocument*>(document);
        doc->initEmpty();
        KWPageStyle style = doc->pageManager()->defaultPageStyle();
        Q_ASSERT(style.isValid());

        KoColumns columns;
        columns.count = url.queryItemValue("columncount").toInt();
        columns.gapWidth = url.queryItemValue("columngap").toDouble();
        style.setColumns(columns);

        KoPageLayout layout = style.pageLayout();
        layout.format = KoPageFormat::formatFromString(url.queryItemValue("pageformat"));
        layout.orientation = (KoPageFormat::Orientation)url.queryItemValue("pageorientation").toInt();
        layout.height = MM_TO_POINT(url.queryItemValue("height").toDouble());
        layout.width = MM_TO_POINT(url.queryItemValue("width").toDouble());
        if (url.queryItemValue("facingpages").toInt() == 1) {
            layout.bindingSide = MM_TO_POINT(url.queryItemValue("leftmargin").toDouble());
            layout.pageEdge = MM_TO_POINT(url.queryItemValue("rightmargin").toDouble());
            layout.leftMargin = layout.rightMargin = -1;
        }
        else {
            layout.bindingSide = layout.pageEdge = -1;
            layout.leftMargin = MM_TO_POINT(url.queryItemValue("leftmargin").toDouble());
            layout.rightMargin = MM_TO_POINT(url.queryItemValue("rightmargin").toDouble());
        }
        layout.topMargin = MM_TO_POINT(url.queryItemValue("topmargin").toDouble());
        layout.bottomMargin = MM_TO_POINT(url.queryItemValue("bottommargin").toDouble());
        style.setPageLayout(layout);

        doc->setUnit(KoUnit::fromSymbol(url.queryItemValue("unit")));
        doc->relayout();
    } else if (url.scheme() == "template") {
        qApp->setOverrideCursor(Qt::BusyCursor);
        // Nip away the manually added template:// bit of the uri passed from the caller
        bool ok = document->loadNativeFormat(uri.mid(11));
        document->setModified(false);
        document->undoStack()->clear();

        if (ok) {
            QString mimeType = QMimeDatabase().mimeTypeForUrl(url).name();
            // in case this is a open document template remove the -template from the end
            mimeType.remove( QRegExp( "-template$" ) );
            document->setMimeTypeAfterLoading(mimeType);
            document->resetURL();
            document->setEmpty();
        } else {
            document->showLoadingErrorDialog();
            document->initEmpty();
        }
        qApp->restoreOverrideCursor();
    } else {
        document->openUrl(url);
    }

    document->setModified(false);
    qApp->processEvents();

    d->canvas = dynamic_cast<KWCanvasItem*> (d->part->canvasItem(d->part->document()));
    createAndSetCanvasControllerOn(d->canvas);
    createAndSetZoomController(d->canvas);
    updateZoomControllerAccordingToDocument(document);

    d->canvas->resourceManager()->setResource(KoDocumentResourceManager::HandleRadius, 9);
    d->canvas->resourceManager()->setResource(KoDocumentResourceManager::GrabSensitivity, 9);

    QGraphicsWidget *graphicsWidget = dynamic_cast<QGraphicsWidget*>(d->canvas);
    graphicsWidget->setParentItem(this);
    graphicsWidget->installEventFilter(this);
    graphicsWidget->setVisible(true);
    graphicsWidget->setGeometry(x(), y(), width(), height());

    if (d->pageNumber >= 1) {
      gotoPage(d->pageNumber, document);
    }

    QList<QTextDocument*> texts;
    KoFindText::findTextInShapes(d->canvas->shapeManager()->shapes(), texts);
    d->findText->setDocuments(texts);

    d->document = qobject_cast<KWDocument*>(document);
    d->documentModel = new CQTextDocumentModel(this, d->document, d->canvas->shapeManager());
    emit documentModelChanged();
    emit thumbnailSizeChanged();
    connect(d->documentModel, SIGNAL(thumbnailSizeChanged()), SIGNAL(thumbnailSizeChanged()));

    d->updateLinkTargets();
    emit linkTargetsChanged();

    connect(d->canvas->shapeManager(), SIGNAL(selectionChanged()), SIGNAL(textEditorChanged()));
    connect(d->canvas->shapeManager(), SIGNAL(selectionChanged()), SIGNAL(shapeTransparencyChanged()));

    d->notes = new CQTextDocumentNotesModel(this);
    emit notesChanged();

    emit textEditorChanged();
    emit loadingFinished();
}

void CQTextDocumentCanvas::gotoPage(int pageNumber, KoDocument *document)
{
    const KWDocument *kwDoc = static_cast<const KWDocument*>(document);
    KWPage currentTextDocPage = kwDoc->pageManager()->page(pageNumber);

    QRectF rect = d->canvas->viewConverter()->documentToView(currentTextDocPage.rect());
    canvasController()->pan(rect.topLeft().toPoint() - d->canvas->viewConverter()->documentToView(canvasController()->documentOffset()).toPoint());
    alignTopWith(rect.top());
    updateCanvas();
}

int CQTextDocumentCanvas::cameraY() const
{
    return d->currentPoint.y();
}

qreal CQTextDocumentCanvas::pagePosition(int pageIndex)
{
    KWPage page = d->document->pageManager()->page(pageIndex);
    // a very silly heuristic for ensuring the page number changes if we change pages.
    // this means we don't have to glue the canvas and controlleritem together too close,
    // but yes, it does look a bit silly.
    QTimer::singleShot(0, d->throttleTimer, SLOT(stop()));
    QTimer::singleShot(0, this, SIGNAL(currentPageNumberChanged()));
    return d->canvas->viewMode()->documentToView(page.rect().topLeft(), d->canvas->viewConverter()).y();
}

qreal CQTextDocumentCanvas::shapeTransparency() const
{
    if (d->canvas && d->canvas->shapeManager()) {
        KoShape* shape = d->canvas->shapeManager()->selection()->firstSelectedShape();
        if (shape) {
            return shape->transparency();
        }
    }
    return CQCanvasBase::shapeTransparency();
}

void CQTextDocumentCanvas::setShapeTransparency(qreal newTransparency)
{
    if (d->canvas && d->canvas->shapeManager()) {
        KoShape* shape = d->canvas->shapeManager()->selection()->firstSelectedShape();
        if (shape) {
            if (!qFuzzyCompare(1 + shape->transparency(), 1 + newTransparency)) {
                shape->setTransparency(newTransparency);
                CQCanvasBase::setShapeTransparency(newTransparency);
            }
        }
    }
}

QObject* CQTextDocumentCanvas::textEditor()
{
    if (d->canvas) {
        if (d->textEditor) {
            disconnect(d->textEditor, SIGNAL(cursorPositionChanged()), this, SIGNAL(selectionChanged()));
        }
        d->textEditor = KoTextEditor::getTextEditorFromCanvas(d->canvas);
        if (d->textEditor) {
            disconnect(d->textEditor, SIGNAL(cursorPositionChanged()), this, SIGNAL(selectionChanged()));
        }
        emit selectionChanged();
        return d->textEditor;
    }
    return 0;
}

bool CQTextDocumentCanvas::hasSelection() const
{
    if (d->textEditor) {
        return d->textEditor->hasSelection();
    }
    return false;
}

QRectF CQTextDocumentCanvas::selectionStartPos() const
{
    if (d->textEditor) {
        return d->getCursorPosition(d->textEditor->selectionStart());
    }
    return QRectF(0,0,0,0);
}

QRectF CQTextDocumentCanvas::selectionEndPos() const
{
    if (d->textEditor) {
        return d->getCursorPosition(d->textEditor->selectionEnd());
    }
    return QRectF(0,0,0,0);
}

QObject* CQTextDocumentCanvas::zoomAction() const
{
    if (zoomController() && zoomController()->zoomAction()) {
        return zoomController()->zoomAction();
    }
    return 0;
}

QSizeF CQTextDocumentCanvas::thumbnailSize() const
{
    if (d->documentModel) {
        return d->documentModel->thumbnailSize();
    }
    return QSizeF();
}

void CQTextDocumentCanvas::setThumbnailSize(const QSizeF& newSize)
{
    if (d->documentModel) {
        d->documentModel->setThumbnailSize(newSize.toSize());
    }
    emit thumbnailSizeChanged();
}

void CQTextDocumentCanvas::deselectEverything()
{
    KoTextEditor* editor = KoTextEditor::getTextEditorFromCanvas(d->canvas);
    if (editor) {
        editor->clearSelection();
    }
    d->canvas->shapeManager()->selection()->deselectAll();
    updateCanvas();
}

QObject* CQTextDocumentCanvas::notes() const
{
    return d->notes;
}

void CQTextDocumentCanvas::addSticker(const QString& imageUrl)
{
    QSvgRenderer renderer(QUrl(imageUrl).toLocalFile());
   // Prepare a QImage with desired characteristics
    QImage image(200, 200, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    // Get QPainter that paints to the image
    QPainter painter(&image);
    renderer.render(&painter);
    painter.end();

    KoProperties* params = new KoProperties();
    params->setProperty("qimage", image);
    KoShapeFactoryBase* factory = KoShapeRegistry::instance()->get("PictureShape");
    if (factory) {
        KoShape* shape = factory->createShape(params, d->document->resourceManager());

        QPointF pos = d->canvas->viewToDocument(d->canvas->documentOffset() + QPointF(d->canvas->size().width() / 2, d->canvas->size().height() / 2));
        KoShapeAnchor *anchor = new KoShapeAnchor(shape);
        anchor->setAnchorType(KoShapeAnchor::AnchorPage);
        anchor->setHorizontalPos(KoShapeAnchor::HFromLeft);
        anchor->setVerticalPos(KoShapeAnchor::VFromTop);
        anchor->setHorizontalRel(KoShapeAnchor::HPage);
        anchor->setVerticalRel(KoShapeAnchor::VPage);
        shape->setAnchor(anchor);
        shape->setPosition(pos);
        shape->scale(0.2, 0.2);

//        KWShapeCreateCommand *cmd = new KWShapeCreateCommand(d->document, shape);
        KoSelection *selection = d->canvas->shapeManager()->selection();
        selection->deselectAll();
        selection->select(shape);
//        d->canvas->addCommand(cmd);
        d->canvas->shapeManager()->addShape(shape);

        d->notes->addEntry("", imageUrl, "Neutral", shape);
    }
}

void CQTextDocumentCanvas::addNote(const QString& text, const QString& color, const QString& imageUrl)
{
    QSvgRenderer renderer(QUrl(imageUrl).toLocalFile());

   // Prepare a QImage with desired characteritisc
    QImage image(400, 200, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    // Get QPainter that paints to the image
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    renderer.render(&painter, image.rect());

    QFont font;
    font.setFamily("Permanent Marker");
    font.setStyle(QFont::StyleNormal);
    font.setPixelSize(40);
    painter.setPen(QPen(QColor(color), 0));
    painter.setFont(font);
    painter.drawText(image.rect().adjusted(10, 10, -20, -20), Qt::AlignCenter | Qt::TextWordWrap, text);
    painter.end();

    KoProperties* params = new KoProperties();
    params->setProperty("qimage", image);
    KoShapeFactoryBase* factory = KoShapeRegistry::instance()->get("PictureShape");
    if (factory) {
        KoShape* shape = factory->createShape(params, d->document->resourceManager());

        QPointF pos = d->canvas->viewToDocument(d->canvas->documentOffset() + QPointF(d->canvas->size().width() / 2, d->canvas->size().height() / 2));
        KoShapeAnchor *anchor = new KoShapeAnchor(shape);
        anchor->setAnchorType(KoShapeAnchor::AnchorPage);
        anchor->setHorizontalPos(KoShapeAnchor::HFromLeft);
        anchor->setVerticalPos(KoShapeAnchor::VFromTop);
        anchor->setHorizontalRel(KoShapeAnchor::HPage);
        anchor->setVerticalRel(KoShapeAnchor::VPage);
        shape->setAnchor(anchor);
        shape->setPosition(pos);
        shape->rotate(-15);
        shape->scale(0.3, 0.3);

//        KWShapeCreateCommand *cmd = new KWShapeCreateCommand(d->document, shape);
        KoSelection *selection = d->canvas->shapeManager()->selection();
        selection->deselectAll();
        selection->select(shape);
//        d->canvas->addCommand(cmd);
        d->canvas->shapeManager()->addShape(shape);

        d->notes->addEntry(text, "", color, shape);
    }
}

void CQTextDocumentCanvas::setCameraY(int cameraY)
{
    d->currentPoint.setY (cameraY);
    emit cameraYChanged();
}

void CQTextDocumentCanvas::alignTopWith(int y)
{
    d->currentPoint.setY(y);
    emit cameraYChanged();
}

int CQTextDocumentCanvas::currentPageNumber() const
{
    if (d->document && !d->throttleTimer->isActive()) {
        // Can't use this at the moment, we sort of don't have the right one, because derp :P
        //d->canvas->resourceManager()->resource(KoCanvasResourceManager::CurrentPage).toInt();
        d->throttleTimer->start();
        const KWDocument *kwDoc = static_cast<const KWDocument*>(d->document);
        d->pageNumber = kwDoc->pageManager()->page(d->canvas->viewMode()->viewToDocument(d->canvas->documentOffset(), d->canvas->viewConverter())).pageNumber();
    }
    return d->pageNumber;
}

void CQTextDocumentCanvas::setCurrentPageNumber(const int& currentPageNumber)
{
    if (d->pageNumber != currentPageNumber) {
        gotoPage(currentPageNumber, d->document);
    }
}

void CQTextDocumentCanvas::render(QPainter* painter, const QRectF& target)
{
    Q_UNUSED(target)
    QStyleOptionGraphicsItem option;
    option.exposedRect = QRect(0, 0, width(), height());
    option.rect = option.exposedRect.toAlignedRect();
    d->canvas->canvasItem()->paint(painter, &option);
}

bool CQTextDocumentCanvas::event( QEvent* event )
{
    switch(static_cast<int>(event->type())) {
        case ViewModeSwitchEvent::AboutToSwitchViewModeEvent: {
            ViewModeSynchronisationObject* syncObject = static_cast<ViewModeSwitchEvent*>(event)->synchronisationObject();

            if (d->canvas) {
                syncObject->documentOffset = d->canvas->documentOffset();
                syncObject->zoomLevel = zoomController()->zoomAction()->effectiveZoom();
                syncObject->activeToolId = KoToolManager::instance()->activeToolId();
                syncObject->shapes = d->canvas->shapeManager()->shapes();
                syncObject->initialized = true;
            }

            return true;
        }
        case ViewModeSwitchEvent::SwitchedToTouchModeEvent: {
            ViewModeSynchronisationObject* syncObject = static_cast<ViewModeSwitchEvent*>(event)->synchronisationObject();

            if (d->canvas && syncObject->initialized) {
                d->canvas->shapeManager()->setShapes(syncObject->shapes);

                KoToolManager::instance()->switchToolRequested("PageToolFactory_ID");
                qApp->processEvents();

                zoomController()->setZoom(KoZoomMode::ZOOM_CONSTANT, syncObject->zoomLevel);

                qApp->processEvents();
                emit positionShouldChange(syncObject->documentOffset);
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

void CQTextDocumentCanvas::currentToolChanged(KoCanvasController* controller, int uniqueToolId)
{
    Q_UNUSED(controller)
    Q_UNUSED(uniqueToolId)
    d->currentTool = qobject_cast<KoToolBase*>(KoToolManager::instance()->toolById(d->canvas, KoToolManager::instance()->activeToolId()));
}

void CQTextDocumentCanvas::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e)
{
    QMouseEvent me(e->type(), e->pos().toPoint(), e->button(), e->buttons(), e->modifiers());
    KoPointerEvent pe(&me, d->canvas->viewToDocument(e->pos() + d->canvas->documentOffset()));
    d->currentTool->mouseDoubleClickEvent(&pe);
    updateCanvas();
    emit selectionChanged();
    e->setAccepted(me.isAccepted());
}

void CQTextDocumentCanvas::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{
    QMouseEvent me(e->type(), e->pos().toPoint(), e->button(), e->buttons(), e->modifiers());
    KoPointerEvent pe(&me, d->canvas->viewToDocument(e->pos() + d->canvas->documentOffset()));
    d->currentTool->mouseMoveEvent(&pe);
    updateCanvas();
    emit selectionChanged();
    e->setAccepted(me.isAccepted());
}

void CQTextDocumentCanvas::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
    QMouseEvent me(e->type(), e->pos().toPoint(), e->button(), e->buttons(), e->modifiers());
    KoPointerEvent pe(&me, d->canvas->viewToDocument(e->pos() + d->canvas->documentOffset()));
    d->currentTool->mousePressEvent(&pe);
    updateCanvas();
    emit selectionChanged();
    e->setAccepted(me.isAccepted());
}

void CQTextDocumentCanvas::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{
    QMouseEvent me(e->type(), e->pos().toPoint(), e->button(), e->buttons(), e->modifiers());
    KoPointerEvent pe(&me, d->canvas->viewToDocument(e->pos() + d->canvas->documentOffset()));
    d->currentTool->mouseReleaseEvent(&pe);
    updateCanvas();
    emit selectionChanged();
    e->setAccepted(me.isAccepted());
}

void CQTextDocumentCanvas::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (d->canvas) {
        QGraphicsWidget *widget = dynamic_cast<QGraphicsWidget*>(d->canvas);
        if (widget) {
            widget->setGeometry(newGeometry);
        }
    }
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

void CQTextDocumentCanvas::createAndSetCanvasControllerOn(KoCanvasBase* canvas)
{
    //TODO: pass a proper action collection
    CQCanvasController *controller = new CQCanvasController(new KActionCollection(this));
    setCanvasController(controller);
    connect (controller, SIGNAL(documentSizeChanged(QSize)), SLOT(updateDocumentSize(QSize)));
    controller->setCanvas(canvas);
    KoToolManager::instance()->addController (controller);
}

void CQTextDocumentCanvas::createAndSetZoomController(KoCanvasBase* canvas)
{
    KoZoomHandler* zoomHandler = static_cast<KoZoomHandler*> (canvas->viewConverter());
    setZoomController(new KoZoomController(canvasController(), zoomHandler, new KActionCollection(this)));

    KWCanvasItem *kwCanvasItem = static_cast<KWCanvasItem*>(canvas);
    connect (kwCanvasItem, SIGNAL(documentSize(QSizeF)), zoomController(), SLOT(setDocumentSize(QSizeF)));
    connect (canvasController()->proxyObject, SIGNAL(moveDocumentOffset(QPoint)), SIGNAL(currentPageNumberChanged()));
    connect (canvasController()->proxyObject, SIGNAL(moveDocumentOffset(QPoint)), kwCanvasItem, SLOT(setDocumentOffset(QPoint)));
    connect (zoomController(), SIGNAL(zoomChanged(KoZoomMode::Mode,qreal)), SIGNAL(zoomActionChanged()));
    kwCanvasItem->updateSize();
    emit zoomActionChanged();
}

void CQTextDocumentCanvas::updateZoomControllerAccordingToDocument(const KoDocument* document)
{
    const KWDocument *kwDoc = static_cast<const KWDocument*>(document);
    zoomController()->setPageSize (kwDoc->pageManager()->begin().rect().size());
}

QString CQTextDocumentCanvas::searchTerm() const
{
    return d->searchTerm;
}

void CQTextDocumentCanvas::setSearchTerm(const QString& term)
{
    d->searchTerm = term;
    if (!term.isEmpty()) {
        d->findText->find(term);
    }
    emit searchTermChanged();
}

void CQTextDocumentCanvas::findMatchFound(const KoFindMatch &match)
{
    QTextCursor cursor = match.location().value<QTextCursor>();
    d->canvas->canvasItem()->update();

    d->canvas->resourceManager()->setResource (KoText::CurrentTextAnchor, cursor.anchor());
    d->canvas->resourceManager()->setResource (KoText::CurrentTextPosition, cursor.position());
}

void CQTextDocumentCanvas::findNoMatchFound()
{
    qDebug() << "Match for " << d->searchTerm << " not found";
}

void CQTextDocumentCanvas::updateCanvas()
{
    KWCanvasItem* kwCanvasItem = dynamic_cast<KWCanvasItem*> (d->canvas);
    kwCanvasItem->update();
}

void CQTextDocumentCanvas::findNext()
{
    d->findText->findNext();
}

void CQTextDocumentCanvas::findPrevious()
{
    d->findText->findPrevious();
}

QObject* CQTextDocumentCanvas::documentModel() const
{
    return d->documentModel;
}

KWDocument* CQTextDocumentCanvas::document() const
{
    return d->document;
}

QObject* CQTextDocumentCanvas::doc() const
{
    return d->document;
}

QObject* CQTextDocumentCanvas::part() const
{
    return d->part;
}

QObjectList CQTextDocumentCanvas::linkTargets() const
{
    return d->linkTargets;
}

QSize CQTextDocumentCanvas::documentSize() const
{
    return d->documentSize;
}

void CQTextDocumentCanvas::updateDocumentSize(const QSize& size)
{
    d->documentSize = size;
    emit documentSizeChanged();
}
