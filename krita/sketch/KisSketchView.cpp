/* This file is part of the KDE project
 * Copyright (C) 2012 Boudewijn Rempt <boud@kogmbh.com>
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
 */
#include "KisSketchView.h"

#include <QTimer>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QClipboard>
#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QDebug>

#include <kdebug.h>
#include <kmimetype.h>
#include <kstandarddirs.h>
#include <kactioncollection.h>
#include <kaction.h>

#include <KoZoomHandler.h>
#include <KoZoomController.h>
#include <KoProgressUpdater.h>
#include <KoToolProxy.h>
#include <KoCanvasController.h>
#include <KoFilterManager.h>
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>
#include <KoUnit.h>
#include <KoShapeController.h>
#include <KoDocumentResourceManager.h>
#include <KoCanvasResourceManager.h>
#include <KoShapeManager.h>

#include <kundo2stack.h>

#include "ProgressProxy.h"

#include "kis_painter.h"
#include "kis_layer.h"
#include "kis_paint_device.h"
#include "kis_doc2.h"
#include "kis_canvas2.h"
#include <kis_canvas_controller.h>
#include <kis_qpainter_canvas.h>
#include "kis_config.h"
#include "kis_view2.h"
#include "kis_image.h"
#include <kis_image_signal_router.h>
#include "kis_clipboard.h"
#include <input/kis_input_manager.h>
#include <kis_canvas_resource_provider.h>
#include <kis_zoom_manager.h>
#include <kis_selection_manager.h>
#include <kis_paint_device.h>
#include <kis_layer.h>
#include <kis_qpainter_canvas.h>
#include <kis_part2.h>
#include <kis_canvas_decoration.h>

#include "KisSketchPart.h"
#include "KisSelectionExtras.h"
#include "Settings.h"
#include "cpuid.h"
#include "DocumentManager.h"

#ifdef HAVE_OPENGL
#include <GL/glew.h>
#include <opengl2/kis_gl2_canvas.h>
#include "KisSketchCanvasFactory.h"
#include "KisSketchCanvas.h"
#include <QGLShaderProgram>
#include <QGLBuffer>
#endif

class KisSketchView::Private
{
public:
    Private( KisSketchView* qq)
        : q(qq)
        , doc(0)
        , view(0)
        , canvas(0)
        , canvasWidget(0)
        , selectionExtras(0)
        , undoAction(0)
        , redoAction(0)
        , useOpenGL(false)
        , viewportMoved(false)
        , zoomLevelChanged(false)
    { }
    ~Private() {
        delete selectionExtras;
    }

    void imageUpdated(const QRect &updated);
    void documentOffsetMoved();
    void zoomChanged();
    void resetDocumentPosition();
    void configChanged();
    void removeNodeAsync(KisNodeSP removedNode);

    KisSketchView* q;

    KisDoc2* doc;
    KisView2* view;
    KisCanvas2* canvas;
    KUndo2Stack* undoStack;

    QWidget *canvasWidget;

    QString file;

#ifdef HAVE_OPENGL
    QGLShaderProgram *shader;
    QGLBuffer *vertexBuffer;
    QGLBuffer *indexBuffer;
#endif

    KisSelectionExtras *selectionExtras;

    int modelMatrixLocation;
    int viewMatrixLocation;
    int projectionMatrixLocation;
    int texture0Location;
    int textureScaleLocation;
    int vertexAttributeLocation;
    int uv0AttributeLocation;

    QTimer *timer;

    QTimer *loadedTimer;
    QTimer *savedTimer;
    QAction* undoAction;
    QAction* redoAction;
    bool useOpenGL;

    KisPrescaledProjectionSP prescaledProjection;

    QColor backgroundColor;
    QBrush checkers;

    QPoint canvasOffset;
    bool viewportMoved;
    bool zoomLevelChanged;
};

KisSketchView::KisSketchView(QDeclarativeItem* parent)
    : QDeclarativeItem(parent)
    , d(new Private(this))
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setAcceptTouchEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::MiddleButton | Qt::RightButton);

    grabGesture(Qt::PanGesture);
    //grabGesture(Qt::PinchGesture);

    KoZoomMode::setMinimumZoom(0.1);
    KoZoomMode::setMaximumZoom(16.0);

    if (d->useOpenGL) {
#ifdef HAVE_OPENGL
        KisCanvas2::setCanvasWidgetFactory(new KisSketchCanvasFactory());
#endif
    }

    d->configChanged();

    d->timer = new QTimer(this);
    d->timer->setSingleShot(true);
    connect(d->timer, SIGNAL(timeout()), this, SLOT(resetDocumentPosition()));

    d->loadedTimer = new QTimer(this);
    d->loadedTimer->setSingleShot(true);
    d->loadedTimer->setInterval(100);
    connect(d->loadedTimer, SIGNAL(timeout()), SIGNAL(loadingFinished()));

    d->savedTimer = new QTimer(this);
    d->savedTimer->setSingleShot(true);
    d->savedTimer->setInterval(100);
    connect(d->savedTimer, SIGNAL(timeout()), SIGNAL(savingFinished()));

    connect(DocumentManager::instance(), SIGNAL(aboutToDeleteDocument()), SLOT(documentAboutToBeDeleted()));
    connect(DocumentManager::instance(), SIGNAL(documentChanged()), SLOT(documentChanged()));
    connect(DocumentManager::instance()->progressProxy(), SIGNAL(valueChanged(int)), SIGNAL(progress(int)));
    connect(DocumentManager::instance(), SIGNAL(documentSaved()), d->savedTimer, SLOT(start()));

    if(DocumentManager::instance()->document())
        documentChanged();
}

KisSketchView::~KisSketchView()
{
    if(d->doc) {
        if (d->useOpenGL) {
#ifdef HAVE_OPENGL
            qobject_cast<KisSketchCanvas*>(d->canvasWidget)->stopRendering();
#endif
        }

        DocumentManager::instance()->closeDocument();
    }
    delete d;
}

QObject* KisSketchView::selectionManager() const
{
    if(!d->view)
        return 0;
    return d->view->selectionManager();
}

QObject* KisSketchView::selectionExtras() const
{
    if (!d->selectionExtras) {
        d->selectionExtras = new KisSelectionExtras(d->view);
    }
    return d->selectionExtras;
}

QObject* KisSketchView::doc() const
{
    return d->doc;
}

QObject* KisSketchView::view() const
{
    return d->view;
}

QString KisSketchView::file() const
{
    return d->file;
}

QString KisSketchView::fileTitle() const
{
    QFileInfo file(d->file);
    return file.fileName();
}

bool KisSketchView::isModified() const
{
    return d->doc->isModified();
}

void KisSketchView::setFile(const QString& file)
{
    if (!file.isEmpty() && file != d->file) {
        d->file = file;
        emit fileChanged();

        if(!file.startsWith("temp://")) {
            DocumentManager::instance()->openDocument(file);
        }
    }
}

void KisSketchView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if(!d->canvasWidget) {
        return;
    }

    if (d->useOpenGL) {
#ifdef HAVE_OPENGL
        qobject_cast<QGLWidget*>(scene()->views().at(0)->viewport())->makeCurrent();

        d->shader->bind();
        d->vertexBuffer->bind();
        d->indexBuffer->bind();

        QMatrix4x4 model;
        model.scale(1.0f, -1.0f);
        d->shader->setUniformValue(d->modelMatrixLocation, model);

        QMatrix4x4 view;
        d->shader->setUniformValue(d->viewMatrixLocation, view);

        QMatrix4x4 projection;
        projection.ortho(0, 1, 0, 1, -1, 1);
        d->shader->setUniformValue(d->projectionMatrixLocation, projection.transposed());

        glBindTexture(GL_TEXTURE_2D, qobject_cast<KisSketchCanvas*>(d->canvasWidget)->texture());
        d->shader->setUniformValue(d->texture0Location, 0);

        d->shader->setUniformValue(d->textureScaleLocation, QVector2D(1.0f, 1.0f));

        d->shader->setAttributeBuffer(d->vertexAttributeLocation, GL_FLOAT, 0, 3);
        d->shader->enableAttributeArray(d->vertexAttributeLocation);
        d->shader->setAttributeBuffer(d->uv0AttributeLocation, GL_FLOAT, 12 * sizeof(float), 2);
        d->shader->enableAttributeArray(d->uv0AttributeLocation);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        d->shader->disableAttributeArray(d->uv0AttributeLocation);
        d->shader->disableAttributeArray(d->vertexAttributeLocation);

        d->indexBuffer->release();
        d->vertexBuffer->release();
        d->shader->release();
#endif
    }
    else {
        if(d->zoomLevelChanged) {
            d->zoomLevelChanged = false;
            d->viewportMoved = false;
            d->prescaledProjection->notifyZoomChanged();
            d->canvasOffset = QPoint();
        } else if(d->viewportMoved) {
            d->viewportMoved = false;

            QPoint newOffset = d->canvas->coordinatesConverter()->imageRectInViewportPixels().topLeft().toPoint();

            if(!d->canvasOffset.isNull()) {
                QPoint moveOffset = newOffset - d->canvasOffset;
                d->prescaledProjection->viewportMoved(moveOffset);
            } else {
                d->prescaledProjection->preScale();
            }

            d->canvasOffset = newOffset;
        }

        const KisCoordinatesConverter *converter = d->canvas->coordinatesConverter();
        QRectF geometry(x(), y(), width(), height());

        painter->save();

        painter->setCompositionMode(QPainter::CompositionMode_Source);
        painter->fillRect(geometry, d->backgroundColor);

        QTransform checkersTransform;
        QPointF brushOrigin;
        QPolygonF polygon;

        painter->setClipRect(geometry);

        converter->getQPainterCheckersInfo(&checkersTransform, &brushOrigin, &polygon);
        painter->setPen(Qt::NoPen);
        painter->setBrush(d->checkers);
        painter->setBrushOrigin(brushOrigin);
        painter->setTransform(checkersTransform);
        painter->drawPolygon(polygon);

        painter->setTransform(converter->viewportToWidgetTransform());

        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter->drawImage(geometry, d->prescaledProjection->prescaledQImage(), geometry);

        painter->setTransform(converter->flakeToWidgetTransform());
        d->canvas->globalShapeManager()->paint(*painter, *converter, false);

        d->canvas->toolProxy()->paint(*painter, *converter);

        KisQPainterCanvas *qc = qobject_cast<KisQPainterCanvas*>(d->canvasWidget);
        Q_FOREACH(KisCanvasDecoration* deco, qc->decorations()) {
            deco->paint(*painter, converter->widgetToDocument(geometry), converter);
        }

        painter->restore();
    }
}

void KisSketchView::componentComplete()
{
    if (d->useOpenGL) {
#ifdef HAVE_OPENGL
        qobject_cast<QGLWidget*>(scene()->views().at(0)->viewport())->makeCurrent();

        d->shader = new QGLShaderProgram(this);
        d->shader->addShaderFromSourceFile(QGLShader::Vertex, KGlobal::dirs()->findResource("data", "krita/shaders/gl2.vert"));
        d->shader->addShaderFromSourceFile(QGLShader::Fragment, KGlobal::dirs()->findResource("data", "krita/shaders/gl2.frag"));
        d->shader->link();

        d->modelMatrixLocation = d->shader->uniformLocation("modelMatrix");
        d->viewMatrixLocation = d->shader->uniformLocation("viewMatrix");
        d->projectionMatrixLocation = d->shader->uniformLocation("projectionMatrix");
        d->texture0Location = d->shader->uniformLocation("texture0");
        d->textureScaleLocation = d->shader->uniformLocation("textureScale");
        d->vertexAttributeLocation = d->shader->attributeLocation("vertex");
        d->uv0AttributeLocation = d->shader->attributeLocation("uv0");

        d->vertexBuffer = new QGLBuffer(QGLBuffer::VertexBuffer);
        d->vertexBuffer->create();
        d->vertexBuffer->bind();

        QVector<float> vertices;
        vertices << 0.0f <<  0.0f << 0.0f;
        vertices << 1.0f <<  0.0f << 0.0f;
        vertices << 1.0f << -1.0f << 0.0f;
        vertices << 0.0f << -1.0f << 0.0f;
        int vertSize = sizeof(float) * vertices.count();

        QVector<float> uvs;
        uvs << 0.f << 0.f;
        uvs << 1.f << 0.f;
        uvs << 1.f << 1.f;
        uvs << 0.f << 1.f;
        int uvSize = sizeof(float) * uvs.count();

        d->vertexBuffer->allocate(vertSize + uvSize);
        d->vertexBuffer->write(0, reinterpret_cast<void*>(vertices.data()), vertSize);
        d->vertexBuffer->write(vertSize, reinterpret_cast<void*>(uvs.data()), uvSize);
        d->vertexBuffer->release();

        d->indexBuffer = new QGLBuffer(QGLBuffer::IndexBuffer);
        d->indexBuffer->create();
        d->indexBuffer->bind();

        QVector<uint> indices;
        indices << 0 << 1 << 2 << 0 << 2 << 3;
        d->indexBuffer->allocate(reinterpret_cast<void*>(indices.data()), indices.size() * sizeof(uint));
        d->indexBuffer->release();
#endif
    }
}

bool KisSketchView::canUndo() const
{
    if(d->undoAction)
        return d->undoAction->isEnabled();
    return false;
}

bool KisSketchView::canRedo() const
{
    if(d->redoAction)
        return d->redoAction->isEnabled();
    return false;
}

void KisSketchView::undo()
{
    d->undoAction->trigger();
}

void KisSketchView::redo()
{
    d->redoAction->trigger();
}

void KisSketchView::zoomIn()
{
    d->view->actionCollection()->action("zoom_in")->trigger();
}

void KisSketchView::zoomOut()
{
    d->view->actionCollection()->action("zoom_out")->trigger();
}

void KisSketchView::save()
{
    DocumentManager::instance()->save();
}

void KisSketchView::saveAs(const QString& fileName, const QString& mimeType)
{
    DocumentManager::instance()->saveAs(fileName, mimeType);
}

void KisSketchView::documentAboutToBeDeleted()
{
    if (d->useOpenGL) {
#ifdef HAVE_OPENGL
        qobject_cast<KisSketchCanvas*>(d->canvasWidget)->stopRendering();
#endif
    }

    if(d->undoAction)
        d->undoAction->disconnect(this);

    if(d->redoAction)
        d->redoAction->disconnect(this);

    KisView2 *oldView = d->view;
    disconnect(d->view, SIGNAL(floatingMessageRequested(QString,QString)), this, SIGNAL(floatingMessageRequested(QString,QString)));
    d->view = 0;
    emit viewChanged();

    delete oldView;

    d->canvas = 0;
    d->canvasWidget = 0;
}

void KisSketchView::documentChanged()
{
    d->doc = DocumentManager::instance()->document();

    connect(d->doc, SIGNAL(modified(bool)), SIGNAL(modifiedChanged()));

    d->view = qobject_cast<KisView2*>(DocumentManager::instance()->part()->createView(QApplication::activeWindow()));
    connect(d->view, SIGNAL(floatingMessageRequested(QString,QString)), this, SIGNAL(floatingMessageRequested(QString,QString)));
    emit viewChanged();
    d->view->canvasControllerWidget()->setGeometry(x(), y(), width(), height());
    d->view->hide();
    d->canvas = d->view->canvasBase();

    qApp->installEventFilter(d->canvas->inputManager());

    d->undoStack = d->doc->undoStack();
    d->undoAction = d->view->actionCollection()->action("edit_undo");
    connect(d->undoAction, SIGNAL(changed()), this, SIGNAL(canUndoChanged()));

    d->redoAction = d->view->actionCollection()->action("edit_redo");
    connect(d->redoAction, SIGNAL(changed()), this, SIGNAL(canRedoChanged()));

    KoToolManager::instance()->switchToolRequested( "KritaShape/KisToolBrush" );

    if (d->useOpenGL) {
#ifdef HAVE_OPENGL
        d->canvasWidget = d->canvas->canvasWidget();
        qobject_cast<KisSketchCanvas*>(d->canvasWidget)->initialize();
        connect(qobject_cast<KisSketchCanvas*>(d->canvasWidget), SIGNAL(renderFinished()), SLOT(update()));
#endif
    }
    else {
        d->canvasWidget = d->canvas->canvasWidget();
        connect(d->doc->image(), SIGNAL(sigImageUpdated(QRect)), SLOT(imageUpdated(QRect)));
        connect(d->view->canvasControllerWidget()->proxyObject, SIGNAL(moveDocumentOffset(QPoint)), SLOT(documentOffsetMoved()));
        connect(d->view->zoomController(), SIGNAL(zoomChanged(KoZoomMode::Mode,qreal)), SLOT(zoomChanged()));
        connect(d->canvas, SIGNAL(updateCanvasRequested(QRect)), SLOT(imageUpdated(QRect)));
        connect(d->doc->image()->signalRouter(), SIGNAL(sigRemoveNodeAsync(KisNodeSP)), SLOT(removeNodeAsync(KisNodeSP)));
    }

    if(!d->prescaledProjection)
        d->prescaledProjection = new KisPrescaledProjection();

    d->prescaledProjection->setCoordinatesConverter(const_cast<KisCoordinatesConverter*>(d->canvas->coordinatesConverter()));
    d->prescaledProjection->setMonitorProfile(d->canvas->monitorProfile(), KoColorConversionTransformation::IntentPerceptual, KoColorConversionTransformation::BlackpointCompensation);
    d->prescaledProjection->setImage(d->canvas->image());

    d->imageUpdated(d->canvas->image()->bounds());

    //    emit progress(100);
    //    emit completed();

    static_cast<KoZoomHandler*>(d->canvas->viewConverter())->setResolution(d->doc->image()->xRes(), d->doc->image()->yRes());
    d->view->zoomController()->setZoomMode(KoZoomMode::ZOOM_PAGE);
    d->view->canvasControllerWidget()->setScrollBarValue(QPoint(0, 0));
    d->view->canvasControllerWidget()->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->view->canvasControllerWidget()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    geometryChanged(QRectF(x(), y(), width(), height()), QRectF());

    d->loadedTimer->start(100);
}

bool KisSketchView::sceneEvent(QEvent* event)
{
    if (d->canvas) {
        switch(event->type()) {
        case QEvent::GraphicsSceneMousePress: {
            d->canvas->inputManager()->setEnabled(true);

            QGraphicsSceneMouseEvent *gsmevent = static_cast<QGraphicsSceneMouseEvent*>(event);
            QMouseEvent mevent(QMouseEvent::MouseButtonPress, gsmevent->pos().toPoint(), gsmevent->button(), gsmevent->buttons(), gsmevent->modifiers());
            QApplication::sendEvent(qApp, &mevent);

            emit interactionStarted();
            return true;
        }
//         case QEvent::GraphicsSceneMouseMove: {
//             QGraphicsSceneMouseEvent *gsmevent = static_cast<QGraphicsSceneMouseEvent*>(event);
//             QMouseEvent mevent(QMouseEvent::MouseMove, gsmevent->pos().toPoint(), gsmevent->button(), gsmevent->buttons(), gsmevent->modifiers());
//             QApplication::sendEvent(d->canvasWidget, &mevent);
//             update();
//             emit interactionStarted();
//             return true;
//         }
//         case QEvent::GraphicsSceneMouseRelease: {
//             QGraphicsSceneMouseEvent *gsmevent = static_cast<QGraphicsSceneMouseEvent*>(event);
//             QMouseEvent mevent(QMouseEvent::MouseButtonRelease, gsmevent->pos().toPoint(), gsmevent->button(), gsmevent->buttons(), gsmevent->modifiers());
//             QApplication::sendEvent(d->canvasWidget, &mevent);
//             emit interactionStarted();
//             return true;
//         }
        case QEvent::GraphicsSceneWheel: {
            d->canvas->inputManager()->setEnabled(true);

            QGraphicsSceneWheelEvent *gswevent = static_cast<QGraphicsSceneWheelEvent*>(event);
            QWheelEvent wevent(gswevent->screenPos(), gswevent->delta(), gswevent->buttons(), gswevent->modifiers(), gswevent->orientation());
            QApplication::sendEvent(qApp, &wevent);

            emit interactionStarted();
            return true;
        }
        case QEvent::TouchBegin: {
            d->canvas->inputManager()->setEnabled(true);
            QApplication::sendEvent(qApp, event);
            event->accept();
            emit interactionStarted();
            return true;
        }
//         default:
//             if(QApplication::sendEvent(d->canvasWidget, event)) {
//                 emit interactionStarted();
//                 return true;
//             }
        }
    }
    return QDeclarativeItem::sceneEvent(event);
}

void KisSketchView::geometryChanged(const QRectF& newGeometry, const QRectF& /*oldGeometry*/)
{
    if (d->canvasWidget && !newGeometry.isEmpty()) {
        d->view->canvasControllerWidget()->setGeometry(newGeometry.toRect());
        if (d->useOpenGL) {
            d->canvasWidget->setGeometry(newGeometry.toRect());
            d->canvasWidget->resize(newGeometry.width(), newGeometry.height());
        }
        const_cast<KisCoordinatesConverter*>(d->canvas->coordinatesConverter())->setCanvasWidgetSize(newGeometry.size().toSize());
        d->prescaledProjection->notifyCanvasSizeChanged(newGeometry.size().toSize());
        d->timer->start(100);
    }
}

void KisSketchView::Private::imageUpdated(const QRect &updated)
{
    if(prescaledProjection && !viewportMoved && !zoomLevelChanged) {
        prescaledProjection->recalculateCache(prescaledProjection->updateCache(updated));

        if(q->scene())
            q->scene()->invalidate( 0, 0, q->width(), q->height() );
    }
}

void KisSketchView::Private::documentOffsetMoved()
{
    if(prescaledProjection) {
        viewportMoved = true;

        if(q->scene())
            q->scene()->invalidate( 0, 0, q->width(), q->height() );
    }
}

void KisSketchView::Private::resetDocumentPosition()
{
    view->zoomController()->setZoomMode(KoZoomMode::ZOOM_PAGE);

    QPoint pos;
    QScrollBar *sb = view->canvasControllerWidget()->horizontalScrollBar();

    pos.rx() = sb->minimum() + (sb->maximum() - sb->minimum()) / 2;

    sb = view->canvasControllerWidget()->verticalScrollBar();
    pos.ry() = sb->minimum() + (sb->maximum() - sb->minimum()) / 2;

    view->canvasControllerWidget()->setScrollBarValue(pos);

    canvasOffset = QPoint();
}

void KisSketchView::Private::configChanged()
{
    KisConfig config;
    backgroundColor = config.canvasBorderColor();

    int checkSize = config.checkSize();
    QImage tile(checkSize * 2, checkSize * 2, QImage::Format_RGB32);
    QPainter pt(&tile);
    pt.fillRect(tile.rect(), Qt::white);
    pt.fillRect(0, 0, checkSize, checkSize, config.checkersColor());
    pt.fillRect(checkSize, checkSize, checkSize, checkSize, config.checkersColor());
    pt.end();

    checkers = QBrush(tile);
}

void KisSketchView::Private::removeNodeAsync(KisNodeSP removedNode)
{
    if(removedNode) {
        imageUpdated(removedNode->extent());
    }
}

void KisSketchView::Private::zoomChanged()
{
    zoomLevelChanged = true;
}

#include "KisSketchView.moc"
