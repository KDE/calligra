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

#include <KoZoomHandler.h>
#include <KoZoomController.h>
#include <KoProgressUpdater.h>
#include <KoToolProxy.h>
#include <KoCanvasController.h>
#include <KoFilterManager.h>
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>
#include <KoUnit.h>

#include "ProgressProxy.h"
#include "KisSketchCanvasFactory.h"

#include "kis_painter.h"
#include "kis_layer.h"
#include "kis_paint_device.h"
#include "kis_doc2.h"
#include "kis_canvas2.h"
#include <kis_canvas_controller.h>
#include "kis_config.h"
#include "kis_view2.h"
#include "kis_image.h"
#include "kis_clipboard.h"
#include <opengl2/kis_gl2_canvas.h>
#include <input/kis_input_manager.h>
#include <kis_canvas_resource_provider.h>
#include <kis_zoom_manager.h>

#include "KisSketchCanvas.h"
#include "Settings.h"

class KisSketchView::Private
{
public:
    Private( KisSketchView* qq)
        : q(qq)
        , doc(0)
        , view(0)
        , canvas(0)
    { }
    ~Private() { }

    void update();
    void updateCanvas();
    void updatePanGesture(const QPointF &location);
    void documentOffsetMoved(QPoint newOffset);

    KisSketchView* q;

    KisDoc2* doc;
    KisView2* view;
    KisCanvas2* canvas;

    Settings *settings;
    KisSketchCanvas *canvasWidget;

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
};

void KisSketchView::Private::update()
{
    q->scene()->invalidate( 0, 0, q->width(), q->height() );
}

KisSketchView::KisSketchView(QDeclarativeItem* parent)
    : QDeclarativeItem(parent)
    , d(new Private(this))
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::MiddleButton | Qt::RightButton);

    KoZoomMode::setMinimumZoom(0.1);
    KoZoomMode::setMaximumZoom(16.0);

    // make sure we use the opengl canvas
    KisConfig cfg;
    cfg.setUseOpenGL(true);
    cfg.setUseOpenGLShaders(true);
    cfg.setUseOpenGLTrilinearFiltering(true);
}

KisSketchView::~KisSketchView()
{
    delete d;
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

void KisSketchView::setFile(const QString& file)
{
    if (file != d->file) {
        d->file = file;
        emit fileChanged();
    }
}

void KisSketchView::createDocument()
{
    KisDoc2* doc = new KisDoc2();
    d->doc = doc;

    // create an empty document

    if (d->settings->useClipBoard() && KisClipboard::instance()->hasClip()) {

        KisConfig cfg;
        cfg.setPasteBehaviour(PASTE_ASSUME_MONITOR);

        QSize sz = KisClipboard::instance()->clipSize();
        KisPaintDeviceSP clipDevice = KisClipboard::instance()->clip(QPoint(0,0));

        d->doc->newImage("From Clipboard", sz.width(), sz.height(), clipDevice->colorSpace());

        KisImageWSP image = d->doc->image();
        if (image && image->root() && image->root()->firstChild()) {
            KisLayer * layer = dynamic_cast<KisLayer*>(image->root()->firstChild().data());
            Q_ASSERT(layer);
            layer->setOpacity(OPACITY_OPAQUE_U8);
            QRect r = clipDevice->exactBounds();
            KisPainter painter;
            painter.begin(layer->paintDevice());
            painter.setCompositeOp(COMPOSITE_COPY);
            painter.bitBlt(QPoint(0, 0), clipDevice, r);
            layer->setDirty(QRect(0, 0, sz.width(), sz.height()));
        }
    }
    else if (d->settings->useWebCam()) {
    }
    else if (d->file.isEmpty()) {
        d->doc->newImage("test", d->settings->imageWidth(), d->settings->imageHeight(), KoColorSpaceRegistry::instance()->rgb8());
        d->doc->image()->setResolution(d->settings->imageResolution() / 72.0, d->settings->imageResolution() / 72.0);
    }
    else if (!d->file.isEmpty()){
        //    emit progress(1);

        KisDoc2* doc = new KisDoc2();
        d->doc = doc;

        //    ProgressProxy *proxy = new ProgressProxy(this);
        //    doc->setProgressProxy(proxy);
        //    connect(proxy, SIGNAL(valueChanged(int)), SIGNAL(progress(int)));

        KMimeType::Ptr type = KMimeType::findByPath(d->file);
        QString path = d->file;

        if (type->name() != doc->nativeFormatMimeType()) {
            KoFilterManager *manager = new KoFilterManager(doc,  doc->progressUpdater());
            //manager->setBatchMode(true);
            KoFilter::ConversionStatus status;
            path = manager->importDocument(KUrl(d->file).toLocalFile(), type->name(), status);
        }

        doc->openUrl(KUrl(path));

        //    emit progress(100);
        //    emit completed();

    }

    KisCanvas2::setCanvasWidgetFactory(new KisSketchCanvasFactory());

    d->view = qobject_cast<KisView2*>(d->doc->createView(QApplication::activeWindow()));
    emit viewChanged();
    d->view->canvasControllerWidget()->setGeometry(x(), y(), width(), height());
    d->view->hide();
    d->canvas = d->view->canvasBase();

    KoToolManager::instance()->switchToolRequested( "KritaShape/KisToolBrush" );

    d->canvasWidget = qobject_cast<KisSketchCanvas*>(d->canvas->canvasWidget());
    d->canvasWidget->initialize();
    connect(d->canvasWidget, SIGNAL(renderFinished()), SLOT(update()));

    static_cast<KoZoomHandler*>(d->canvas->viewConverter())->setResolution(d->doc->image()->xRes(), d->doc->image()->yRes());
    d->view->zoomController()->setZoomMode(KoZoomMode::ZOOM_PAGE);
    d->view->canvasControllerWidget()->setScrollBarValue(QPoint(0, 0));

    d->timer = new QTimer(this);
    d->timer->setSingleShot(true);
    connect(d->timer, SIGNAL(timeout()), this, SLOT(resetDocumentPosition()));
}

QObject *KisSketchView::settings()
{
    return d->settings;
}

void KisSketchView::setSettings(QObject *settings)
{
    d->settings = qobject_cast<Settings*>(settings);
}

void KisSketchView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(widget)

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

    glBindTexture(GL_TEXTURE_2D, d->canvasWidget->texture());
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

void KisSketchView::componentComplete()
{
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

bool KisSketchView::sceneEvent(QEvent* event)
{
    if (d->canvas) {
        switch(event->type()) {
        case QEvent::GraphicsSceneMousePress: {
            QGraphicsSceneMouseEvent *gsmevent = static_cast<QGraphicsSceneMouseEvent*>(event);
            QMouseEvent *mevent = new QMouseEvent(QMouseEvent::MouseButtonPress, gsmevent->pos().toPoint(), gsmevent->button(), gsmevent->buttons(), gsmevent->modifiers());
            d->canvas->inputManager()->eventFilter(d->canvas, mevent);
            return true;
        }
        case QEvent::GraphicsSceneMouseMove: {
            QGraphicsSceneMouseEvent *gsmevent = static_cast<QGraphicsSceneMouseEvent*>(event);
            QMouseEvent *mevent = new QMouseEvent(QMouseEvent::MouseMove, gsmevent->pos().toPoint(), gsmevent->button(), gsmevent->buttons(), gsmevent->modifiers());
            d->canvas->inputManager()->eventFilter(d->canvas, mevent);
            return true;
        }
        case QEvent::GraphicsSceneMouseRelease: {
            QGraphicsSceneMouseEvent *gsmevent = static_cast<QGraphicsSceneMouseEvent*>(event);
            QMouseEvent *mevent = new QMouseEvent(QMouseEvent::MouseButtonRelease, gsmevent->pos().toPoint(), gsmevent->button(), gsmevent->buttons(), gsmevent->modifiers());
            d->canvas->inputManager()->eventFilter(d->canvas, mevent);
            return true;
        }
        case QEvent::GraphicsSceneWheel: {
            QGraphicsSceneWheelEvent *gswevent = static_cast<QGraphicsSceneWheelEvent*>(event);
            QWheelEvent *wevent = new QWheelEvent(gswevent->screenPos(), gswevent->delta(), gswevent->buttons(), gswevent->modifiers(), gswevent->orientation());
            d->canvas->inputManager()->eventFilter(d->canvas, wevent);
        }
        default:
            break;
        }
    }
    return QDeclarativeItem::sceneEvent(event);
}

void KisSketchView::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (d->canvasWidget)
    {
        d->canvasWidget->setGeometry(newGeometry.toRect());
        d->view->canvasControllerWidget()->setGeometry(newGeometry.toRect());
        d->canvasWidget->resize(newGeometry.width(), newGeometry.height());

        d->timer->start(100);
    }
}

void KisSketchView::resetDocumentPosition()
{
    d->view->zoomController()->setZoomMode(KoZoomMode::ZOOM_PAGE);

    QPoint pos;
    QScrollBar *sb = d->view->canvasControllerWidget()->horizontalScrollBar();

    pos.rx() = sb->minimum() + (sb->maximum() - sb->minimum()) / 2;

    sb = d->view->canvasControllerWidget()->verticalScrollBar();
    pos.ry() = sb->minimum() + (sb->maximum() - sb->minimum()) / 2;

    d->view->canvasControllerWidget()->setScrollBarValue(pos);
}


#include "KisSketchView.moc"
