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

#include <QGLShaderProgram>
#include <QGLBuffer>

#include <kdebug.h>
#include <kmimetype.h>
#include <kstandarddirs.h>

#include <KoZoomHandler.h>
#include <KoZoomController.h>
#include <KoProgressUpdater.h>
#include <KoToolProxy.h>
#include <KoFilterManager.h>
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>

#include "ProgressProxy.h"

#include "kis_doc2.h"
#include "kis_canvas2.h"
#include "kis_config.h"
#include "kis_view2.h"
#include <opengl2/kis_gl2_canvas.h>

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

    KisGL2Canvas* glCanvas;

    QGLShaderProgram *shader;
    QGLBuffer *vertexBuffer;
    QGLBuffer *indexBuffer;

    int modelMatrixLocation;
    int viewMatrixLocation;
    int projectionMatrixLocation;
    int texture0Location;
    int vertexAttributeLocation;
    int uv0AttributeLocation;
};

void KisSketchView::Private::update()
{
    //
}



KisSketchView::KisSketchView(QDeclarativeItem* parent)
    : CanvasControllerDeclarative(parent)
    , d(new Private(this))
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);

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

void KisSketchView::createDocument()
{
    KPluginFactory* factory = KLibLoader::self()->factory("kritapart");
    d->doc = static_cast<KisDoc2*>(factory->create(this, "KritaPart"));
    d->doc->newImage("test", 1000, 100, KoColorSpaceRegistry::instance()->rgb8());
    d->view = qobject_cast<KisView2*>(d->doc->createView());
    //d->view->hide();
    d->canvas = d->view->canvasBase();
    d->canvas->setCanvasItem(this);

    setCanvas(d->canvas);
    connect(d->canvas, SIGNAL(documentSize(QSizeF)), zoomController(), SLOT(setDocumentSize(QSizeF)));
    resetDocumentOffset();
    d->canvas->updateCanvas(QRectF(0, 0, width(), height()));

    d->glCanvas = qobject_cast<KisGL2Canvas*>(d->canvas->canvasWidget());
    //const_cast<QGLContext*>(d->glCanvas->context())->
}

void KisSketchView::loadDocument()
{
/*
    emit progress(1);

    KisDoc2* doc = new KisDoc2();
    d->doc = doc;

    ProgressProxy *proxy = new ProgressProxy(this);
    doc->setProgressProxy(proxy);
    connect(proxy, SIGNAL(valueChanged(int)), SIGNAL(progress(int)));

    KMimeType::Ptr type = KMimeType::findByPath(file());
    QString path = file();

    if (type->name() != doc->nativeFormatMimeType()) {
        KoFilterManager *manager = new KoFilterManager(doc,  doc->progressUpdater());
        //manager->setBatchMode(true);
        KoFilter::ConversionStatus status;
        path = manager->importDocument(KUrl(file()).toLocalFile(), type->name(), status);
    }

    doc->openUrl(KUrl(path));

    setMargin(10);
    d->updateCanvas();

    QList<QTextDocument*> texts;
    KoFindText::findTextInShapes(d->canvas->shapeManager()->shapes(), texts);

    d->find = new KoFindText(texts, this);
    connect(d->find, SIGNAL(matchFound(KoFindMatch)), this, SLOT(matchFound(KoFindMatch)));
    connect(d->find, SIGNAL(updateCanvas()), this, SLOT(update()));

    emit progress(100);
    emit completed();
*/
}

void KisSketchView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (GLEW_GREMEDY_string_marker) {
        glStringMarkerGREMEDY(0, Q_FUNC_INFO);
    }

    d->canvas->updateCanvas();

    const_cast<QGLContext*>(qobject_cast<QGLWidget*>(scene()->views().at(0)->viewport())->context())->makeCurrent();

    d->shader->bind();
    d->vertexBuffer->bind();
    d->indexBuffer->bind();

    QMatrix4x4 model;
    d->shader->setUniformValue(d->modelMatrixLocation, model);

    QMatrix4x4 view;
    d->shader->setUniformValue(d->viewMatrixLocation, view);

    QMatrix4x4 projection;
    projection.ortho(0, 1, -1, 0, -1, 1);
    d->shader->setUniformValue(d->projectionMatrixLocation, projection.transposed());

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, d->glCanvas->framebufferTexture());
    d->shader->setUniformValue(d->texture0Location, 4);

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
//     qDebug() << Q_FUNC_INFO;
//     qDebug() << width() << height();

    const_cast<QGLContext*>(qobject_cast<QGLWidget*>(scene()->views().at(0)->viewport())->context())->makeCurrent();

    d->shader = new QGLShaderProgram(this);
    d->shader->addShaderFromSourceFile(QGLShader::Vertex, KGlobal::dirs()->findResource("data", "krita/shaders/gl2.vert"));
    d->shader->addShaderFromSourceFile(QGLShader::Fragment, KGlobal::dirs()->findResource("data", "krita/shaders/gl2.frag"));
    d->shader->link();

    d->modelMatrixLocation = d->shader->uniformLocation("modelMatrix");
    d->viewMatrixLocation = d->shader->uniformLocation("viewMatrix");
    d->projectionMatrixLocation = d->shader->uniformLocation("projectionMatrix");
    d->texture0Location = d->shader->uniformLocation("texture0");
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

    createDocument();
}

void KisSketchView::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    d->glCanvas->setGeometry(newGeometry.toRect());
}

void KisSketchView::onSingleTap( const QPointF& location)
{
    Q_UNUSED(location);
}

void KisSketchView::onDoubleTap ( const QPointF& location)
{
    Q_UNUSED(location);
    emit doubleTapped();
}

void KisSketchView::onLongTap ( const QPointF& location)
{
    Q_UNUSED(location);
}

void KisSketchView::onLongTapEnd(const QPointF &location)
{
    Q_UNUSED(location);
}

QPointF KisSketchView::documentToView(const QPointF& point)
{
//    return d->canvas->viewMode()->documentToView(point, d->canvas->viewConverter());
    return QPointF();
}

QPointF KisSketchView::viewToDocument(const QPointF& point)
{
//    return d->canvas->viewMode()->viewToDocument(point, d->canvas->viewConverter());
    return QPointF();
}

#include "KisSketchView.moc"
