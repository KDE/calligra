/* This file is part of the KDE project
 *
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
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

#include "KarbonShapePainter.h"

#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoZoomHandler.h>
#include <KoUnit.h>
#include <KoShape.h>

#include <QtGui/QImage>

class SimpleCanvas : public KoCanvasBase
{
public:
    SimpleCanvas()
        : KoCanvasBase(0), m_shapeManager( new KoShapeManager( this ) )
        , m_zoomHandler( new KoZoomHandler() )
    {
    }

    ~SimpleCanvas()
    {
        delete m_shapeManager;
        delete m_zoomHandler;
    }

    virtual void gridSize(double *horizontal, double *vertical) const 
    {
        if( horizontal )
            *horizontal = 0;
        if( vertical )
            *vertical = 0;
    };

    virtual bool snapToGrid() const
    {
        return false;
    }

    virtual void addCommand(QUndoCommand *command) {};

    virtual KoShapeManager *shapeManager() const
    {
        return m_shapeManager;
    };

    virtual void updateCanvas(const QRectF& rc) {};

    virtual KoToolProxy * toolProxy() const
    {
        return 0;
    };

    virtual const KoViewConverter *viewConverter() const
    {
        return m_zoomHandler;
    }

    virtual QWidget* canvasWidget()
    {
        return 0;
    };

    virtual KoUnit unit() const
    {
        return KoUnit( KoUnit::Point );
    }

    virtual void updateInputMethodInfo() {};
private:
    KoShapeManager * m_shapeManager;
    KoZoomHandler * m_zoomHandler;
};

class KarbonShapePainter::Private
{
public:
    Private()
    : canvas( new SimpleCanvas() )
    {
    }
    SimpleCanvas * canvas;
};

KarbonShapePainter::KarbonShapePainter()
    : d( new Private() )
{
}

KarbonShapePainter::~KarbonShapePainter()
{
    delete d;
}

void KarbonShapePainter::setShapes( const QList<KoShape*> &shapes )
{
    d->canvas->shapeManager()->setShapes( shapes, false );
}

void KarbonShapePainter::paintShapes( QPainter & painter, KoViewConverter & converter )
{
    d->canvas->shapeManager()->paint( painter, converter, true );
}

bool KarbonShapePainter::paintShapes( QImage & image )
{
    if( image.isNull() )
        return false;

    QRectF bound = contentRect();
    QSizeF size = image.size();

    KoZoomHandler zoomHandler;
    QRectF imageBox = zoomHandler.viewToDocument( QRectF( 0, 0, size.width(), size.height() ) );

    // compute the zoom factor based on the bounding rects in document coordinates
    double zoomW = imageBox.width() / bound.width();
    double zoomH = imageBox.height() / bound.height();
    double zoom = qMin( zoomW, zoomH );

    // now set the zoom into the zoom handler used for painting the shape
    zoomHandler.setZoom( zoom );

    QPainter painter( &image );

    painter.setPen( QPen(Qt::NoPen) );
    painter.setBrush( Qt::NoBrush );
    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate( -bound.x(), -bound.y() );

    paintShapes( painter, zoomHandler );

    return true;
}

QRectF KarbonShapePainter::contentRect()
{
    QRectF bound;
    foreach( KoShape * shape, d->canvas->shapeManager()->shapes() )
    {
        QPainterPath outline = shape->absoluteTransformation(0).map( shape->outline() );
        if( bound.isEmpty() )
            bound = outline.boundingRect();
        else
            bound = bound.united( outline.boundingRect() );
    }
    return bound;
}
