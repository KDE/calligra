/* This file is part of the KDE project
 * Copyright (c) 2003 thierry lorthiois <lorthioist@wanadoo.fr>
 * Copyright (c) 2007-2008 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
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

#include "wmfimportparser.h"

#include <KarbonDocument.h>

#include <KoPathShape.h>
#include <KoLineBorder.h>
#include <KoShapeLayer.h>
#include <KoImageData.h>
#include <KoImageCollection.h>
#include <KoColorBackground.h>
#include <KoGradientBackground.h>
#include <KoPatternBackground.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>

#include <pathshapes/rectangle/RectangleShape.h>
#include <pathshapes/ellipse/EllipseShape.h>
#include <artistictextshape/ArtisticTextShape.h>
#include <kdebug.h>

/*
bug : see motar.wmf
*/

WMFImportParser::WMFImportParser() : KoWmfRead()
{
}


bool WMFImportParser::play(KarbonDocument& doc)
{
    mDoc = &doc;
    mScaleX = mScaleY = 1;

    // Play the wmf file
    if (! KoWmfRead::play())
        return false;

    KoShapeLayer * layer = 0;
    // check if we have to insert a default layer
    if (mDoc->layers().count() == 0) {
        layer = new KoShapeLayer();
        mDoc->insertLayer(layer);
    } else
        layer = mDoc->layers().first();

    uint zIndex = 0;
    // add all toplevel shapes to the layer
    foreach(KoShape * shape, mDoc->shapes()) {
        shape->setZIndex(zIndex++);
        if (! shape->parent())
            layer->addChild(shape);
    }

    return true;
}


//-----------------------------------------------------------------------------
// Virtual Painter

bool WMFImportParser::begin()
{
    QRect bounding = boundingRect();

    mBackgroundMode = Qt::TransparentMode;
    mCurrentOrg.setX(bounding.left());
    mCurrentOrg.setY(bounding.top());

    if (isStandard()) {
        //mDoc->setUnit(KoUnit(KoUnit::Point));
        mDoc->setPageSize(bounding.size());
    } else {
        // Placeable Wmf store the boundingRect() in pixel and the default DPI
        // The placeable format doesn't have information on which Unit to use
        // so we choose millimeters by default
        //mDoc->setUnit(KoUnit(KoUnit::Millimeter));
        mDoc->setPageSize(QSizeF(INCH_TO_POINT((double)bounding.width() / defaultDpi()),
                                 INCH_TO_POINT((double)bounding.height() / defaultDpi())));
    }
    if ((bounding.width() != 0) && (bounding.height() != 0)) {
        mScaleX = mDoc->pageSize().width() / (double)bounding.width();
        mScaleY = mDoc->pageSize().height() / (double)bounding.height();
    }

    kDebug(30504) << "bounding rect =" << bounding;
    kDebug(30504) << "page size =" << mDoc->pageSize();
    kDebug(30504) << "scale x =" << mScaleX;
    kDebug(30504) << "scale y =" << mScaleY;

    return true;
}


bool WMFImportParser::end()
{
    return true;
}


void WMFImportParser::save()
{
}


void WMFImportParser::restore()
{
}


void WMFImportParser::setFont(const QFont &font)
{
    mFont = font;
}


void WMFImportParser::setPen(const QPen &pen)
{
    mPen = pen;
}


void WMFImportParser::setTextPen(const QPen &pen)
{
    mTextPen = pen;
}


const QPen &WMFImportParser::pen() const
{
    return mPen;
}


void WMFImportParser::setBrush(const QBrush &brush)
{
    mBrush = brush;
}


void WMFImportParser::setBackgroundColor(const QColor &c)
{
    mBackgroundColor = c;
}


void WMFImportParser::setBackgroundMode(Qt::BGMode mode)
{
    mBackgroundMode = mode;
}



void WMFImportParser::setWindowOrg(int left, int top)
{
    mCurrentOrg.setX(left);
    mCurrentOrg.setY(top);
}


void WMFImportParser::setWindowExt(int width, int height)
{
    // the wmf file can change width/height during the drawing
    if ((width != 0) && (height != 0)) {
        mScaleX = mDoc->pageSize().width() / (double)width;
        mScaleY = mDoc->pageSize().height() / (double)height;
    }
}


void WMFImportParser::setMatrix(const QMatrix &matrix, bool combine)
{
    kDebug(30504) << "matrix =" << matrix;
    kDebug(30504) << "combine =" << combine;
}


void WMFImportParser::setClipRegion(const QRegion &)
{
}


QRegion WMFImportParser::clipRegion()
{
    return mClippingRegion;
}


void WMFImportParser::moveTo(int left, int top)
{
    mCurrentPoint.setX(left);
    mCurrentPoint.setY(top);
}


void WMFImportParser::lineTo(int left, int top)
{
    KoPathShape * line = static_cast<KoPathShape*>(createShape(KoPathShapeId));
    if (! line)
        return;

    line->moveTo(QPointF(coordX(mCurrentPoint.x()), coordY(mCurrentPoint.y())));
    line->lineTo(QPointF(coordX(left), coordY(top)));
    line->normalize();

    appendPen(*line);

    mDoc->add(line);
    mCurrentPoint.setX(left);
    mCurrentPoint.setY(top);
}


void WMFImportParser::drawRect(int left, int top, int width, int height)
{
    QRectF bound = QRectF(QPointF(coordX(left), coordY(top)), QSizeF(scaleW(width), scaleH(height))).normalized();

    RectangleShape * rectangle = static_cast<RectangleShape*>(createShape(RectangleShapeId));
    if (! rectangle)
        return;

    rectangle->setPosition(bound.topLeft());
    rectangle->setSize(bound.size());

    appendPen(*rectangle);
    appendBrush(*rectangle);

    mDoc->add(rectangle);
}


void WMFImportParser::drawRoundRect(int left, int top, int width, int height, int roundw, int roundh)
{
    QRectF bound = QRectF(QPointF(coordX(left), coordY(top)), QSizeF(scaleW(width), scaleH(height))).normalized();

    RectangleShape * rectangle = static_cast<RectangleShape*>(createShape(RectangleShapeId));
    if (! rectangle)
        return;

    rectangle->setPosition(bound.topLeft());
    rectangle->setSize(bound.size());
    rectangle->setCornerRadiusX(2.0 * qAbs(roundw));
    rectangle->setCornerRadiusY(2.0 * qAbs(roundh));

    appendPen(*rectangle);
    appendBrush(*rectangle);

    mDoc->add(rectangle);
}


void WMFImportParser::drawEllipse(int left, int top, int width, int height)
{
    QRectF bound = QRectF(QPointF(coordX(left), coordY(top)), QSizeF(scaleW(width), scaleH(height))).normalized();

    EllipseShape *ellipse = static_cast<EllipseShape*>(createShape(EllipseShapeId));
    if (! ellipse)
        return;

    ellipse->setPosition(bound.topLeft());
    ellipse->setSize(bound.size());

    appendPen(*ellipse);
    appendBrush(*ellipse);

    mDoc->add(ellipse);
}


void WMFImportParser::drawArc(int x, int y, int w, int h, int aStart, int aLen)
{
    double start = (aStart * 180) / 2880.0;
    double end = (aLen * 180) / 2880.0;
    end += start;

    QRectF bound = QRectF(QPointF(coordX(x), coordY(y)), QSizeF(scaleW(w), scaleH(h))).normalized();

    EllipseShape * arc = static_cast<EllipseShape*>(createShape(EllipseShapeId));
    if (! arc)
        return;

    arc->setType(EllipseShape::Arc);
    arc->setStartAngle(start);
    arc->setEndAngle(end);
    arc->setPosition(bound.topLeft());
    arc->setSize(bound.size());

    appendPen(*arc);
    //appendBrush( *arc );

    mDoc->add(arc);
}


void WMFImportParser::drawPie(int x, int y, int w, int h, int aStart, int aLen)
{
    double start = (aStart * 180) / 2880.0;
    double end = (aLen * 180) / 2880.0;
    end += start;

    QRectF bound = QRectF(QPointF(coordX(x), coordY(y)), QSizeF(scaleW(w), scaleH(h))).normalized();

    EllipseShape * pie = static_cast<EllipseShape*>(createShape(EllipseShapeId));
    if (! pie)
        return;

    pie->setType(EllipseShape::Pie);
    pie->setStartAngle(start);
    pie->setEndAngle(end);
    pie->setPosition(bound.topLeft());
    pie->setSize(bound.size());

    appendPen(*pie);
    appendBrush(*pie);

    mDoc->add(pie);
}


void WMFImportParser::drawChord(int x, int y, int w, int h, int aStart, int aLen)
{
    double start = (aStart * 180) / 2880.0;
    double end = (aLen * 180) / 2880.0;
    end += start;

    QRectF bound = QRectF(QPointF(coordX(x), coordY(y)), QSizeF(scaleW(w), scaleH(h))).normalized();

    EllipseShape * chord = static_cast<EllipseShape*>(createShape(EllipseShapeId));
    if (! chord)
        return;

    chord->setType(EllipseShape::Chord);
    chord->setStartAngle(start);
    chord->setEndAngle(end);
    chord->setPosition(bound.topLeft());
    chord->setSize(bound.size());

    appendPen(*chord);
    appendBrush(*chord);

    mDoc->add(chord);
}


void WMFImportParser::drawPolyline(const QPolygon &pa)
{
    KoPathShape *polyline = static_cast<KoPathShape*>(createShape(KoPathShapeId));
    if (! polyline)
        return;

    appendPen(*polyline);
    appendPoints(*polyline, pa);

    mDoc->add(polyline);
}


void WMFImportParser::drawPolygon(const QPolygon &pa, bool winding)
{
    KoPathShape *polygon = static_cast<KoPathShape*>(createShape(KoPathShapeId));
    if (! polygon)
        return;

    appendPen(*polygon);
    appendBrush(*polygon);
    appendPoints(*polygon, pa);

    polygon->close();
    polygon->setFillRule(winding ? Qt::WindingFill : Qt::OddEvenFill);
    mDoc->add(polygon);
}


void WMFImportParser::drawPolyPolygon(QList<QPolygon>& listPa, bool winding)
{
    KoPathShape *path = static_cast<KoPathShape*>(createShape(KoPathShapeId));
    if (! path)
        return;

    if (listPa.count() > 0) {
        appendPen(*path);
        appendBrush(*path);
        appendPoints(*path, listPa.first());
        path->close();
        path->setFillRule(winding ? Qt::WindingFill : Qt::OddEvenFill);
        foreach(const QPolygon & pa, listPa) {
            KoPathShape *newPath = static_cast<KoPathShape*>(createShape(KoPathShapeId));
            if (! newPath)
                continue;

            appendPoints(*newPath, pa);
            newPath->close();
            path->combine(newPath);
        }

        mDoc->add(path);
    }
}


void WMFImportParser::drawImage(int x, int y, const QImage &image, int sx, int sy, int sw, int sh)
{
    KoImageData * data = mDoc->imageCollection()->createImageData(image);
    if (! data)
        return;

    KoShape * pic = createShape("PictureShape");
    if (! pic)
        return;

    pic->setUserData(data);
    pic->setPosition(QPointF(x, y));
    if (sw < 0)
        sw = image.width();
    if (sh < 0)
        sh = image.height();
    pic->setSize(QSizeF(sw, sh));

    kDebug(30504) << "image data size =" << data->pixmap().size();
    kDebug(30504) << "source image size =" << image.size();
    kDebug(30504) << "source position =" << QPointF(sx, sy);
    kDebug(30504) << "source size =" << QPointF(scaleW(sw), scaleH(sh));

    mDoc->add(pic);
}


void WMFImportParser::patBlt(int x, int y, int width, int height, quint32 rasterOperation)
{
    // Not Yet Implemented
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(width);
    Q_UNUSED(height);
    Q_UNUSED(rasterOperation);
}



void WMFImportParser::drawText(int x, int y, int , int , int flags, const QString& text, double rotation)
{
    enum TextFlags { CurrentPosition = 0x01, AlignHCenter = 0x06, AlignBottom = 0x08 };

    if (flags & CurrentPosition) {
        // (left, top) position = current logical position
        x = mCurrentPoint.x();
        y = mCurrentPoint.y();
    }

    // adjust font size
    QFont font = mFont;
    font.setPointSizeF(coordY(mFont.pointSize()));

    ArtisticTextShape * textShape = static_cast<ArtisticTextShape*>(createShape(ArtisticTextShapeID));
    if (! textShape)
        return;

    textShape->setFont(font);
    textShape->setText(text);

    // determine y-offset from given baseline position
    qreal yOffset = 0.0;
    if (flags & AlignBottom)
        yOffset -= textShape->baselineOffset();

    textShape->setPosition(QPointF(coordX(x), coordY(y) + yOffset));

    // set text anchor
    qreal xOffset = 0.0;
    if (flags & AlignHCenter) {
        textShape->setTextAnchor(ArtisticTextShape::AnchorMiddle);
        xOffset = -0.5 * textShape->size().width();
    }

    if (rotation) {
        // we rotate around the anchor point
        QPointF anchor(-xOffset, -yOffset);
        QMatrix matrix;
        matrix.translate(anchor.x(), anchor.y());
        matrix.rotate(rotation);
        matrix.translate(-anchor.x(), -anchor.y());
        textShape->applyTransformation(matrix);
    }

    textShape->setBackground(new KoColorBackground(mTextPen.color()));

    mDoc->add(textShape);
}


//-----------------------------------------------------------------------------
// Utilities

void WMFImportParser::appendPen(KoShape& obj)
{
    double width = mPen.width() * mScaleX;

    KoLineBorder * border = new KoLineBorder(((width < 0.99) ? 1 : width), mPen.color());
    border->setLineStyle(mPen.style(), mPen.dashPattern());
    border->setCapStyle(mPen.capStyle());
    border->setJoinStyle(mPen.joinStyle());

    obj.setBorder(border);
}


void WMFImportParser::appendBrush(KoShape& obj)
{
    switch (mBrush.style()) {
    case Qt::NoBrush:
        obj.setBackground(0);
        break;
    case Qt::TexturePattern: {
        KoImageCollection * imageCollection = mDoc->imageCollection();
        if (imageCollection) {
            KoPatternBackground * bg = new KoPatternBackground(imageCollection);
            bg->setPattern(mBrush.textureImage());
            bg->setMatrix(mBrush.matrix());
            obj.setBackground(bg);
        }
        break;
    }
    case Qt::LinearGradientPattern:
    case Qt::RadialGradientPattern:
    case Qt::ConicalGradientPattern: {
        KoGradientBackground * bg = new KoGradientBackground(*mBrush.gradient());
        bg->setMatrix(mBrush.matrix());
        obj.setBackground(bg);
        break;
    }
    default:
        obj.setBackground(new KoColorBackground(mBrush.color(), mBrush.style()));
    }
}

void  WMFImportParser::setCompositionMode(QPainter::CompositionMode)
{
    //TODO
}

void WMFImportParser::appendPoints(KoPathShape &path, const QPolygon& pa)
{
    // list of point array
    if (pa.size() > 0) {
        path.moveTo(QPointF(coordX(pa.point(0).x()), coordY(pa.point(0).y())));
    }
    for (int i = 1 ; i < pa.size() ; i++) {
        path.lineTo(QPointF(coordX(pa.point(i).x()), coordY(pa.point(i).y())));
    }
    path.normalize();
}

double WMFImportParser::coordX(int left)
{
    return ((double)(left - mCurrentOrg.x()) * mScaleX);
}

double WMFImportParser::coordY(int top)
{
    return ((double)(top - mCurrentOrg.y()) * mScaleY);
}

double WMFImportParser::scaleW(int width)
{
    return (width * mScaleX);
}

double WMFImportParser::scaleH(int height)
{
    return (height * mScaleY);
}

KoShape * WMFImportParser::createShape(const QString &shapeID)
{
    KoShapeFactoryBase * factory = KoShapeRegistry::instance()->get(shapeID);
    if (! factory) {
        kWarning(30514) << "Could not find factory for shape id" << shapeID;
        return 0;
    }

    KoShape * shape = factory->createDefaultShape(mDoc->resourceManager());
    if (shape && shape->shapeId().isEmpty())
        shape->setShapeId(factory->id());

    KoPathShape * path = dynamic_cast<KoPathShape*>(shape);
    if (path && shapeID == KoPathShapeId)
        path->clear();
    // reset tranformation that might come from the default shape
    shape->setTransformation(QMatrix());

    return shape;
}
