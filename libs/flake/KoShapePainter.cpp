/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoShapePainter.h"

#include "KoCanvasBase.h"
#include "KoShape.h"
#include "KoShapeContainer.h"
#include "KoShapeGroup.h"
#include "KoShapeManager.h"
#include "KoShapeManagerPaintingStrategy.h"
#include "KoShapeStrokeModel.h"
#include "KoViewConverter.h"

#include <KoUnit.h>

#include <QImage>
#include <QPainter>

class SimpleCanvas : public KoCanvasBase
{
public:
    SimpleCanvas()
        : KoCanvasBase(nullptr)
        , m_shapeManager(new KoShapeManager(this))
    {
    }

    ~SimpleCanvas() override
    {
        delete m_shapeManager;
    }

    void gridSize(qreal *horizontal, qreal *vertical) const override
    {
        if (horizontal)
            *horizontal = 0;
        if (vertical)
            *vertical = 0;
    };

    bool snapToGrid() const override
    {
        return false;
    }

    void addCommand(KUndo2Command *) override
    {
    }

    KoShapeManager *shapeManager() const override
    {
        return m_shapeManager;
    }

    void updateCanvas(const QRectF &) override
    {
    }

    KoToolProxy *toolProxy() const override
    {
        return nullptr;
    }

    KoViewConverter *viewConverter() const override
    {
        return nullptr;
    }

    QWidget *canvasWidget() override
    {
        return nullptr;
    }

    const QWidget *canvasWidget() const override
    {
        return nullptr;
    }

    KoUnit unit() const override
    {
        return KoUnit(KoUnit::Point);
    }

    void updateInputMethodInfo() override
    {
    }

    void setCursor(const QCursor &) override
    {
    }

private:
    KoShapeManager *m_shapeManager;
};

class Q_DECL_HIDDEN KoShapePainter::Private
{
public:
    Private()
        : canvas(new SimpleCanvas())
    {
    }

    ~Private()
    {
        delete canvas;
    }
    SimpleCanvas *canvas;
};

KoShapePainter::KoShapePainter(KoShapeManagerPaintingStrategy *strategy)
    : d(new Private())
{
    if (strategy) {
        strategy->setShapeManager(d->canvas->shapeManager());
        d->canvas->shapeManager()->setPaintingStrategy(strategy);
    }
}

KoShapePainter::~KoShapePainter()
{
    delete d;
}

void KoShapePainter::setShapes(const QList<KoShape *> &shapes)
{
    d->canvas->shapeManager()->setShapes(shapes, KoShapeManager::AddWithoutRepaint);
}

void KoShapePainter::paint(QPainter &painter, KoViewConverter &converter)
{
    foreach (KoShape *shape, d->canvas->shapeManager()->shapes()) {
        shape->waitUntilReady(converter, false);
    }

    d->canvas->shapeManager()->paint(painter, converter, true);
}

void KoShapePainter::paint(QPainter &painter, const QRect &painterRect, const QRectF &documentRect)
{
    if (documentRect.width() == 0.0f || documentRect.height() == 0.0f)
        return;

    KoViewConverter converter;
    // calculate the painter destination rectangle size in document coordinates
    QRectF paintBox = converter.viewToDocument(QRectF(QPointF(), painterRect.size()));

    // compute the zoom factor based on the bounding rects in document coordinates
    // so that the content fits into the image
    qreal zoomW = paintBox.width() / documentRect.width();
    qreal zoomH = paintBox.height() / documentRect.height();
    qreal zoom = qMin(zoomW, zoomH);

    // now set the zoom into the zoom handler used for painting the shape
    converter.setZoom(zoom);

    painter.save();

    // initialize painter
    painter.setPen(QPen(Qt::NoPen));
    painter.setBrush(Qt::NoBrush);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setClipRect(painterRect.adjusted(-1, -1, 1, 1));

    // convert document rectangle to view coordinates
    QRectF zoomedBound = converter.documentToView(documentRect);
    // calculate offset between painter rectangle and converted document rectangle
    QPointF offset = QRectF(painterRect).center() - zoomedBound.center();
    // center content in painter rectangle
    painter.translate(offset.x(), offset.y());

    // finally paint the shapes
    paint(painter, converter);

    painter.restore();
}

void KoShapePainter::paint(QImage &image)
{
    if (image.isNull())
        return;

    QPainter painter(&image);

    paint(painter, image.rect(), contentRect());
}

QRectF KoShapePainter::contentRect() const
{
    QRectF bound;
    foreach (KoShape *shape, d->canvas->shapeManager()->shapes()) {
        if (!shape->isVisible(true))
            continue;
        if (dynamic_cast<KoShapeGroup *>(shape))
            continue;

        QRectF shapeRect = shape->boundingRect();

        if (bound.isEmpty())
            bound = shapeRect;
        else
            bound = bound.united(shapeRect);
    }
    return bound;
}
