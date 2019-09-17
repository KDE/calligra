/* This file is part of the KDE project
 * Copyright (c) 2003 thierry lorthiois <lorthioist@wanadoo.fr>
 * Copyright (c) 2007-2011 Jan Hambrecht <jaham@gmx.net>
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

#include "WmfImportParser.h"

#include "WmfImportDebug.h"

#include <WmfEnums.h>
#include <WmfDeviceContext.h>

#include <KoXmlWriter.h>
#include <KoUnit.h>

#include <QBuffer>

#include <math.h>

/*
bug : see motar.wmf
*/

#define DEG2RAD(angle) angle * M_PI / 180.0
#define RAD2DEG(angle) angle / M_PI * 180.0

WMFImportParser::WMFImportParser(KoXmlWriter &svgWriter)
    : WmfAbstractBackend(), m_svgWriter(svgWriter)
{
}

WMFImportParser::~WMFImportParser()
{
}

//-----------------------------------------------------------------------------
// Virtual Painter

bool WMFImportParser::begin(const QRect &boundingBox)
{
    m_scaleX = m_scaleY = 1;

    m_pageSize = boundingBox.size();

    if (!isStandard()) {
        // Placeable Wmf store the boundingRect() in pixel and the default DPI
        m_pageSize.rwidth() = INCH_TO_POINT((double)boundingBox.width() / defaultDpi());
        m_pageSize.rheight() = INCH_TO_POINT((double)boundingBox.height() / defaultDpi());
    }
    if ((boundingBox.width() != 0) && (boundingBox.height() != 0)) {
        m_scaleX = m_pageSize.width() / (double)boundingBox.width();
        m_scaleY = m_pageSize.height() / (double)boundingBox.height();
    }

    // standard header:
    m_svgWriter.addCompleteElement("<?xml version=\"1.0\" standalone=\"no\"?>\n");
    m_svgWriter.addCompleteElement("<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\" " \
                                   "\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n");

    // add some PR.  one line is more than enough.
    m_svgWriter.addCompleteElement("<!-- Created using Karbon, part of Calligra: http://www.calligra.org/karbon -->\n");
    m_svgWriter.startElement("svg");
    m_svgWriter.addAttribute("xmlns", "http://www.w3.org/2000/svg");
    m_svgWriter.addAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
    m_svgWriter.addAttribute("width", m_pageSize.width());
    m_svgWriter.addAttribute("height", m_pageSize.height());

    debugWmf << "bounding rect =" << boundingBox;
    debugWmf << "page size =" << m_pageSize;
    debugWmf << "scale x =" << m_scaleX;
    debugWmf << "scale y =" << m_scaleY;

    m_window.org = boundingBox.topLeft();
    //m_viewport.org = boundingBox.topLeft();
    m_window.ext = boundingBox.size();
    m_window.extIsValid = true;
    m_viewport.ext = m_pageSize;
    m_viewport.extIsValid = true;

    updateTransform();

    return true;
}

bool WMFImportParser::end()
{
    m_svgWriter.endElement(); // svg
    return true;
}

void WMFImportParser::save()
{
}

void WMFImportParser::restore()
{
}

void WMFImportParser::setWindowOrg(int left, int top)
{
    debugWmf << left << top;
    if (QPoint(left, top) != m_window.org) {
        m_window.org.setX(left);
        m_window.org.setY(top);
        updateTransform();
    }
}

void WMFImportParser::setWindowExt(int width, int height)
{
    debugWmf << width << height;
    // the wmf file can change width/height during the drawing
    if (QSize(width, height) != m_window.ext) {
        m_window.ext = QSizeF(width, height);
        m_window.extIsValid = true;
        updateTransform();
    }
}

void WMFImportParser::setViewportOrg(int left, int top)
{
    debugWmf << left << top;
    if (QPoint(left, top) != m_viewport.org) {
        m_viewport.org.setX(left);
        m_viewport.org.setY(top);
        updateTransform();
    }
}

void WMFImportParser::setViewportExt(int width, int height)
{
    debugWmf << width << height;
    if ((width != 0) && (height != 0)) {
        m_viewport.ext = QSizeF(width, height);
        m_viewport.extIsValid = true;
        updateTransform();
    }
}

void WMFImportParser::setMatrix(Libwmf::WmfDeviceContext &/*context*/, const QMatrix &matrix,
                                bool combine)
{
    if (combine)
        m_matrix = matrix * m_matrix;
    else
        m_matrix = matrix;

    debugWmf << "matrix =" << matrix;
    debugWmf << "combine =" << combine;
}

void WMFImportParser::setPixel(Libwmf::WmfDeviceContext &context, int x, int y, const QColor &color)
{
    Q_UNUSED(context);
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(color);

    // Not Yet Implemented
}

void WMFImportParser::lineTo(Libwmf::WmfDeviceContext &context, int left, int top)
{
    const QString strokeStyle = saveStroke(context);

    static int lineIndex = 0;

    const QPointF p1 = coord(context.currentPosition);
    const QPointF p2 = coord(QPoint(left, top));

    m_svgWriter.startElement("line");
    m_svgWriter.addAttribute("id", QString("line%1").arg(++lineIndex));
    m_svgWriter.addAttribute("x1", p1.x());
    m_svgWriter.addAttribute("y1", p1.y());
    m_svgWriter.addAttribute("x2", p2.x());
    m_svgWriter.addAttribute("y2", p2.y());
    m_svgWriter.addAttribute("style", strokeStyle+"fill:none");
    m_svgWriter.endElement(); // line

    context.currentPosition = QPoint(left, top);
}

void WMFImportParser::drawRect(Libwmf::WmfDeviceContext &context, int left, int top, int width, int height)
{
    QRectF bound = boundBox(left, top, width, height);

    const QString fillStyle = saveFill(context);
    const QString strokeStyle = saveStroke(context);

    static int rectIndex = 0;

    m_svgWriter.startElement("rect");
    m_svgWriter.addAttribute("id", QString("rect%1").arg(++rectIndex));
    m_svgWriter.addAttribute("x", bound.x());
    m_svgWriter.addAttribute("y", bound.y());
    m_svgWriter.addAttribute("width", bound.width());
    m_svgWriter.addAttribute("height", bound.height());
    m_svgWriter.addAttribute("style", strokeStyle+fillStyle);
    m_svgWriter.endElement(); // rect
}

void WMFImportParser::drawRoundRect(Libwmf::WmfDeviceContext &context, int left, int top, int width, int height, int roundw, int roundh)
{
    QRectF bound = boundBox(left, top, width, height);
    // roundw and roundh are in percent of width and height
    const qreal rx = qAbs(roundw)/100. * bound.width();
    const qreal ry = qAbs(roundh)/100. * bound.height();

    const QString fillStyle = saveFill(context);
    const QString strokeStyle = saveStroke(context);

    static int roundRectIndex = 0;

    m_svgWriter.startElement("rect");
    m_svgWriter.addAttribute("id", QString("roundRect%1").arg(++roundRectIndex));
    m_svgWriter.addAttribute("x", bound.x());
    m_svgWriter.addAttribute("y", bound.y());
    m_svgWriter.addAttribute("width", bound.width());
    m_svgWriter.addAttribute("height", bound.height());
    m_svgWriter.addAttribute("rx", 0.5*rx);
    m_svgWriter.addAttribute("ry", 0.5*ry);
    m_svgWriter.addAttribute("style", strokeStyle+fillStyle);
    m_svgWriter.endElement(); // rect
}

void WMFImportParser::drawEllipse(Libwmf::WmfDeviceContext &context, int left, int top, int width, int height)
{
    QRectF bound = boundBox(left, top, width, height);

    const QString fillStyle = saveFill(context);
    const QString strokeStyle = saveStroke(context);

    static int ellipseIndex = 0;

    m_svgWriter.startElement("ellipse");
    m_svgWriter.addAttribute("id", QString("ellipse%1").arg(++ellipseIndex));
    m_svgWriter.addAttribute("cx", bound.center().x());
    m_svgWriter.addAttribute("cy", bound.center().y());
    m_svgWriter.addAttribute("rx", 0.5*bound.width());
    m_svgWriter.addAttribute("ry", 0.5*bound.height());
    m_svgWriter.addAttribute("style", strokeStyle+fillStyle);
    m_svgWriter.endElement(); // ellipse
}

void WMFImportParser::drawArc(Libwmf::WmfDeviceContext &context, int x, int y, int w, int h, int aStart, int aLen)
{
    const qreal a1 = DEG2RAD((aStart * 180) / 2880.0);
    const qreal a2 = DEG2RAD((aLen * 180) / 2880.0);
    const int largeArc = a2 > M_PI ? 1 : 0;

    QRectF bound = boundBox(x, y, w, h);

    const qreal rx = 0.5*bound.width();
    const qreal ry = 0.5*bound.height();
    const QPointF p1 = bound.center() + QPointF(rx*cos(a1), -ry*sin(a1));
    const QPointF p2 = bound.center() + QPointF(rx*cos(a1+a2), -ry*sin(a1+a2));

    QString path =
            QString("M%1,%2 ").arg(p1.x()).arg(p1.y()) +
            QString("A%1,%2 0 %5 0 %3,%4").arg(rx).arg(ry).arg(p2.x()).arg(p2.y()).arg(largeArc);

    const QString strokeStyle = saveStroke(context);

    static int arcIndex = 0;

    m_svgWriter.startElement("path");
    m_svgWriter.addAttribute("id", QString("arc%1").arg(++arcIndex));
    m_svgWriter.addAttribute("d", path);
    m_svgWriter.addAttribute("style", strokeStyle+"fill:none");
    m_svgWriter.endElement(); // path
}

void WMFImportParser::drawPie(Libwmf::WmfDeviceContext &context, int x, int y, int w, int h, int aStart, int aLen)
{
    const qreal a1 = DEG2RAD((aStart * 180) / 2880.0);
    const qreal a2 = DEG2RAD((aLen * 180) / 2880.0);
    const int largeArc = a2 > M_PI ? 1 : 0;

    QRectF bound = boundBox(x, y, w, h);

    const qreal rx = 0.5*bound.width();
    const qreal ry = 0.5*bound.height();
    const QPointF p1 = bound.center() + QPointF(rx*cos(a1), -ry*sin(a1));
    const QPointF p2 = bound.center() + QPointF(rx*cos(a1+a2), -ry*sin(a1+a2));

    QString path =
            QString("M%1,%2 ").arg(bound.center().x()).arg(bound.center().y()) +
            QString("L%1,%2 ").arg(p1.x()).arg(p1.y()) +
            QString("A%1,%2 0 %5 0 %3,%4 ").arg(rx).arg(ry).arg(p2.x()).arg(p2.y()).arg(largeArc) +
            QString("L%1,%2").arg(bound.center().x()).arg(bound.center().y());

    const QString fillStyle = saveFill(context);
    const QString strokeStyle = saveStroke(context);

    static int pieIndex = 0;

    m_svgWriter.startElement("path");
    m_svgWriter.addAttribute("id", QString("pie%1").arg(++pieIndex));
    m_svgWriter.addAttribute("d", path);
    m_svgWriter.addAttribute("style", strokeStyle+fillStyle);
    m_svgWriter.endElement(); // path
}

void WMFImportParser::drawChord(Libwmf::WmfDeviceContext &context, int x, int y, int w, int h, int aStart, int aLen)
{
    const qreal a1 = DEG2RAD((aStart * 180) / 2880.0);
    const qreal a2 = DEG2RAD((aLen * 180) / 2880.0);
    const int largeArc = a2 > M_PI ? 1 : 0;

    QRectF bound = boundBox(x, y, w, h);

    const qreal rx = 0.5*bound.width();
    const qreal ry = 0.5*bound.height();
    const QPointF p1 = bound.center() + QPointF(rx*cos(a1), -ry*sin(a1));
    const QPointF p2 = bound.center() + QPointF(rx*cos(a1+a2), -ry*sin(a1+a2));

    QString path =
            QString("M%1,%2 ").arg(p1.x()).arg(p1.y()) +
            QString("A%1,%2 0 %5 0 %3,%4 ").arg(rx).arg(ry).arg(p2.x()).arg(p2.y()).arg(largeArc) +
            QString("L%1,%2").arg(p1.x()).arg(p1.y());

    const QString fillStyle = saveFill(context);
    const QString strokeStyle = saveStroke(context);

    static int chordKIndex = 0;

    m_svgWriter.startElement("path");
    m_svgWriter.addAttribute("id", QString("chord%1").arg(++chordKIndex));
    m_svgWriter.addAttribute("d", path);
    m_svgWriter.addAttribute("style", strokeStyle+fillStyle);
    m_svgWriter.endElement(); // path
}

void WMFImportParser::drawPolyline(Libwmf::WmfDeviceContext &context, const QPolygon &pa)
{
    QString points;
    const int pointCount = pa.size();
    if (pointCount <= 1)
        return;

    QPointF p;
    // There exists a problem on msvc with for(each) and QVector<QPoint>
    for (int i = 0; i < pa.count(); ++i) {
        const QPoint &point(pa[i]);
        p = coord(point);
        points += QString("%1,%2 ").arg(p.x()).arg(p.y());
    }

    const QString strokeStyle = saveStroke(context);

    static int polylineIndex = 0;

    m_svgWriter.startElement("polyline");
    m_svgWriter.addAttribute("id", QString("polyline%1").arg(++polylineIndex));
    m_svgWriter.addAttribute("points", points);
    m_svgWriter.addAttribute("style", strokeStyle+"fill:none");
    m_svgWriter.endElement(); // polyline
}

void WMFImportParser::drawPolygon(Libwmf::WmfDeviceContext &context, const QPolygon &pa)
{
    QString points;
    const int pointCount = pa.size();
    if (pointCount <= 1)
        return;

    QPointF p;
    // There exists a problem on msvc with for(each) and QVector<QPoint>
    for (int i = 0; i < pa.count(); ++i) {
        const QPoint &point(pa[i]);
        p = coord(point);
        points += QString("%1,%2 ").arg(p.x()).arg(p.y());
    }

    const QString fillStyle = saveFill(context);
    const QString strokeStyle = saveStroke(context);

    static int polygonIndex = 0;

    m_svgWriter.startElement("polygon");
    m_svgWriter.addAttribute("id", QString("polygon%1").arg(++polygonIndex));
    m_svgWriter.addAttribute("points", points);
    m_svgWriter.addAttribute("style", strokeStyle+fillStyle);
    m_svgWriter.endElement(); // polygon
}

void WMFImportParser::drawPolyPolygon(Libwmf::WmfDeviceContext &context, QList<QPolygon>& listPa)
{
    if (listPa.isEmpty())
        return;

    QString path;
    QPointF p;
    foreach(const QPolygon &poly, listPa) {
        int pointCount = poly.size();
        if(pointCount <= 1)
            continue;
        p = coord(poly[0]);
        path += QString("M%1,%2 L").arg(p.x()).arg(p.y());
        for(int i = 1; i < pointCount; ++i) {
            p = coord(poly[i]);
            path += QString("%1,%2 ").arg(p.x()).arg(p.y());
        }
        path.append("Z ");
    }

    const QString fillStyle = saveFill(context);
    const QString strokeStyle = saveStroke(context);

    static int polyPolygonIndex = 0;

    m_svgWriter.startElement("path");
    m_svgWriter.addAttribute("id", QString("polyPolygon%1").arg(++polyPolygonIndex));
    m_svgWriter.addAttribute("d", path);
    m_svgWriter.addAttribute("style", strokeStyle+fillStyle);
    m_svgWriter.endElement(); // path
}

void WMFImportParser::drawImage(Libwmf::WmfDeviceContext &/*context*/, int x, int y, const QImage &rawImage, int sx, int sy, int sw, int sh)
{
    QPoint imgOrg(qMax(sx, 0), qMax(sy, 0));
    QSize imgExt = QSize(rawImage.width()-imgOrg.x(), rawImage.height()-imgOrg.y());
    if (sw > 0) {
        imgExt.rwidth() = qMin(imgExt.width(), sw);
    }
    if (sh > 0) {
        imgExt.rheight() = qMin(imgExt.height(), sh);
    }
    QImage image = rawImage.copy(QRect(imgOrg, imgExt));

    QByteArray ba;
    QBuffer buffer(&ba);
    if(!buffer.open(QIODevice::WriteOnly))
        return;
    if (!image.save(&buffer, "PNG"))
        return;

    static int imageIndex = 0;

    const QPointF pos = coord(QPoint(x, y));
    const QSizeF s = size(image.size());

    m_svgWriter.startElement("image");
    m_svgWriter.addAttribute("id", QString("image%1").arg(++imageIndex));
    m_svgWriter.addAttribute("x", pos.x());
    m_svgWriter.addAttribute("y", pos.y());
    m_svgWriter.addAttribute("width", s.width());
    m_svgWriter.addAttribute("height", s.height());
    m_svgWriter.addAttribute("xlink:href", "data:image/png;base64," + ba.toBase64());
    m_svgWriter.endElement(); // image
}

void WMFImportParser::patBlt(Libwmf::WmfDeviceContext &/*context*/, int x, int y, int width, int height, quint32 rasterOperation)
{
    // Not Yet Implemented
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(width);
    Q_UNUSED(height);
    Q_UNUSED(rasterOperation);
}

void WMFImportParser::drawText(Libwmf::WmfDeviceContext &context, int x, int y, const QString& text)
{
    if (context.textAlign & TA_UPDATECP) {
        // (left, top) position = current logical position
        x = context.currentPosition.x();
        y = context.currentPosition.y();
    }

    QFontMetrics metrics(context.font);

    if (context.textAlign & TA_BOTTOM) {
        y -= metrics.descent();
    } else if(context.textAlign & TA_BASELINE) {
        // nothing to do here
    } else { // TA_TOP
        // this the is the default
        y += metrics.ascent();
    }

    static int textIndex = 0;

    const QPointF pos = coord(QPoint(x, y));

    m_svgWriter.startElement("text");
    m_svgWriter.addAttribute("id", QString("text%1").arg(++textIndex));
    m_svgWriter.addAttribute("x", pos.x());
    m_svgWriter.addAttribute("y", pos.y());
    if (context.textAlign & TA_CENTER)
        m_svgWriter.addAttribute("text-anchor", "middle");
    else if (context.textAlign & TA_RIGHT)
        m_svgWriter.addAttribute("text-anchor", "end");
    m_svgWriter.addAttribute("font-family", context.font.family());
    // adjust font size
    m_svgWriter.addAttributePt("font-size", size(QSize(0, context.font.pointSize())).height());
    if (context.font.bold())
        m_svgWriter.addAttribute("font-weight", "bold");
    if (context.font.italic())
        m_svgWriter.addAttribute("font-style", "italic");
    if (context.font.underline())
        m_svgWriter.addAttribute("text-decoration", "underline");
    m_svgWriter.addAttribute("stroke", context.foregroundTextColor.name());

    if (context.escapement) {
        // we rotate around the anchor point
        // rotation is in 1/10th of a degree
        QString transform =
                QString("translate(%1,%2) ").arg(pos.x()).arg(pos.y()) +
                QString("rotate(%1) ").arg(qreal(context.escapement) / -10.0) +
                QString("translate(%1,%2)").arg(-pos.x()).arg(-pos.y());
        m_svgWriter.addAttribute("transform", transform);
    }

    m_svgWriter.addTextNode(text);

    m_svgWriter.endElement(); // text
}

//-----------------------------------------------------------------------------
// Utilities

QString WMFImportParser::saveStroke(Libwmf::WmfDeviceContext &context)
{
    if(context.pen.style() == Qt::NoPen) {
        return "stroke:none;";
    }

    const qreal width = context.pen.width() > qreal(1.0) ? qMax(qreal(1.0), context.pen.width() * m_scaleX) : qreal(1.0);

    QString strokeStyle;

    strokeStyle += QString("stroke:%1;").arg(context.pen.color().name());
    strokeStyle += QString("stroke-width:%1;").arg(width);
    if (context.pen.capStyle() == Qt::FlatCap)
        strokeStyle += "stroke-linecap:butt;";
    else if (context.pen.capStyle() == Qt::RoundCap)
        strokeStyle += "stroke-linecap:round;";
    else if (context.pen.capStyle() == Qt::SquareCap)
        strokeStyle += "stroke-linecap:square;";

    if (context.pen.joinStyle() == Qt::MiterJoin) {
        strokeStyle += "stroke-linejoin:miter;";
        strokeStyle += QString("stroke-miterlimit:%1;").arg(context.pen.miterLimit());
    } else if (context.pen.joinStyle() == Qt::RoundJoin)
        strokeStyle += "stroke-linejoin:round;";
    else if (context.pen.joinStyle() == Qt::BevelJoin)
        strokeStyle += "stroke-linejoin:bevel;";

    // dash
    if (context.pen.style() > Qt::SolidLine) {
        qreal dashFactor = width;

        if (context.pen.dashOffset() != 0)
            strokeStyle += QString("stroke-dashoffset:%1;").arg(dashFactor * context.pen.dashOffset());

        QString dashStr;
        const QVector<qreal> dashes = context.pen.dashPattern();
        int dashCount = dashes.size();
        for (int i = 0; i < dashCount; ++i) {
            if (i > 0)
                dashStr += ",";
            dashStr += QString("%1").arg(dashes[i] * dashFactor);
        }
        strokeStyle += QString("stroke-dasharray:%1;").arg(dashStr);
    }

    return strokeStyle;
}

QString WMFImportParser::saveFill(Libwmf::WmfDeviceContext &context)
{
    if (context.brush.style() == Qt::NoBrush) {
        return "fill:none;";
    }

    QString fillStyle;

    if (context.brush.style() == Qt::SolidPattern) {
        fillStyle = QString("fill:%1;").arg(context.brush.color().name());
        if (context.brush.color().alphaF() < 1.0)
            fillStyle += QString("fill-opacity:%1;").arg(context.brush.color().alphaF());

        return fillStyle;
    }

    static int fillIndex = 0;
    QString fillId = QString("fill%1").arg(++fillIndex);

    switch (context.brush.style()) {
    case Qt::TexturePattern: {
        QImage texture = context.brush.textureImage();
        m_svgWriter.startElement("pattern");
        m_svgWriter.addAttribute("id", fillId);
        m_svgWriter.addAttribute("x", 0);
        m_svgWriter.addAttribute("y", 0);
        m_svgWriter.addAttribute("width", texture.size().width());
        m_svgWriter.addAttribute("height", texture.size().height());
        m_svgWriter.addAttribute("patternUnits", "userSpaceOnUse");

        m_svgWriter.addAttribute("viewBox", QString("0 0 %1 %2").arg(texture.size().width()).arg(texture.size().height()));

        m_svgWriter.startElement("image");
        m_svgWriter.addAttribute("x", "0");
        m_svgWriter.addAttribute("y", "0");
        m_svgWriter.addAttribute("width", QString("%1px").arg(texture.size().width()));
        m_svgWriter.addAttribute("height", QString("%1px").arg(texture.size().height()));

        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        if (texture.save(&buffer, "PNG")) {
            m_svgWriter.addAttribute("xlink:href", "data:image/png;base64," + ba.toBase64());
        }

        m_svgWriter.endElement(); // image
        m_svgWriter.endElement(); // pattern
        break;
    }
    case Qt::HorPattern:
        m_svgWriter.startElement("pattern");
        m_svgWriter.addAttribute("id", fillId);
        m_svgWriter.addAttribute("x", 0);
        m_svgWriter.addAttribute("y", 0);
        m_svgWriter.addAttribute("width", 30);
        m_svgWriter.addAttribute("height", 30);
        m_svgWriter.addAttribute("patternUnits", "userSpaceOnUse");
        m_svgWriter.startElement("path");
        m_svgWriter.addAttribute("d", "M0,5 L30,5 M0,15 L30,15 M0,25 L30,25");
        m_svgWriter.addAttribute("style", "stroke:black;stroke-width:1");
        m_svgWriter.endElement(); // path
        m_svgWriter.endElement(); // pattern
        break;
    case Qt::VerPattern:
        m_svgWriter.startElement("pattern");
        m_svgWriter.addAttribute("id", fillId);
        m_svgWriter.addAttribute("x", 0);
        m_svgWriter.addAttribute("y", 0);
        m_svgWriter.addAttribute("width", 30);
        m_svgWriter.addAttribute("height", 30);
        m_svgWriter.addAttribute("patternUnits", "userSpaceOnUse");
        m_svgWriter.startElement("path");
        m_svgWriter.addAttribute("d", "M5,0 L5,30 M15,0 L15,30 M25,0 L25,30");
        m_svgWriter.addAttribute("style", "stroke:black;stroke-width:1");
        m_svgWriter.endElement(); // path
        m_svgWriter.endElement(); // pattern
        break;
    case Qt::CrossPattern:
        m_svgWriter.startElement("pattern");
        m_svgWriter.addAttribute("id", fillId);
        m_svgWriter.addAttribute("x", 0);
        m_svgWriter.addAttribute("y", 0);
        m_svgWriter.addAttribute("width", 30);
        m_svgWriter.addAttribute("height", 30);
        m_svgWriter.addAttribute("patternUnits", "userSpaceOnUse");
        m_svgWriter.startElement("path");
        m_svgWriter.addAttribute("d", "M5,0 L5,30 M15,0 L15,30 M25,0 L25,30 M0,5 L30,5 M0,15 L30,15 M0,25 L30,25");
        m_svgWriter.addAttribute("style", "stroke:black;stroke-width:1");
        m_svgWriter.endElement(); // path
        m_svgWriter.endElement(); // pattern
        break;
    case Qt::BDiagPattern:
        m_svgWriter.startElement("pattern");
        m_svgWriter.addAttribute("id", fillId);
        m_svgWriter.addAttribute("x", 0);
        m_svgWriter.addAttribute("y", 0);
        m_svgWriter.addAttribute("width", 30);
        m_svgWriter.addAttribute("height", 30);
        m_svgWriter.addAttribute("patternUnits", "userSpaceOnUse");
        m_svgWriter.startElement("path");
        m_svgWriter.addAttribute("d", "M0,30 L30,0 M0,15 L15,0 M15,30 L30,15");
        m_svgWriter.addAttribute("style", "stroke:black;stroke-width:1");
        m_svgWriter.endElement(); // path
        m_svgWriter.endElement(); // pattern
        break;
    case Qt::FDiagPattern:
        m_svgWriter.startElement("pattern");
        m_svgWriter.addAttribute("id", fillId);
        m_svgWriter.addAttribute("x", 0);
        m_svgWriter.addAttribute("y", 0);
        m_svgWriter.addAttribute("width", 30);
        m_svgWriter.addAttribute("height", 30);
        m_svgWriter.addAttribute("patternUnits", "userSpaceOnUse");
        m_svgWriter.startElement("path");
        m_svgWriter.addAttribute("d", "M0,00 L30,30 M0,15 L15,30 M15,0 L30,15");
        m_svgWriter.addAttribute("style", "stroke:black;stroke-width:1");
        m_svgWriter.endElement(); // path
        m_svgWriter.endElement(); // pattern
        break;
    case Qt::DiagCrossPattern:
        m_svgWriter.startElement("pattern");
        m_svgWriter.addAttribute("id", fillId);
        m_svgWriter.addAttribute("x", 0);
        m_svgWriter.addAttribute("y", 0);
        m_svgWriter.addAttribute("width", 30);
        m_svgWriter.addAttribute("height", 30);
        m_svgWriter.addAttribute("patternUnits", "userSpaceOnUse");
        m_svgWriter.startElement("path");
        m_svgWriter.addAttribute("d", "M0,30 L30,0 M0,15 L15,0 M15,30 L30,15 M0,00 L30,30 M0,15 L15,30 M15,0 L30,15");
        m_svgWriter.addAttribute("style", "stroke:black;stroke-width:1");
        m_svgWriter.endElement(); // path
        m_svgWriter.endElement(); // pattern
        break;
    default:
        debugWmf << "unsupported brush style:" << context.brush.style();
        return QString();
    }

    fillStyle = QString("fill:url(#%1);").arg(fillId);

    if (context.polyFillMode == Libwmf::ALTERNATE)
        fillStyle += "fill-rule:evenodd;";

    return fillStyle;
}

void  WMFImportParser::setCompositionMode(QPainter::CompositionMode)
{
    //TODO
}

QRectF WMFImportParser::boundBox(int left, int top, int width, int height)
{
    const int l = qMin(left, left+width);
    const int t = qMin(top, top+height);
    const int w = qAbs(width);
    const int h = qAbs(height);

    return QRectF(coord(QPoint(l,t)), size(QSize(w, h)));
}

QPointF WMFImportParser::coord(const QPoint &p)
{
    const qreal dx = m_viewport.org.x()-m_window.org.x();
    const qreal dy = m_viewport.org.y()-m_window.org.y();
    const qreal x = (p.x() + dx) * m_scaleX;
    const qreal y = (p.y() + dy) * m_scaleY;
    return QPointF(x, y);
}

QSizeF WMFImportParser::size(const QSize &s)
{
    return QSizeF(m_scaleX *s.width(), m_scaleY*s.height());
}

void WMFImportParser::updateTransform()
{
    if (m_window.extIsValid && m_viewport.extIsValid) {
        m_scaleX = m_viewport.ext.width() / m_window.ext.width();
        m_scaleY = m_viewport.ext.height() / m_window.ext.height();
    }
    debugWmf << "window:" << QRectF(m_window.org, m_window.ext);
    debugWmf << "viewport:" << QRectF(m_viewport.org, m_viewport.ext);
    debugWmf << "scale:" << m_scaleX << m_scaleY;
}
