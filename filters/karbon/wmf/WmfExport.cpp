/* This file is part of the KDE project
 * Copyright (c) 2003 thierry lorthiois (lorthioist@wanadoo.fr)
 * Copyright (c) 2007 Jan Hambrecht <jaham@gmx.net>
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

#include "WmfExport.h"
#include <WmfWriter.h>

#include <KarbonDocument.h>
#include <KarbonPart.h>

#include <kpluginfactory.h>

#include <KoFilterChain.h>
#include <KoShapeStroke.h>
#include <KoShape.h>
#include <KoShapeContainer.h>
#include <KoColorBackground.h>
#include <KoGradientBackground.h>
#include <KoPatternBackground.h>
#include <KoUnit.h>
#include <KoPAPageBase.h>
#include <KoPageLayout.h>

#include <algorithm>

#include <QPainterPath>

/*
TODO: bs.wmf stroke in red with MSword and in brown with Words ??
*/

K_PLUGIN_FACTORY_WITH_JSON(WmfExportFactory, "calligra_filter_karbon2wmf.json",
			   registerPlugin<WmfExport>();)



WmfExport::WmfExport(QObject*parent, const QVariantList&) :
        KoFilter(parent)
{
}

WmfExport::~WmfExport()
{
}

KoFilter::ConversionStatus WmfExport::convert(const QByteArray& from, const QByteArray& to)
{
    if (to != "image/x-wmf" || from != "application/vnd.oasis.opendocument.graphics")
        return KoFilter::NotImplemented;

    KoDocument * doc = m_chain->inputDocument();
    if (! doc)
        return KoFilter::ParsingError;

    KarbonDocument * karbonPart = dynamic_cast<KarbonDocument*>(doc);
    if (! karbonPart)
        return KoFilter::WrongFormat;

    // open Placeable Wmf file
    mWmf = new Libwmf::WmfWriter(m_chain->outputFile());
    if (!mWmf->begin()) {
        delete mWmf;
        return KoFilter::WrongFormat;
    }

    paintDocument(karbonPart);

    mWmf->end();

    delete mWmf;

    return KoFilter::OK;
}

void WmfExport::paintDocument(KarbonDocument* document)
{
    KoPAPageBase *page = document->pages().value(0);
    if (!page) {
        return;
    }
    // resolution
    mDpi = 1000;

    const KoPageLayout &layout = page->pageLayout();
    QSizeF pageSize(layout.width, layout.height);
    int width = static_cast<int>(POINT_TO_INCH(pageSize.width()) * mDpi);
    int height = static_cast<int>(POINT_TO_INCH(pageSize.height()) * mDpi);

    mWmf->setDefaultDpi(mDpi);
    mWmf->setWindow(0, 0, width, height);

    if ((pageSize.width() != 0) && (pageSize.height() != 0)) {
        mScaleX = static_cast<double>(width) / pageSize.width();
        mScaleY = static_cast<double>(height) / pageSize.height();
    }

    QList<KoShape*> shapes = page->shapes();
    std::sort(shapes.begin(), shapes.end(), KoShape::compareShapeZIndex);

    // Export layers.
    foreach(KoShape * shape, shapes) {
        if (dynamic_cast<KoShapeContainer*>(shape))
            continue;
        paintShape(shape);
    }
}

void WmfExport::paintShape(KoShape * shape)
{
    QList<QPolygonF> subpaths = shape->outline().toFillPolygons(shape->absoluteTransformation(0));

    if (! subpaths.count())
        return;

    QList<QPolygon> polygons;
    foreach(const QPolygonF & subpath, subpaths) {
        QPolygon p;
        uint pointCount = subpath.count();
        for (uint i = 0; i < pointCount; ++i)
            p.append(QPoint(coordX(subpath[i].x()), coordY(subpath[i].y())));

        polygons.append(p);
    }
    mWmf->setPen(getPen(shape->stroke()));

    if (polygons.count() == 1 && ! shape->background())
        mWmf->drawPolyline(polygons.first());
    else {
        QBrush fill(Qt::NoBrush);
        QSharedPointer<KoColorBackground>  cbg = qSharedPointerDynamicCast<KoColorBackground>(shape->background());
        if (cbg)
            fill = QBrush(cbg->color(), cbg->style());
        QSharedPointer<KoGradientBackground>  gbg = qSharedPointerDynamicCast<KoGradientBackground>(shape->background());
        if (gbg) {
            fill = QBrush(*gbg->gradient());
            fill.setTransform(gbg->transform());
        }
        QSharedPointer<KoPatternBackground>  pbg = qSharedPointerDynamicCast<KoPatternBackground>(shape->background());
        if (pbg) {
            fill.setTextureImage(pbg->pattern());
            fill.setTransform(pbg->transform());
        }
        mWmf->setBrush(fill);
        if (polygons.count() == 1)
            mWmf->drawPolygon(polygons.first());
        else
            mWmf->drawPolyPolygon(polygons);
    }
}

QPen WmfExport::getPen(const KoShapeStrokeModel * stroke)
{
    const KoShapeStroke * lineStroke = dynamic_cast<const KoShapeStroke*>(stroke);
    if (! lineStroke)
        return QPen(Qt::NoPen);

    QPen pen(lineStroke->lineStyle());
    if (pen.style() > Qt::SolidLine)
        pen.setDashPattern(lineStroke->lineDashes());

    pen.setColor(lineStroke->color());
    pen.setCapStyle(lineStroke->capStyle());
    pen.setJoinStyle(lineStroke->joinStyle());
    pen.setWidthF(coordX(lineStroke->lineWidth()));
    pen.setMiterLimit(lineStroke->miterLimit());

    return pen;
}

int WmfExport::coordX(double left)
{
    return (int)(left * mScaleX);
}

int WmfExport::coordY(double top)
{
    return (int)(top * mScaleY);
}

#include <WmfExport.moc>

