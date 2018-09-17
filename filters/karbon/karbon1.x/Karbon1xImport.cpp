/* This file is part of the KDE project
 * Copyright (C) 2007,2011 Jan Hambrecht <jaham@gmx.net>
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

#include "Karbon1xImport.h"

#include <KarbonGlobal.h>

#include <KoFilterChain.h>
#include <KoStoreDevice.h>
#include <KoXmlWriter.h>
#include <KoUnit.h>
#include <SvgUtil.h>

#include <kpluginfactory.h>

#include <QBuffer>
#include <QImage>
#include <QColor>
#include <QFont>
#include <QFontMetrics>
#include <QDebug>
#include <QLoggingCategory>

#include <math.h>

K_PLUGIN_FACTORY_WITH_JSON(KarbonImportFactory, "calligra_filter_karbon1x2karbon.json",
                           registerPlugin<KarbonImport>();)

const QLoggingCategory &KARBON1_LOG()
{
    static const QLoggingCategory category("calligra.filter.karbon1x2karbon");
    return category;
}

#define debugKarbon1 qCDebug(KARBON1_LOG)
#define warnKarbon1 qCWarning(KARBON1_LOG)
#define errorKarbon1 qCCritical(KARBON1_LOG)

KarbonImport::KarbonImport(QObject*parent, const QVariantList&)
    : KoFilter(parent), m_svgWriter(0)
{
}

KarbonImport::~KarbonImport()
{
    delete m_svgWriter;
}

KoFilter::ConversionStatus KarbonImport::convert(const QByteArray& from, const QByteArray& to)
{
    // check for proper conversion
    if (to != "image/svg+xml" || from != "application/x-karbon")
        return KoFilter::NotImplemented;

    const QString fileName(m_chain->inputFile());
    if (fileName.isEmpty()) {
        errorKarbon1 << "No input file name!";
        return KoFilter::StupidError;
    }

    QFile svgFile(m_chain->outputFile());
    if (!svgFile.open(QIODevice::WriteOnly)) {
        return KoFilter::CreationError;
    }

    delete m_svgWriter;
    m_svgWriter = new KoXmlWriter(&svgFile);

    KoStore* store = KoStore::createStore(fileName, KoStore::Read);
    if (store && store->hasFile("maindoc.xml")) {

        if (! store->open("maindoc.xml")) {
            errorKarbon1 << "Opening root has failed";
            delete store;
            return KoFilter::StupidError;
        }
        KoStoreDevice ioMain(store);
        ioMain.open(QIODevice::ReadOnly);
        if (! parseRoot(&ioMain)) {
            warnKarbon1 << "Parsing maindoc.xml has failed! Aborting!";
            delete store;
            return KoFilter::StupidError;
        }
        ioMain.close();
        store->close();
    } else {
        warnKarbon1 << "Opening store has failed. Trying raw XML file!";
        // Be sure to undefine store
        delete store;
        store = 0;

        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        if (! parseRoot(&file)) {
            errorKarbon1 << "Could not process document! Aborting!";
            file.close();
            return KoFilter::StupidError;
        }
        file.close();
    }

    // We have finished with the input store/file, so close the store (already done for a raw XML file)
    delete store;
    store = 0;

    svgFile.close();

    return KoFilter::OK;
}

bool KarbonImport::parseRoot(QIODevice* io)
{
    int line, col;
    QString errormessage;

    KoXmlDocument inputDoc;
    const bool parsed = inputDoc.setContent(io, &errormessage, &line, &col);

    if (! parsed) {
        errorKarbon1 << "Error while parsing file: "
        << "at line " << line << " column: " << col
        << " message: " << errormessage;
        // ### TODO: feedback to the user
        return false;
    }

    // Do the conversion!
    convert(inputDoc);

    return true;
}

bool KarbonImport::convert(const KoXmlDocument &document)
{
    KoXmlElement doc = document.documentElement();

    return loadXML(doc);
}

bool KarbonImport::loadXML(const KoXmlElement& doc)
{
    if (doc.attribute("mime") != "application/x-karbon" || doc.attribute("syntaxVersion") != "0.1")
        return false;

    const qreal width = doc.attribute("width", "595.277").toDouble();
    const qreal height = doc.attribute("height", "841.891").toDouble();

    // standard header:
    m_svgWriter->addCompleteElement("<?xml version=\"1.0\" standalone=\"no\"?>\n");
    m_svgWriter->addCompleteElement("<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\" " \
                                   "\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n");

    // add some PR.  one line is more than enough.
    m_svgWriter->addCompleteElement("<!-- Created using Karbon, part of Calligra: http://www.calligra.org/karbon -->\n");
    m_svgWriter->startElement("svg");
    m_svgWriter->addAttribute("xmlns", "http://www.w3.org/2000/svg");
    m_svgWriter->addAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
    m_svgWriter->addAttribute("width", width);
    m_svgWriter->addAttribute("height", height);

    m_mirrorMatrix.translate(0, SvgUtil::fromUserSpace(height));
    m_mirrorMatrix.scale(1.0, -1.0);
    m_transformation.push(m_mirrorMatrix);

    KoXmlElement e;
    forEachElement(e, doc) {
        if (e.tagName() == "LAYER") {
            m_svgWriter->startElement("g");
            if (e.attribute("visible") == "0") {
                m_svgWriter->addAttribute("display", "none");
            }
            if (!e.attribute("ID").isEmpty()) {
                m_svgWriter->addAttribute("id", e.attribute("ID"));
            }

            loadGroup(e);

            m_svgWriter->endElement(); // g
        }
    }

    m_svgWriter->endElement(); // svg
    m_svgWriter->endDocument();

    return true;
}

void KarbonImport::loadGroup(const KoXmlElement &element)
{
    KoXmlElement e;
    forEachElement(e, element) {
        if (e.tagName() == "COMPOSITE" || e.tagName() == "PATH") {
            loadPath(e);
        } else if (e.tagName() == "ELLIPSE") {
            loadEllipse(e);
        } else if (e.tagName() == "RECT") {
            loadRect(e);
        } else if (e.tagName() == "POLYLINE") {
            loadPolyline(e);
        } else if (e.tagName() == "POLYGON") {
            loadPolygon(e);
        } else if (e.tagName() == "SINUS") {
            loadSinus(e);
        } else if (e.tagName() == "SPIRAL") {
            loadSpiral(e);
        } else if (e.tagName() == "STAR") {
            loadStar(e);
        } else if (e.tagName() == "GROUP") {
            m_svgWriter->startElement("g");
            QTransform m = SvgUtil::parseTransform(e.attribute("transform", ""));
            m_transformation.push(m_transformation.top()*m);
            loadGroup(e);
            m_transformation.pop();
            loadCommon(e, true);
            m_svgWriter->endElement(); // g
        }
        /* TODO
        else if( e.tagName() == "CLIP" )
        {
            VClipGroup* grp = new VClipGroup( this );
            grp->load( e );
            append( grp );
        }
        */
        else if (e.tagName() == "IMAGE") {
            loadImage(e);
        } else if (e.tagName() == "TEXT") {
            loadText(e);
        }
    }
}

QString KarbonImport::loadStyle(const KoXmlElement &element)
{
    QString style;
    KoXmlElement e;
    forEachElement(e, element) {
        if (e.tagName() == "STROKE") {
            style += loadStroke(e);
        } else if (e.tagName() == "FILL") {
            style += loadFill(e);
        }
    }

    return style;
}

QColor KarbonImport::loadColor(const KoXmlElement &element)
{
    enum ColorSpace {
        rgb  = 0,  // the RGB colorspace (red, green and blue components)
        cmyk = 1,  // the CMYK colorspace (cyan, magenta, yellow and black components)
        hsb  = 2,  // the HSB colorspace (hue, saturation and brightnes components)
        gray = 3   // the Gray colorspace (gray from black to white)
    };

    ushort colorSpace = element.attribute("colorSpace").toUShort();

    const qreal opacity = element.attribute("opacity", "1.0").toDouble();

    qreal value[4] = { 0 };

    if (colorSpace == gray)
        value[0] = element.attribute("v", "0.0").toDouble();
    else {
        value[0] = element.attribute("v1", "0.0").toDouble();
        value[1] = element.attribute("v2", "0.0").toDouble();
        value[2] = element.attribute("v3", "0.0").toDouble();

        if (colorSpace == cmyk)
            value[3] = element.attribute("v4", "0.0").toDouble();
    }

    if (value[0] < 0.0 || value[0] > 1.0)
        value[0] = 0.0;
    if (value[1] < 0.0 || value[1] > 1.0)
        value[1] = 0.0;
    if (value[2] < 0.0 || value[2] > 1.0)
        value[2] = 0.0;
    if (value[3] < 0.0 || value[3] > 1.0)
        value[3] = 0.0;

    QColor color;

    if (colorSpace == hsb)
        color.setHsvF(value[0], value[1], value[2], opacity);
    else if (colorSpace == gray)
        color.setRgbF(value[0], value[0], value[0], opacity);
    else if (colorSpace == cmyk)
        color.setCmykF(value[0], value[1], value[2], value[3], opacity);
    else
        color.setRgbF(value[0], value[1], value[2], opacity);

    return color;
}

void KarbonImport::loadColorStops(const KoXmlElement &element)
{
    // load stops
    KoXmlElement colorstop;
    forEachElement(colorstop, element) {
        if (colorstop.tagName() == "COLORSTOP") {
            QColor color = loadColor(colorstop.firstChild().toElement());
            qreal stop = colorstop.attribute("ramppoint", "0.0").toDouble();

            m_svgWriter->startElement("stop");
            m_svgWriter->addAttribute("stop-color", color.name());
            m_svgWriter->addAttribute("offset", stop);
            m_svgWriter->addAttribute("stop-opacity", color.alphaF());
            m_svgWriter->endElement();
        }
    }
}

QString KarbonImport::loadGradient(const KoXmlElement &element)
{
    enum GradientType { linear = 0, radial = 1, conic  = 2 };
    enum GradientSpread { none = 0, reflect = 1, repeat  = 2 };

    QPointF origin;
    origin.setX(element.attribute("originX", "0.0").toDouble());
    origin.setY(element.attribute("originY", "0.0").toDouble());

    QPointF focal;
    focal.setX(element.attribute("focalX", "0.0").toDouble());
    focal.setY(element.attribute("focalY", "0.0").toDouble());

    QPointF vector;
    vector.setX(element.attribute("vectorX", "0.0").toDouble());
    vector.setY(element.attribute("vectorY", "0.0").toDouble());

    const int type = element.attribute("type", 0).toInt();
    const int spread = element.attribute("repeatMethod", 0).toInt();

    const QString spreadMethod[3] = {
        QString("pad"),
        QString("reflect"),
        QString("repeat")
    };

    const QString uid = makeUnique("gradient");

    switch (type) {
    case linear: {
        m_svgWriter->startElement("linearGradient");
        m_svgWriter->addAttribute("id", uid);
        m_svgWriter->addAttribute("gradientUnits", "userSpaceOnUse");
        m_svgWriter->addAttribute("x1", origin.x());
        m_svgWriter->addAttribute("y1", origin.y());
        m_svgWriter->addAttribute("x2", vector.x());
        m_svgWriter->addAttribute("y2", vector.y());
        m_svgWriter->addAttribute("spreadMethod", spreadMethod[spread]);
        // color stops
        loadColorStops(element);
        m_svgWriter->endElement();
        break;
    }
    case radial: {
        QPointF diffVec = origin - vector;
        const qreal radius = sqrt(diffVec.x() * diffVec.x() + diffVec.y() * diffVec.y());

        m_svgWriter->startElement("radialGradient");
        m_svgWriter->addAttribute("id", uid);
        m_svgWriter->addAttribute("gradientUnits", "userSpaceOnUse");
        m_svgWriter->addAttribute("cx", origin.x());
        m_svgWriter->addAttribute("cy", origin.y());
        m_svgWriter->addAttribute("fx", focal.x());
        m_svgWriter->addAttribute("fy", focal.y());
        m_svgWriter->addAttribute("r", radius);
        m_svgWriter->addAttribute("spreadMethod", spreadMethod[spread]);
        // color stops
        loadColorStops(element);
        m_svgWriter->endElement();
        break;
    }
    case conic: {
        // TODO
//        QPointF dirVec = vector - origin;
//        qreal angle = atan2(dirVec.y(), dirVec.x()) * 180.0 / M_PI;
//        QConicalGradient * g = new QConicalGradient();
//        g->setCenter(origin);
//        g->setAngle(angle);
//        gradient = g;
        break;
    }
    }

    return uid;
}

QString KarbonImport::loadPattern(const KoXmlElement &element)
{
    QPointF origin;
    origin.setX(element.attribute("originX", "0.0").toDouble());
    origin.setY(element.attribute("originY", "0.0").toDouble());

    QPointF vector;
    vector.setX(element.attribute("vectorX", "0.0").toDouble());
    vector.setY(element.attribute("vectorY", "0.0").toDouble());

    QPointF dirVec = vector - origin;
    qreal angle = atan2(dirVec.y(), dirVec.x()) * 180.0 / M_PI;

    QTransform m;
    m.translate(origin.x(), origin.y());
    m.rotate(angle);

    QString trafo = QString("matrix(%1 %2 %3 %4 %5 %6)")
            .arg(m.m11()).arg(m.m12())
            .arg(m.m21()).arg(m.m22())
            .arg(m.dx()).arg(m.dy());

    QString fname = element.attribute("tilename");

    QImage image;
    if (! image.load(fname)) {
        warnKarbon1 << "Failed to load pattern image" << fname;
        return QString();
    }

    const QString uid = makeUnique("pattern");

    m_svgWriter->startElement("defs");
    m_svgWriter->startElement("pattern");
    m_svgWriter->addAttribute("id", uid);
    m_svgWriter->addAttribute("x", origin.x());
    m_svgWriter->addAttribute("y", origin.y());

    m_svgWriter->addAttribute("width", image.width());
    m_svgWriter->addAttribute("height", image.height());
    m_svgWriter->addAttribute("patternUnits", "userSpaceOnUse");
    m_svgWriter->addAttribute("patternTransform", trafo);

    m_svgWriter->addAttribute("viewBox", QString("0 0 %1 %2").arg(image.width()).arg(image.height()));
    //*m_defs << " patternContentUnits=\"userSpaceOnUse\"";

    m_svgWriter->startElement("image");
    m_svgWriter->addAttribute("x", "0");
    m_svgWriter->addAttribute("y", "0");
    m_svgWriter->addAttribute("width", QString("%1px").arg(image.width()));
    m_svgWriter->addAttribute("height", QString("%1px").arg(image.height()));

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    if (image.save(&buffer, "PNG")) {
        m_svgWriter->addAttribute("xlink:href", "data:image/png;base64," + ba.toBase64());
    }

    m_svgWriter->endElement(); // image
    m_svgWriter->endElement(); // pattern
    m_svgWriter->endElement(); // defs

    return uid;
}

QVector<qreal> KarbonImport::loadDashes(const KoXmlElement& element)
{
    QVector<qreal> dashes;

    KoXmlElement e;
    forEachElement(e, element) {
        if (e.tagName() == "DASH") {
            dashes.append(qMax<qreal>(0.0, e.attribute("l", "0.0").toDouble()));
        }
    }
    return dashes;
}

QString KarbonImport::loadStroke(const KoXmlElement &element)
{
    QString strokeStyle;

    switch (element.attribute("lineCap", "0").toUShort()) {
    case 1:
        strokeStyle += "stroke-linecap:round;";
        break;
    case 2:
        strokeStyle += "stroke-linecap:square;";
        break;
    default:
        strokeStyle += "stroke-linecap:butt;";
    }

    switch (element.attribute("lineJoin", "0").toUShort()) {
    case 1:
        strokeStyle += "stroke-linejoin:round;";
        break;
    case 2:
        strokeStyle += "stroke-linejoin:bevel;";
        break;
    default:
        strokeStyle += "stroke-linejoin:miter;";
    }

    QString lineWidth = element.attribute("lineWidth", "1.0");
    strokeStyle += QString("stroke-width:%1;").arg(lineWidth) +
                   QString("stroke-miterlimit:%1;").arg(element.attribute("miterLimit", "10.0"));

    QString strokePaint;

    KoXmlElement e;
    forEachElement(e, element) {
        if (e.tagName() == "COLOR") {
            strokePaint += QString("stroke:%1;").arg(loadColor(e).name());
        } else if (e.tagName() == "DASHPATTERN") {
            const qreal dashOffset = element.attribute("offset", "0.0").toDouble();
            const qreal dashFactor = lineWidth.toDouble();
            if (dashOffset != 0)
                strokePaint += QString("stroke-dashoffset:%1;").arg(dashFactor * dashOffset);

            QString dashStr;
            const QVector<qreal> dashes = loadDashes(e);
            const int dashCount = dashes.size();
            for (int i = 0; i < dashCount; ++i) {
                if (i > 0)
                    dashStr += ",";
                dashStr += QString("%1").arg(dashes[i] * dashFactor);
            }
            strokePaint += QString("stroke-dasharray:%1;").arg(dashStr);
        } else if (e.tagName() == "GRADIENT") {
            strokePaint += QString("stroke:url(#%1);").arg(loadGradient(e));
        }
        /* TODO gradient and pattern on stroke not yet implemented in flake
        else if( e.tagName() == "PATTERN" )
        {
            m_type = patt;
            m_pattern.load( e );
        }
        */
    }

    if (strokePaint.isEmpty())
        return "stroke:none;";

    return strokePaint+strokeStyle;
}

QString KarbonImport::loadFill(const KoXmlElement &element)
{
    QString fill;

    KoXmlElement e;
    forEachElement(e, element) {
        if (e.tagName() == "COLOR") {
            fill += QString("fill:%1;").arg(loadColor(e).name());
        }
        if (e.tagName() == "GRADIENT") {
            const QString gradientId = loadGradient(e);
            if (!gradientId.isEmpty()) {
                fill += QString("fill:url(#%1);").arg(gradientId);
            }
        } else if (e.tagName() == "PATTERN") {
            const QString patternId = loadPattern(e);
            if (!patternId.isEmpty()) {
                fill += QString("fill:url(#%1);").arg(patternId);
            }
        }
    }

    if (fill.isEmpty())
        return "fill:none;";

    return fill;
}

void KarbonImport::loadCommon(const KoXmlElement &element, bool ignoreTransform)
{
    QString id = element.attribute("ID");
    if (! id.isEmpty()) {
        m_svgWriter->addAttribute("id", makeUnique(id));
    } else {
        m_svgWriter->addAttribute("id", makeUnique("shape"));
    }

    if (ignoreTransform)
        return;

    QTransform m = SvgUtil::parseTransform(element.attribute("transform", ""));
    m_svgWriter->addAttribute("transform", SvgUtil::transformToString(m_transformation.top()*m));
}

void KarbonImport::loadPath(const KoXmlElement &element)
{
    QString data = element.attribute("d");

    KoXmlElement child;
    forEachElement(child, element) {
        // backward compatibility for karbon before calligra 1.3.x
        if (child.tagName() == "PATH") {
            KoXmlElement cmd;
            forEachElement(cmd, child) {
                if (cmd.tagName() == "MOVE") {
                    data += QString("M%1,%2 ").arg(cmd.attribute("x")).arg(cmd.attribute("y"));
                } else if (cmd.tagName() == "LINE") {
                    data += QString("L%1,%2 ").arg(cmd.attribute("x")).arg(cmd.attribute("y"));
                } else if (cmd.tagName() == "CURVE") {
                    data += QString("C%1,%2 %3,%4 %5,%6").arg(cmd.attribute("x1")).arg(cmd.attribute("y1"))
                            .arg(cmd.attribute("x2")).arg(cmd.attribute("y2"))
                            .arg(cmd.attribute("x3")).arg(cmd.attribute("y3"));
                }
            }

            if (child.attribute("isClosed") != "0") {
                data += "Z ";
            }
        }
    }

    QString style = loadStyle(element);
    if (element.attribute("fillRule") == "0") {
        // non-zero is default, so only write fillrule if evenodd is set
        style += "fill-rule:evenodd;";
    }

    m_svgWriter->startElement("path");
    loadCommon(element);
    m_svgWriter->addAttribute("style", style);
    m_svgWriter->addAttribute("d", data);
    m_svgWriter->endElement(); // path
}

void KarbonImport::loadEllipse(const KoXmlElement &element)
{
    const qreal rx = KoUnit::parseValue(element.attribute("rx"));
    const qreal ry = KoUnit::parseValue(element.attribute("ry"));
    const qreal cx = KoUnit::parseValue(element.attribute("cx"));
    const qreal cy = KoUnit::parseValue(element.attribute("cy"));
    //ellipse->setSize(QSizeF(2*rx, 2*ry));

    const qreal a1 = element.attribute("start-angle").toDouble() * M_PI / 180.;
    const qreal a2 = element.attribute("end-angle").toDouble() * M_PI / 180.;
    const int largeArc = a2 > M_PI ? 1 : 0;

    const QPointF center(cx, cy);
    const QPointF p1 = center + QPointF(rx*cos(a1), -ry*sin(a1));
    const QPointF p2 = center + QPointF(rx*cos(a1+a2), -ry*sin(a1+a2));

    QString data;

    if (element.attribute("kind") == "cut") {
        data += QString("M%1,%2 ").arg(p1.x()).arg(p1.y()) +
                QString("A%1,%2 0 %5 0 %3,%4 ").arg(rx).arg(ry).arg(p2.x()).arg(p2.y()).arg(largeArc) +
                QString("L%1,%2").arg(p1.x()).arg(p1.y());
    }
    else if (element.attribute("kind") == "section") {
        data += QString("M%1,%2 ").arg(center.x()).arg(center.y()) +
                QString("L%1,%2 ").arg(p1.x()).arg(p1.y()) +
                QString("A%1,%2 0 %5 0 %3,%4 ").arg(rx).arg(ry).arg(p2.x()).arg(p2.y()).arg(largeArc) +
                QString("L%1,%2").arg(center.x()).arg(center.y());
    }
    else if (element.attribute("kind") == "arc") {
        data += QString("M%1,%2 ").arg(p1.x()).arg(p1.y()) +
                QString("A%1,%2 0 %5 0 %3,%4").arg(rx).arg(ry).arg(p2.x()).arg(p2.y()).arg(largeArc);
    } else {
        const QString style = loadStyle(element);

        m_svgWriter->startElement("ellipse");
        m_svgWriter->addAttribute("cx", cx);
        m_svgWriter->addAttribute("cy", cy);
        m_svgWriter->addAttribute("rx", rx);
        m_svgWriter->addAttribute("ry", ry);
        loadCommon(element);
        m_svgWriter->addAttribute("style", style);
        m_svgWriter->endElement(); // ellipse
        return;
    }

    const QString style = loadStyle(element);

    m_svgWriter->startElement("path");
    loadCommon(element);
    m_svgWriter->addAttribute("style", style);
    m_svgWriter->addAttribute("d", data);
    m_svgWriter->endElement(); // path
}

void KarbonImport::loadRect(const KoXmlElement &element)
{
    const QString style = loadStyle(element);

    // TODO: apply height to x-coordinate ?
//    qreal x = KoUnit::parseValue(element.attribute("x"));
//    qreal y = KoUnit::parseValue(element.attribute("y"));
//    rect->setAbsolutePosition(QPointF(x, y), KoFlake::BottomLeftCorner);

    m_svgWriter->startElement("rect");
    m_svgWriter->addAttribute("x", element.attribute("x"));
    m_svgWriter->addAttribute("y", element.attribute("y"));
    m_svgWriter->addAttribute("width", element.attribute("width"));
    m_svgWriter->addAttribute("height", element.attribute("height"));
    m_svgWriter->addAttribute("rx", element.attribute("rx"));
    m_svgWriter->addAttribute("ry", element.attribute("ry"));
    loadCommon(element);
    m_svgWriter->addAttribute("style", style);
    m_svgWriter->endElement(); // rect
}

void KarbonImport::loadPolyline(const KoXmlElement &element)
{
    const QString style = loadStyle(element);

    m_svgWriter->startElement("polyline");
    loadCommon(element);
    m_svgWriter->addAttribute("style", style);
    m_svgWriter->addAttribute("points", element.attribute("points"));
    m_svgWriter->endElement(); // polyline
}

void KarbonImport::loadPolygon(const KoXmlElement &element)
{
    // TODO:
//    qreal x = KoUnit::parseValue(element.attribute("x"));
//    qreal y = KoUnit::parseValue(element.attribute("y"));
//    polygon->setAbsolutePosition(QPointF(x, y), KoFlake::TopLeftCorner);

    const QString style = loadStyle(element);

    m_svgWriter->startElement("polygon");
    loadCommon(element);
    m_svgWriter->addAttribute("style", style);
    m_svgWriter->addAttribute("points", element.attribute("points"));
    m_svgWriter->endElement(); // polyline
}

void KarbonImport::loadSinus(const KoXmlElement &element)
{
    QPointF p1, p2, p3;

    const qreal x = KoUnit::parseValue(element.attribute("x"));
    const qreal y = KoUnit::parseValue(element.attribute("y"));
    const qreal w = KoUnit::parseValue(element.attribute("width"), 10.0);
    const qreal h = KoUnit::parseValue(element.attribute("height"), 10.0);
    const uint periods = element.attribute("periods").toUInt();
    const qreal sx = w / periods;
    const qreal sy = h;

    //    sinus->setAbsolutePosition(QPointF(x, y - h)/*, KoFlake::TopLeftCorner*/);
    //    sinus->setSize(QSizeF(w / periods, h));

    QString data = QString("M%1,%2 ").arg(x).arg(y);

    for (uint i = 0; i < periods; ++i) {
        p1.setX(i + 1.0 / 24.0);
        p1.setY((2.0 * KarbonGlobal::sqrt2 - 1.0) * KarbonGlobal::one_7);
        p2.setX(i + 1.0 / 12.0);
        p2.setY((4.0 * KarbonGlobal::sqrt2 - 2.0) * KarbonGlobal::one_7);
        p3.setX(i + 1.0 / 8.0);
        p3.setY(KarbonGlobal::sqrt2 * 0.5);
        data += QString("C%1,%2 %3,%4 %5,%6 ")
                .arg(x+sx*p1.x()).arg(y+sy*p1.y())
                .arg(x+sx*p2.x()).arg(y+sy*p2.y())
                .arg(x+sx*p3.x()).arg(y+sy*p3.y());

        p1.setX(i + 1.0 / 6.0);
        p1.setY((3.0 * KarbonGlobal::sqrt2 + 2.0) * KarbonGlobal::one_7);
        p2.setX(i + 5.0 / 24.0);
        p2.setY(1.0);
        p3.setX(i + 1.0 / 4.0);
        p3.setY(1.0);
        data += QString("C%1,%2 %3,%4 %5,%6 ")
                .arg(x+sx*p1.x()).arg(y+sy*p1.y())
                .arg(x+sx*p2.x()).arg(y+sy*p2.y())
                .arg(x+sx*p3.x()).arg(y+sy*p3.y());

        p1.setX(i + 7.0 / 24.0);
        p1.setY(1.0);
        p2.setX(i + 1.0 / 3.0);
        p2.setY((3.0 * KarbonGlobal::sqrt2 + 2.0) * KarbonGlobal::one_7);
        p3.setX(i + 3.0 / 8.0);
        p3.setY(KarbonGlobal::sqrt2 * 0.5);
        data += QString("C%1,%2 %3,%4 %5,%6 ")
                .arg(x+sx*p1.x()).arg(y+sy*p1.y())
                .arg(x+sx*p2.x()).arg(y+sy*p2.y())
                .arg(x+sx*p3.x()).arg(y+sy*p3.y());

        p1.setX(i + 5.0 / 12.0);
        p1.setY((4.0 * KarbonGlobal::sqrt2 - 2.0) * KarbonGlobal::one_7);
        p2.setX(i + 11.0 / 24.0);
        p2.setY((2.0 * KarbonGlobal::sqrt2 - 1.0) * KarbonGlobal::one_7);
        p3.setX(i + 1.0 / 2.0);
        p3.setY(0.0);
        data += QString("C%1,%2 %3,%4 %5,%6 ")
                .arg(x+sx*p1.x()).arg(y+sy*p1.y())
                .arg(x+sx*p2.x()).arg(y+sy*p2.y())
                .arg(x+sx*p3.x()).arg(y+sy*p3.y());

        p1.setX(i + 13.0 / 24.0);
        p1.setY(-(2.0 * KarbonGlobal::sqrt2 - 1.0) * KarbonGlobal::one_7);
        p2.setX(i + 7.0 / 12.0);
        p2.setY(-(4.0 * KarbonGlobal::sqrt2 - 2.0) * KarbonGlobal::one_7);
        p3.setX(i + 5.0 / 8.0);
        p3.setY(-KarbonGlobal::sqrt2 * 0.5);
        data += QString("C%1,%2 %3,%4 %5,%6 ")
                .arg(x+sx*p1.x()).arg(y+sy*p1.y())
                .arg(x+sx*p2.x()).arg(y+sy*p2.y())
                .arg(x+sx*p3.x()).arg(y+sy*p3.y());

        p1.setX(i + 2.0 / 3.0);
        p1.setY(-(3.0 * KarbonGlobal::sqrt2 + 2.0) * KarbonGlobal::one_7);
        p2.setX(i + 17.0 / 24.0);
        p2.setY(-1.0);
        p3.setX(i + 3.0 / 4.0);
        p3.setY(-1.0);
        data += QString("C%1,%2 %3,%4 %5,%6 ")
                .arg(x+sx*p1.x()).arg(y+sy*p1.y())
                .arg(x+sx*p2.x()).arg(y+sy*p2.y())
                .arg(x+sx*p3.x()).arg(y+sy*p3.y());

        p1.setX(i + 19.0 / 24.0);
        p1.setY(-1.0);
        p2.setX(i + 5.0 / 6.0);
        p2.setY(-(3.0 * KarbonGlobal::sqrt2 + 2.0) * KarbonGlobal::one_7);
        p3.setX(i + 7.0 / 8.0);
        p3.setY(-KarbonGlobal::sqrt2 * 0.5);
        data += QString("C%1,%2 %3,%4 %5,%6 ")
                .arg(x+sx*p1.x()).arg(y+sy*p1.y())
                .arg(x+sx*p2.x()).arg(y+sy*p2.y())
                .arg(x+sx*p3.x()).arg(y+sy*p3.y());

        p1.setX(i + 11.0 / 12.0);
        p1.setY(-(4.0 * KarbonGlobal::sqrt2 - 2.0) * KarbonGlobal::one_7);
        p2.setX(i + 23.0 / 24.0);
        p2.setY(-(2.0 * KarbonGlobal::sqrt2 - 1.0) * KarbonGlobal::one_7);
        p3.setX(i + 1.0);
        p3.setY(0.0);
        data += QString("C%1,%2 %3,%4 %5,%6 ")
                .arg(x+sx*p1.x()).arg(y+sy*p1.y())
                .arg(x+sx*p2.x()).arg(y+sy*p2.y())
                .arg(x+sx*p3.x()).arg(y+sy*p3.y());
    }

    const QString style = loadStyle(element);

    m_svgWriter->startElement("path");
    loadCommon(element);
    m_svgWriter->addAttribute("style", style);
    m_svgWriter->addAttribute("d", data);
    m_svgWriter->endElement(); // path
}

void KarbonImport::loadSpiral(const KoXmlElement &element)
{
    enum SpiralType { round, rectangular };

    qreal radius  = qAbs(KoUnit::parseValue(element.attribute("radius")));
    // TODO: proper way to use "angle" value in transformation code below
//     qreal angle = element.attribute("angle").toDouble();
    qreal fade = element.attribute("fade").toDouble();

    qreal cx = KoUnit::parseValue(element.attribute("cx"));
    qreal cy = KoUnit::parseValue(element.attribute("cy"));

    uint segments  = element.attribute("segments").toUInt();
    int clockwise = element.attribute("clockwise").toInt();
    int type = element.attribute("type").toInt();

    // It makes sense to have at least one segment:
    if (segments < 1)
        segments = 1;

    // Fall back, when fade is out of range:
    if (fade <= 0.0 || fade >= 1.0)
        fade = 0.5;

    // advance by pi/2 clockwise or cclockwise?
    qreal adv_ang = (clockwise ? 1.0 : -1.0) * 90.0;
    qreal adv_rad = (clockwise ? -1.0 : 1.0) * KarbonGlobal::pi_2;
    // radius of first segment is non-faded radius:
    qreal r = radius;

    QPointF oldP(0.0, (clockwise ? -1.0 : 1.0) * radius);
    QPointF newP;
    QPointF newCenter(0.0, 0.0);

    QString data = QString("M%1,%2 ").arg(cx+oldP.x()).arg(cy+oldP.y());

    qreal startAngle = clockwise ? 90.0 : -90.0;

    for (uint i = 0; i < segments; ++i) {
        if (type == round) {
            const qreal a1 = startAngle * M_PI / 180.;
            const qreal a2 = M_PI_2;
            QPointF p1 = QPointF(r*cos(a1), -r*sin(a1));
            QPointF p2 = QPointF(r*cos(a1+a2), -r*sin(a1+a2));
            const QPointF offset = oldP + p1;
            p1 += offset;
            p2 += offset;
            data += QString("A%1,%2 0 0 0 %3,%4").arg(r).arg(r).arg(cx+p2.x()).arg(cy+p2.y());
            oldP = p2;
        } else {
            newP.setX(r * cos(adv_rad * (i + 2)) + newCenter.x());
            newP.setY(r * sin(adv_rad * (i + 2)) + newCenter.y());

            data += QString("L%1,%2 ").arg(cx+newP.x()).arg(cy+newP.y());

            newCenter += (newP - newCenter) * (1.0 - fade);
            oldP = newP;
        }

        r *= fade;
        startAngle += adv_ang;
    }


    // sadly it's not feasible to simply add angle while creation.
    // make cw-spiral start at mouse-pointer
    // one_pi_180 = 1/(pi/180) = 180/pi.
//    QTransform m;
//    m.rotate((angle + (clockwise ? KarbonGlobal::pi : 0.0)) * KarbonGlobal::one_pi_180);
//    spiral->applyAbsoluteTransformation(m);

    const QString style = loadStyle(element) + "fill-rule:nonzero;";

    m_svgWriter->startElement("path");
    loadCommon(element);
    m_svgWriter->addAttribute("style", style);
    m_svgWriter->addAttribute("d", data);
    m_svgWriter->endElement(); // path
}

void KarbonImport::loadStar(const KoXmlElement &element)
{
    enum StarType { star_outline, spoke, wheel, polygon, framed_star, star, gear };
    enum CornerType { tip = 0, base = 1 };

    const qreal cx = KoUnit::parseValue(element.attribute("cx"));
    const qreal cy = KoUnit::parseValue(element.attribute("cy"));

    const qreal outerRadius  = qAbs(KoUnit::parseValue(element.attribute("outerradius")));
    qreal innerRadius  = qAbs(KoUnit::parseValue(element.attribute("innerradius")));
    const uint edges  = qMax(element.attribute("edges").toUInt(), static_cast<uint>(3));

    const qreal innerAngle  = element.attribute("innerangle").toUInt();
    qreal angle = element.attribute("angle").toDouble();
    const qreal roundness  = element.attribute("roundness").toDouble();

    const int type = element.attribute("type").toInt();

    QString data;

    if (type == star_outline || type == polygon) {

        const qreal radianStep = M_PI / static_cast<qreal>(edges);

        QPointF lastCorner, lastTangent;
        QPointF currCorner, currTangent;

        for (uint i = 0; i < 2*edges; ++i) {
            uint cornerType = i % 2;
            if (cornerType == base && type == polygon)
                continue;
            const qreal cornerRadius = cornerType == base ? innerRadius : outerRadius;
            const qreal radian = static_cast<qreal>((i+1)*radianStep) + angle * M_PI / 180.;
            currCorner = QPointF(cornerRadius * cos(radian), cornerRadius * sin(radian));
            currTangent = QPointF(currCorner.y()/cornerRadius, -currCorner.x()/cornerRadius);
            currCorner += QPointF(cx, cy);

            if (i == 0) {
                data += QString("M%1,%2 ").arg(currCorner.x()).arg(currCorner.y());
            } else if (roundness > 1e-10 || roundness < -1e-10) {
                // normalized cross product to compute tangential vector for handle point
                const QPointF p1 = lastCorner + roundness * lastTangent;
                const QPointF p2 = currCorner - roundness * currTangent;
                data += QString("C%1,%2 %3,%4 %5,%6").arg(p1.x()).arg(p1.y()).arg(p2.x()).arg(p2.y())
                        .arg(currCorner.x()).arg(currCorner.y());
            } else {
                data += QString("L%1,%2").arg(currCorner.x()).arg(currCorner.y());
            }
            lastCorner = currCorner;
            lastTangent = currTangent;
        }
        data +="Z";
    } else {

        // We start at angle + KarbonGlobal::pi_2:
        QPointF p2, p3;
        QPointF p(cx + outerRadius * cos(angle + KarbonGlobal::pi_2),
                  cy + outerRadius * sin(angle + KarbonGlobal::pi_2));

        data += QString("M%1,%2 ").arg(p.x()).arg(p.y());

        qreal inAngle = KarbonGlobal::twopi / 360 * innerAngle;

        if (type == star) {
            int j = (edges % 2 == 0) ? (edges - 2) / 2 : (edges - 1) / 2;
            //innerRadius = getOptimalInnerRadius( outerRadius, edges, innerAngle );
            int jumpto = 0;
            bool discontinuous = (edges % 4 == 2);

            qreal outerRoundness = (KarbonGlobal::twopi * outerRadius * roundness) / edges;
            qreal nextOuterAngle;

            for (uint i = 1; i < edges + 1; ++i) {
                qreal nextInnerAngle = angle + inAngle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * (jumpto + 0.5);
                p.setX(cx + innerRadius * cos(nextInnerAngle));
                p.setY(cy + innerRadius * sin(nextInnerAngle));
                if (roundness == 0.0) {
                    data += QString("L%1,%2 ").arg(p.x()).arg(p.y());
                } else {
                    nextOuterAngle = angle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * jumpto;
                    p2.setX(cx + outerRadius * cos(nextOuterAngle) -
                            cos(angle + KarbonGlobal::twopi / edges * jumpto) * outerRoundness);
                    p2.setY(cy + outerRadius * sin(nextOuterAngle) -
                            sin(angle + KarbonGlobal::twopi / edges * jumpto) * outerRoundness);

                    data += QString("Q%1,%2 %3,%4 ").arg(p2.x()).arg(p2.y()).arg(p.x()).arg(p.y());
                }

                jumpto = (i * j) % edges;
                nextInnerAngle = angle + inAngle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * (jumpto - 0.5);
                p.setX(cx + innerRadius * cos(nextInnerAngle));
                p.setY(cy + innerRadius * sin(nextInnerAngle));
                data += QString("L%1,%2 ").arg(p.x()).arg(p.y());

                nextOuterAngle = angle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * jumpto;
                p.setX(cx + outerRadius * cos(nextOuterAngle));
                p.setY(cy + outerRadius * sin(nextOuterAngle));

                if (roundness == 0.0) {
                    data += QString("L%1,%2 ").arg(p.x()).arg(p.y());
                } else {
                    p2.setX(cx + innerRadius * cos(nextInnerAngle));
                    p2.setY(cy + innerRadius * sin(nextInnerAngle));

                    p3.setX(cx + outerRadius * cos(nextOuterAngle) +
                            cos(angle + KarbonGlobal::twopi / edges * jumpto) * outerRoundness);
                    p3.setY(cy + outerRadius * sin(nextOuterAngle) +
                            sin(angle + KarbonGlobal::twopi / edges * jumpto) * outerRoundness);

                    data += QString("C%1,%2 %3,%4 %5,%6 ").arg(p2.x()).arg(p2.y())
                            .arg(p3.x()).arg(p3.y()).arg(p.x()).arg(p.y());
                }
                if (discontinuous && i == (edges / 2)) {
                    angle += KarbonGlobal::pi;
                    nextOuterAngle = angle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * jumpto;
                    p.setX(cx + outerRadius * cos(nextOuterAngle));
                    p.setY(cy + outerRadius * sin(nextOuterAngle));
                    data += QString("M%1,%2 ").arg(p.x()).arg(p.y());
                }
            }
        } else {
            if (type == wheel || type == spoke)
                innerRadius = 0.0;

            qreal innerRoundness = (KarbonGlobal::twopi * innerRadius * roundness) / edges;
            qreal outerRoundness = (KarbonGlobal::twopi * outerRadius * roundness) / edges;

            for (uint i = 0; i < edges; ++i) {
                qreal nextOuterAngle = angle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * (i + 1.0);
                qreal nextInnerAngle = angle + inAngle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * (i + 0.5);
                if (type != polygon) {
                    p.setX(cx + innerRadius * cos(nextInnerAngle));
                    p.setY(cy + innerRadius * sin(nextInnerAngle));

                    if (roundness == 0.0) {
                        data += QString("L%1,%2 ").arg(p.x()).arg(p.y());
                    } else {
                        p2.setX(cx + outerRadius *
                                cos(angle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * (i)) -
                                cos(angle + KarbonGlobal::twopi / edges * (i)) * outerRoundness);
                        p2.setY(cy + outerRadius *
                                sin(angle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * (i)) -
                                sin(angle + KarbonGlobal::twopi / edges * (i)) * outerRoundness);

                        p3.setX(cx + innerRadius * cos(nextInnerAngle) +
                                cos(angle + inAngle + KarbonGlobal::twopi / edges * (i + 0.5)) * innerRoundness);
                        p3.setY(cy + innerRadius * sin(nextInnerAngle) +
                                sin(angle + inAngle + KarbonGlobal::twopi / edges * (i + 0.5)) * innerRoundness);

                        if (type == gear) {
                            data += QString("L%1,%2 %3,%4 %5,%6 ")
                                    .arg(p2.x()).arg(p2.y())
                                    .arg(p3.x()).arg(p3.y())
                                    .arg(p.x()).arg(p.y());
                        } else {
                            data += QString("C%1,%2 %3,%4 %5,%6 ").arg(p2.x()).arg(p2.y())
                                    .arg(p3.x()).arg(p3.y()).arg(p.x()).arg(p.y());
                        }
                    }
                }

                p.setX(cx + outerRadius * cos(nextOuterAngle));
                p.setY(cy + outerRadius * sin(nextOuterAngle));

                if (roundness == 0.0) {
                    data += QString("L%1,%2 ").arg(p.x()).arg(p.y());
                } else {
                    p2.setX(cx + innerRadius * cos(nextInnerAngle) -
                            cos(angle + inAngle + KarbonGlobal::twopi / edges * (i + 0.5)) * innerRoundness);
                    p2.setY(cy + innerRadius * sin(nextInnerAngle) -
                            sin(angle + inAngle + KarbonGlobal::twopi / edges * (i + 0.5)) * innerRoundness);

                    p3.setX(cx + outerRadius * cos(nextOuterAngle) +
                            cos(angle + KarbonGlobal::twopi / edges * (i + 1.0)) * outerRoundness);
                    p3.setY(cy + outerRadius * sin(nextOuterAngle) +
                            sin(angle + KarbonGlobal::twopi / edges * (i + 1.0)) * outerRoundness);

                    if (type == gear) {
                        data += QString("L%1,%2 %3,%4 %5,%6 ")
                                .arg(p2.x()).arg(p2.y())
                                .arg(p3.x()).arg(p3.y())
                                .arg(p.x()).arg(p.y());
                    } else {
                        data += QString("C%1,%2 %3,%4 %5,%6 ").arg(p2.x()).arg(p2.y())
                                .arg(p3.x()).arg(p3.y()).arg(p.x()).arg(p.y());
                    }
                }
            }
        }
        if (type == wheel || type == framed_star) {
            data += "Z L";
            for (int i = edges - 1; i >= 0; --i) {
                qreal nextOuterAngle = angle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * (i + 1.0);
                p.setX(cx + outerRadius * cos(nextOuterAngle));
                p.setY(cy + outerRadius * sin(nextOuterAngle));
                data += QString("%1,%2 ").arg(p.x()).arg(p.y());
            }
        }
        data += "Z";
    }

    const QString style = loadStyle(element) + "fill-rule:evenodd;";

    m_svgWriter->startElement("path");
    loadCommon(element);
    m_svgWriter->addAttribute("style", style);
    m_svgWriter->addAttribute("d", data);
    m_svgWriter->endElement(); // path
}

void KarbonImport::loadImage(const KoXmlElement &element)
{
    QString fname = element.attribute("fname");
    QTransform m(element.attribute("m11", "1.0").toDouble(),
              element.attribute("m12", "0.0").toDouble(), 0,
              element.attribute("m21", "0.0").toDouble(),
              element.attribute("m22", "1.0").toDouble(), 0,
              element.attribute("dx", "0.0").toDouble(),
              element.attribute("dy", "0.0").toDouble(), 1);

    QImage image;
    if (!image.load(fname)) {
        warnKarbon1 << "Could not load image " << fname;
        return;
    }

    QByteArray ba;
    QBuffer buffer(&ba);
    if(!buffer.open(QIODevice::WriteOnly))
        return;
    if (!image.save(&buffer, "PNG"))
        return;

    QString trafo = QString("matrix(%1 %2 %3 %4 %5 %6)")
            .arg(m.m11()).arg(m.m12())
            .arg(m.m21()).arg(m.m22())
            .arg(m.dx()).arg(m.dy());

    m_svgWriter->startElement("image");
    loadCommon(element);
    m_svgWriter->addAttribute("transform", trafo);
    m_svgWriter->addAttribute("xlink:href", "data:image/png;base64," + ba.toBase64());
    m_svgWriter->endElement(); // image
}

void KarbonImport::loadText(const KoXmlElement &element)
{
    enum Position { Above, On, Under };

    //int position = element.attribute( "position", "0" ).toInt();
    // TODO: apply alignment value
//     int alignment = element.attribute("alignment", "0").toInt();
    /* TODO reactivate when we have a shadow implementation
    bool shadow = ( element.attribute( "shadow" ).toInt() == 1 );
    bool translucentShadow = ( element.attribute( "translucentshadow" ).toInt() == 1 );
    int shadowAngle = element.attribute( "shadowangle" ).toInt();
    int shadowDistance = element.attribute( "shadowdist" ).toInt();
    qreal offset = element.attribute( "offset" ).toDouble();
    */
    QString text = element.attribute("text", "");

    // catch path of the form Mx1 y1Lx2 y2 indicating a single line segment
    QRegExp exp("M[-+]?[0-9]*\\.?[0-9]+ [-+]?[0-9]*\\.?[0-9]+L[-+]?[0-9]*\\.?[0-9]+ [-+]?[0-9]*\\.?[0-9]+");

    KoXmlElement e = element.firstChild().toElement();
    const bool isOnPath = e.tagName() == "PATH";
    if (isOnPath) {
        QString data = e.attribute("d");
        // check if we have a single line path
        if (exp.exactMatch(data)) {
            QStringList coords = data.split(QRegExp("[M\\sL]"), QString::SkipEmptyParts);
            if (coords.size() == 4) {
                // in old karbon a single line text path was used to specify the direction
                // of the text, however the length of the path was not required to match
                // the length of the text, so we need to extend the path in ta have our
                // text not cut off after importing from svg
                QPointF p1(coords[0].toDouble(), coords[1].toDouble());
                QPointF p2(coords[2].toDouble(), coords[3].toDouble());
                // calculate length of line
                const qreal dx = p2.x()-p1.x();
                const qreal dy = p2.y()-p1.y();
                const qreal currLength = sqrt(dx*dx+dy*dy);
                // read the font properties
                QFont font;
                font.setFamily(element.attribute("family", "Times"));
                font.setPointSizeF(element.attribute("size", "12").toDouble());
                font.setBold(element.attribute("bold").toInt() == 1);
                font.setItalic(element.attribute("italic").toInt() == 1);
                // calculate the required font length
                QFontMetrics metrics(font);
                qreal requiredLength = metrics.width(text);
                if (requiredLength > currLength) {
                    // extend the text path with the required text length to be safe
                    p2 = p1 + requiredLength * QPointF(dx/currLength, dy/currLength);
                    data = QString("M%1 %2L%3 %4").arg(p1.x()).arg(p1.y()).arg(p2.x()).arg(p2.y());
                }
            }
        }
        m_svgWriter->startElement("defs");
        m_svgWriter->startElement("path");
        loadCommon(element);
        m_svgWriter->addAttribute("d", data);
        m_svgWriter->endElement(); // path
        m_svgWriter->endElement(); // defs
    }

    QString pathId = m_lastId;

    QString style = loadStyle(element);
    style += QString("font-family:%1;").arg(element.attribute("family", "Times"));
    style += QString("font-size:%1;").arg(element.attribute("size", "12"));
    if (element.attribute("bold").toInt() == 1) {
        style += "font-weight:bold;";
    }
    if (element.attribute("italic").toInt() == 1) {
        style += "font-style:italic;";
    }

    m_svgWriter->startElement("text");
    // ignore transformation, we are set on a path
    // so that one is already correctly transformed
    loadCommon(element, true);
    m_svgWriter->addAttribute("style", style);
    if (isOnPath) {
        m_svgWriter->startElement("textPath");
        m_svgWriter->addAttribute("xlink:href", "#"+pathId);
        m_svgWriter->addAttribute("startOffset", element.attribute("offset"));
    }
    m_svgWriter->addTextNode(text);
    if (isOnPath) {
        m_svgWriter->endElement(); // textpath
    }
    m_svgWriter->endElement(); // text
}

QString KarbonImport::makeUnique(const QString &id)
{
    QString idBase = id.isEmpty() ? "defitem" : id;
    if (!m_uniqueNames.contains(idBase)) {
        m_uniqueNames.insert(idBase, 0);
        m_lastId = idBase;
    }
    else {
        int counter = m_uniqueNames.value(idBase);
        m_uniqueNames.insert(idBase, counter+1);
        m_lastId = idBase + QString("%1").arg(counter);
    }

    return m_lastId;
}

#include "Karbon1xImport.moc"
