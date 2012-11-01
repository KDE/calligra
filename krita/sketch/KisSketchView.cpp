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

#include <GL/glew.h>

#include <QTimer>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QClipboard>
#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QDebug>

#include <QGLShaderProgram>
#include <QGLBuffer>

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

#include <kundo2stack.h>

#include "ProgressProxy.h"
#include "KisSketchCanvasFactory.h"

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
#include "kis_clipboard.h"
#include <opengl2/kis_gl2_canvas.h>
#include <input/kis_input_manager.h>
#include <kis_canvas_resource_provider.h>
#include <kis_zoom_manager.h>
#include <kis_selection_manager.h>
#include <kis_paint_device.h>
#include <kis_layer.h>
#include <kis_qpainter_canvas.h>
#include <kis_part2.h>

#include "KisSketchCanvas.h"
#include "KisSketchPart.h"
#include "Settings.h"
#include "cpuid.h"
#include "DocumentManager.h"

class KisSketchView::Private
{
public:
    Private( KisSketchView* qq)
        : q(qq)
        , part(0)
        , doc(0)
        , view(0)
        , canvas(0)
        , canvasWidget(0)
        , undoAction(0)
        , redoAction(0)
        , useOpenGL(false)
    { }
    ~Private() { }

    void update();
    void resetDocumentPosition();

    KisSketchView* q;

    KisSketchPart* part;
    KisDoc2* doc;
    KisView2* view;
    KisCanvas2* canvas;
    KUndo2Stack* undoStack;

    QWidget *canvasWidget;

    QString file;

    QGLShaderProgram *shader;
    QGLBuffer *vertexBuffer;
    QGLBuffer *indexBuffer;

    int modelMatrixLocation;
    int viewMatrixLocation;
    int projectionMatrixLocation;
    int texture0Location;
    int textureScaleLocation;
    int vertexAttributeLocation;
    int uv0AttributeLocation;

    QTimer *timer;

    QTimer *loadedTimer;
    QAction* undoAction;
    QAction* redoAction;
    bool useOpenGL;

};

KisSketchView::KisSketchView(QDeclarativeItem* parent)
    : QDeclarativeItem(parent)
    , d(new Private(this))
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setAcceptTouchEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::MiddleButton | Qt::RightButton);

    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);

    KoZoomMode::setMinimumZoom(0.1);
    KoZoomMode::setMaximumZoom(16.0);

    if (d->useOpenGL) {
        KisCanvas2::setCanvasWidgetFactory(new KisSketchCanvasFactory());
    }

    d->timer = new QTimer(this);
    d->timer->setSingleShot(true);
    connect(d->timer, SIGNAL(timeout()), this, SLOT(resetDocumentPosition()));

    d->loadedTimer = new QTimer(this);
    d->loadedTimer->setSingleShot(true);
    connect(d->loadedTimer, SIGNAL(timeout()), SIGNAL(loadingFinished()));

    connect(DocumentManager::instance(), SIGNAL(aboutToDeleteDocument()), SLOT(documentAboutToBeDeleted()));
    connect(DocumentManager::instance(), SIGNAL(documentChanged()), SLOT(documentChanged()));
    connect(DocumentManager::instance()->progressProxy(), SIGNAL(valueChanged(int)), SIGNAL(progress(int)));

    if(DocumentManager::instance()->document())
        documentChanged();
}

KisSketchView::~KisSketchView()
{
    if(d->doc) {
        if (d->useOpenGL) {
            qobject_cast<KisSketchCanvas*>(d->canvasWidget)->stopRendering();
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
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if(!d->canvasWidget) {
        return;
    }

    if(d->view->canvasControllerWidget()->geometry() != QRect(x(), y(), width(), height())) {
        geometryChanged(QRectF(x(), y(), width(), height()), QRectF());
    }

    if (d->useOpenGL) {
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
    }
    else {
        d->canvasWidget->render(painter);
    }
}

void KisSketchView::componentComplete()
{
    if (d->useOpenGL) {
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

void KisSketchView::save()
{
    d->part->save();
    emit floatingMessageRequested("Saved", "file-save");
}

void KisSketchView::saveAs(const QString& fileName, const QString& mimeType)
{
    d->doc->setOutputMimeType(mimeType.toAscii());
    d->part->saveAs(fileName);
    emit floatingMessageRequested(QString("Saved to %1").arg(fileName), "file-save");
}

void KisSketchView::documentAboutToBeDeleted()
{
    if (d->useOpenGL) {
        qobject_cast<KisSketchCanvas*>(d->canvasWidget)->stopRendering();
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

    d->undoStack = d->doc->undoStack();
    d->undoAction = d->view->actionCollection()->action("edit_undo");
    connect(d->undoAction, SIGNAL(changed()), this, SIGNAL(canUndoChanged()));

    d->redoAction = d->view->actionCollection()->action("edit_redo");
    connect(d->redoAction, SIGNAL(changed()), this, SIGNAL(canRedoChanged()));

    KoToolManager::instance()->switchToolRequested( "KritaShape/KisToolBrush" );

    if (d->useOpenGL) {
        d->canvasWidget = d->canvas->canvasWidget();
        qobject_cast<KisSketchCanvas*>(d->canvasWidget)->initialize();
        connect(qobject_cast<KisSketchCanvas*>(d->canvasWidget), SIGNAL(renderFinished()), SLOT(update()));
    }
    else {
        d->canvasWidget = d->canvas->canvasWidget();
        connect(qobject_cast<KisQPainterCanvas*>(d->canvasWidget), SIGNAL(updated()), SLOT(update()));
    }

    static_cast<KoZoomHandler*>(d->canvas->viewConverter())->setResolution(d->doc->image()->xRes(), d->doc->image()->yRes());
    d->view->zoomController()->setZoomMode(KoZoomMode::ZOOM_PAGE);
    d->view->canvasControllerWidget()->setScrollBarValue(QPoint(0, 0));
    d->view->canvasControllerWidget()->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->view->canvasControllerWidget()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //    emit progress(100);
    //    emit completed();

    geometryChanged(QRectF(x(), y(), width(), height()), QRectF());

    d->loadedTimer->start(100);
}

bool KisSketchView::sceneEvent(QEvent* event)
{
    if (d->canvas) {
        switch(event->type()) {
        case QEvent::GraphicsSceneMousePress: {
            QGraphicsSceneMouseEvent *gsmevent = static_cast<QGraphicsSceneMouseEvent*>(event);
            QMouseEvent mevent(QMouseEvent::MouseButtonPress, gsmevent->pos().toPoint(), gsmevent->button(), gsmevent->buttons(), gsmevent->modifiers());
            QApplication::sendEvent(d->canvasWidget, &mevent);
            emit interactionStarted();
            return true;
        }
        case QEvent::GraphicsSceneMouseMove: {
            QGraphicsSceneMouseEvent *gsmevent = static_cast<QGraphicsSceneMouseEvent*>(event);
            QMouseEvent mevent(QMouseEvent::MouseMove, gsmevent->pos().toPoint(), gsmevent->button(), gsmevent->buttons(), gsmevent->modifiers());
            QApplication::sendEvent(d->canvasWidget, &mevent);
            emit interactionStarted();
            return true;
        }
        case QEvent::GraphicsSceneMouseRelease: {
            QGraphicsSceneMouseEvent *gsmevent = static_cast<QGraphicsSceneMouseEvent*>(event);
            QMouseEvent mevent(QMouseEvent::MouseButtonRelease, gsmevent->pos().toPoint(), gsmevent->button(), gsmevent->buttons(), gsmevent->modifiers());
            QApplication::sendEvent(d->canvasWidget, &mevent);
            emit interactionStarted();
            return true;
        }
        case QEvent::GraphicsSceneWheel: {
            QGraphicsSceneWheelEvent *gswevent = static_cast<QGraphicsSceneWheelEvent*>(event);
            QWheelEvent wevent(gswevent->screenPos(), gswevent->delta(), gswevent->buttons(), gswevent->modifiers(), gswevent->orientation());
            QApplication::sendEvent(d->canvasWidget, &wevent);
            emit interactionStarted();
            return true;
        }
        case QEvent::TouchBegin: {
            QApplication::sendEvent(d->canvasWidget, event);
            event->accept();
            emit interactionStarted();
            return true;
        }
        default:
            if(QApplication::sendEvent(d->canvasWidget, event)) {
            emit interactionStarted();
                return true;
        }
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
        d->timer->start(100);
    }
}

void KisSketchView::Private::update()
{
    q->scene()->invalidate( 0, 0, q->width(), q->height() );
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
}

#include "KisSketchView.moc"
