/* This file is part of the KDE project
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

#include "Karbon1xImport.h"

#include <KarbonGlobal.h>
#include <KarbonPart.h>

#include <KoFilterChain.h>
#include <KoStoreDevice.h>
#include <KoOdfGraphicStyles.h>
#include <KoPageLayout.h>
#include <KoShape.h>
#include <KoShapeContainer.h>
#include <KoShapeLayer.h>
#include <KoPathShape.h>
#include <KoResourceManager.h>
#include <KoPathShapeLoader.h>
#include <KoShapeGroup.h>
#include <commands/KoShapeGroupCommand.h>
#include <KoLineBorder.h>
#include <pathshapes/ellipse/EllipseShape.h>
#include <pathshapes/rectangle/RectangleShape.h>
#include <pathshapes/star/StarShape.h>
#include <artistictextshape/ArtisticTextShape.h>
#include <pictureshape/PictureShape.h>
#include <KoImageData.h>
#include <KoImageCollection.h>
#include <KoPathPoint.h>
#include <KoZoomHandler.h>
#include <KoPatternBackground.h>
#include <KoColorBackground.h>
#include <KoGradientBackground.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>

#include <KGenericFactory>

#include <QtGui/QTextCursor>

#include <math.h>

K_PLUGIN_FACTORY(KarbonImportFactory, registerPlugin<KarbonImport>();)
K_EXPORT_PLUGIN(KarbonImportFactory("kofficefilters"))

KarbonImport::KarbonImport(QObject*parent, const QVariantList&)
        : KoFilter(parent), m_document(0)
{
}

KarbonImport::~KarbonImport()
{
}

KoFilter::ConversionStatus KarbonImport::convert(const QByteArray& from, const QByteArray& to)
{
    // check for proper conversion
    if (to != "application/vnd.oasis.opendocument.graphics" || from != "application/x-karbon")
        return KoFilter::NotImplemented;

    const QString fileName(m_chain->inputFile());
    if (fileName.isEmpty()) {
        kError() << "No input file name!";
        return KoFilter::StupidError;
    }

    KarbonPart * part = dynamic_cast<KarbonPart*>(m_chain->outputDocument());
    if (! part)
        return KoFilter::CreationError;

    m_document = &part->document();

    KoStore* store = KoStore::createStore(fileName, KoStore::Read);
    if (store && store->hasFile("maindoc.xml")) {

        if (! store->open("maindoc.xml")) {
            kError() << "Opening root has failed";
            delete store;
            return KoFilter::StupidError;
        }
        KoStoreDevice ioMain(store);
        ioMain.open(QIODevice::ReadOnly);
        if (! parseRoot(&ioMain)) {
            kWarning() << "Parsing maindoc.xml has failed! Aborting!";
            delete store;
            return KoFilter::StupidError;
        }
        ioMain.close();
        store->close();
    } else {
        kWarning() << "Opening store has failed. Trying raw XML file!";
        // Be sure to undefine store
        delete store;
        store = 0;

        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        if (! parseRoot(&file)) {
            kError() << "Could not process document! Aborting!";
            file.close();
            return KoFilter::StupidError;
        }
        file.close();
    }

    // We have finished with the input store/file, so close the store (already done for a raw XML file)
    delete store;
    store = 0;

    return KoFilter::OK;
}

bool KarbonImport::parseRoot(QIODevice* io)
{
    int line, col;
    QString errormessage;

    KoXmlDocument inputDoc;
    const bool parsed = inputDoc.setContent(io, &errormessage, &line, &col);

    if (! parsed) {
        kError() << "Error while parsing file: "
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

    bool success = loadXML(doc);

    KoPageLayout pageLayout;

    // <PAPER>
    KoXmlElement paper = doc.namedItem("PAPER").toElement();
    if (!paper.isNull()) {
        pageLayout.format = static_cast<KoPageFormat::Format>(getAttribute(paper, "format", 0));
        pageLayout.orientation = static_cast<KoPageFormat::Orientation>(getAttribute(paper, "orientation", 0));

        if (pageLayout.format == KoPageFormat::CustomSize) {
            pageLayout.width = m_document->pageSize().width();
            pageLayout.height = m_document->pageSize().height();
        } else {
            pageLayout.width = getAttribute(paper, "width", 0.0);
            pageLayout.height = getAttribute(paper, "height", 0.0);
        }
    } else {
        pageLayout.width = getAttribute(doc, "width", 595.277);
        pageLayout.height = getAttribute(doc, "height", 841.891);
    }

    KoXmlElement borders = paper.namedItem("PAPERBORDERS").toElement();
    if (!borders.isNull()) {
        if (borders.hasAttribute("left"))
            pageLayout.leftMargin = borders.attribute("left").toDouble();
        if (borders.hasAttribute("top"))
            pageLayout.topMargin = borders.attribute("top").toDouble();
        if (borders.hasAttribute("right"))
            pageLayout.rightMargin = borders.attribute("right").toDouble();
        if (borders.hasAttribute("bottom"))
            pageLayout.bottomMargin = borders.attribute("bottom").toDouble();
    }

    // TODO set page layout to the output document

    return success;
}

double KarbonImport::getAttribute(KoXmlElement &element, const char *attributeName, double defaultValue)
{
    QString value = element.attribute(attributeName);
    if (! value.isEmpty())
        return value.toDouble();
    else
        return defaultValue;
}

int KarbonImport::getAttribute(KoXmlElement &element, const char *attributeName, int defaultValue)
{
    QString value = element.attribute(attributeName);
    if (! value.isEmpty())
        return value.toInt();
    else
        return defaultValue;
}

bool KarbonImport::loadXML(const KoXmlElement& doc)
{
    if (doc.attribute("mime") != "application/x-karbon" || doc.attribute("syntaxVersion") != "0.1")
        return false;

    double width = doc.attribute("width", "800.0").toDouble();
    double height = doc.attribute("height", "550.0").toDouble();

    m_document->setPageSize(QSizeF(width, height));
    //m_document->setUnit(KoUnit::unit(doc.attribute("unit", KoUnit::unitName(m_document->unit()))));

    m_mirrorMatrix.scale(1.0, -1.0);
    m_mirrorMatrix.translate(0, -m_document->pageSize().height());

    KoShapeLayer * defaulLayer = m_document->layers().first();

    KoXmlElement e;
    forEachElement(e, doc) {
        if (e.tagName() == "LAYER") {
            KoShapeLayer * layer = new KoShapeLayer();
            layer->setZIndex(nextZIndex());
            layer->setVisible(e.attribute("visible") == 0 ? false : true);
            loadGroup(layer, e);

            m_document->insertLayer(layer);
        }
    }

    if (defaulLayer && m_document->layers().count() > 1)
        m_document->removeLayer(defaulLayer);

    return true;
}

void KarbonImport::loadGroup(KoShapeContainer * parent, const KoXmlElement &element)
{
    QList<KoShape*> shapes;

    KoXmlElement e;
    forEachElement(e, element) {
        KoShape * shape = 0;
        if (e.tagName() == "COMPOSITE" || e.tagName() == "PATH") {
            shape = loadPath(e);
        } else if (e.tagName() == "ELLIPSE") {
            shape = loadEllipse(e);
        } else if (e.tagName() == "RECT") {
            shape = loadRect(e);
        } else if (e.tagName() == "POLYLINE") {
            shape = loadPolyline(e);
        } else if (e.tagName() == "POLYGON") {
            shape = loadPolygon(e);
        } else if (e.tagName() == "SINUS") {
            shape = loadSinus(e);
        } else if (e.tagName() == "SPIRAL") {
            shape = loadSpiral(e);
        } else if (e.tagName() == "STAR") {
            shape = loadStar(e);
        } else if (e.tagName() == "GROUP") {
            KoShapeGroup * group = new KoShapeGroup();
            group->setZIndex(nextZIndex());
            loadGroup(group, e);
            shape = group;
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
            shape = loadImage(e);
        } else if (e.tagName() == "TEXT") {
            shape = loadText(e);
        }
        if (shape)
            shapes.append(shape);
    }

    foreach(KoShape * shape, shapes) {
        m_document->add(shape);
    }

    KoShapeGroup * g = dynamic_cast<KoShapeGroup*>(parent);
    if (g) {
        KoShapeGroupCommand cmd(g, shapes);
        cmd.redo();
    } else {
        foreach(KoShape * shape, shapes) {
            parent->addShape(shape);
        }
    }

    loadCommon(parent, element);
}

void KarbonImport::loadStyle(KoShape * shape, const KoXmlElement &element)
{
    // reset fill and stroke first
    shape->setBorder(0);
    shape->setBackground(0);

    KoXmlElement e;
    forEachElement(e, element) {
        if (e.tagName() == "STROKE") {
            loadStroke(shape, e);
        } else if (e.tagName() == "FILL") {
            loadFill(shape, e);
        }
    }
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

    double opacity = element.attribute("opacity", "1.0").toDouble();

    double value[4] = { 0 };

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

QBrush KarbonImport::loadGradient(KoShape * shape, const KoXmlElement &element)
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

    QSizeF shapeSize = shape->size();
    QMatrix shapeMatrix = m_mirrorMatrix * shape->absoluteTransformation(0).inverted();
    origin = KoFlake::toRelative(shapeMatrix.map(origin), shapeSize);
    focal = KoFlake::toRelative(shapeMatrix.map(focal), shapeSize);
    vector = KoFlake::toRelative(shapeMatrix.map(vector), shapeSize);

    int type = element.attribute("type", 0).toInt();
    int spread = element.attribute("repeatMethod", 0).toInt();

    QGradient * gradient = 0;

    switch (type) {
    case linear: {
        QLinearGradient * g = new QLinearGradient();
        g->setStart(origin);
        g->setFinalStop(vector);
        gradient = g;
        break;
    }
    case radial: {
        QPointF diffVec = origin - vector;
        double radius = sqrt(diffVec.x() * diffVec.x() + diffVec.y() * diffVec.y());

        QRadialGradient * g = new QRadialGradient();
        g->setCenter(origin);
        g->setRadius(radius);
        g->setFocalPoint(focal);
        gradient = g;
        break;
    }
    case conic: {
        QPointF dirVec = vector - origin;
        double angle = atan2(dirVec.y(), dirVec.x()) * 180.0 / M_PI;
        QConicalGradient * g = new QConicalGradient();
        g->setCenter(origin);
        g->setAngle(angle);
        gradient = g;
        break;
    }
    }
    if (! gradient)
        return QBrush();

    QGradientStops stops;

    // load stops
    KoXmlElement colorstop;
    forEachElement(colorstop, element) {
        if (colorstop.tagName() == "COLORSTOP") {
            QColor color = loadColor(colorstop.firstChild().toElement());
            double stop = colorstop.attribute("ramppoint", "0.0").toDouble();
            stops.append(QGradientStop(stop, color));
        }
    }
    gradient->setStops(stops);
    gradient->setCoordinateMode(QGradient::ObjectBoundingMode);
    switch (spread) {
    case reflect:
        gradient->setSpread(QGradient::ReflectSpread);
        break;
    case repeat:
        gradient->setSpread(QGradient::RepeatSpread);
        break;
    default:
        gradient->setSpread(QGradient::PadSpread);
        break;
    }

    QBrush gradientBrush(*gradient);
    delete gradient;

    return gradientBrush;
}

void KarbonImport::loadPattern(KoShape * shape, const KoXmlElement &element)
{
    QPointF origin;
    origin.setX(element.attribute("originX", "0.0").toDouble());
    origin.setY(element.attribute("originY", "0.0").toDouble());
    origin = m_mirrorMatrix.map(origin) - shape->position();

    QPointF vector;
    vector.setX(element.attribute("vectorX", "0.0").toDouble());
    vector.setY(element.attribute("vectorY", "0.0").toDouble());
    vector = m_mirrorMatrix.map(vector) - shape->position();

    QPointF dirVec = vector - origin;
    double angle = atan2(dirVec.y(), dirVec.x()) * 180.0 / M_PI;

    QMatrix m;
    m.translate(origin.x(), origin.y());
    m.rotate(angle);

    QString fname = element.attribute("tilename");

    QImage img;
    if (! img.load(fname)) {
        kWarning() << "Failed to load pattern image" << fname;
        return;
    }

    KoImageCollection *imageCollection = m_document->resourceManager()->imageCollection();
    if (imageCollection) {
        KoPatternBackground * newFill = new KoPatternBackground(imageCollection);
        newFill->setPattern(img.mirrored(false, true));
        newFill->setMatrix(m);
        shape->setBackground(newFill);
    }
}

QVector<qreal> KarbonImport::loadDashes(const KoXmlElement& element)
{
    QVector<qreal> dashes;

    KoXmlElement e;
    forEachElement(e, element) {
        if (e.tagName() == "DASH") {
            double value = qMax(0.0, e.attribute("l", "0.0").toDouble());
            dashes.append(value);
        }
    }
    return dashes;
}

void KarbonImport::loadStroke(KoShape * shape, const KoXmlElement &element)
{
    KoLineBorder * border = new KoLineBorder();

    switch (element.attribute("lineCap", "0").toUShort()) {
    case 1:
        border->setCapStyle(Qt::RoundCap); break;
    case 2:
        border->setCapStyle(Qt::SquareCap); break;
    default:
        border->setCapStyle(Qt::FlatCap);
    }

    switch (element.attribute("lineJoin", "0").toUShort()) {
    case 1:
        border->setJoinStyle(Qt::RoundJoin);; break;
    case 2:
        border->setJoinStyle(Qt::BevelJoin); break;
    default:
        border->setJoinStyle(Qt::MiterJoin);
    }

    border->setLineWidth(qMax(0.0, element.attribute("lineWidth", "1.0").toDouble()));
    border->setMiterLimit(qMax(0.0, element.attribute("miterLimit", "10.0").toDouble()));

    bool hasStroke = false;

    KoXmlElement e;
    forEachElement(e, element) {
        if (e.tagName() == "COLOR") {
            border->setColor(loadColor(e));
            hasStroke = true;
        } else if (e.tagName() == "DASHPATTERN") {
            double dashOffset = element.attribute("offset", "0.0").toDouble();
            border->setLineStyle(Qt::CustomDashLine, loadDashes(e));
            border->setDashOffset(dashOffset);
            hasStroke = true;
        } else if (e.tagName() == "GRADIENT") {
            border->setLineBrush(loadGradient(shape, e));
            hasStroke = true;
        }
        /* TODO gradient and pattern on stroke not yet implemented in flake
        else if( e.tagName() == "PATTERN" )
        {
            m_type = patt;
            m_pattern.load( e );
        }
        */
    }

    if (hasStroke)
        shape->setBorder(border);
    else
        delete border;
}

void KarbonImport::loadFill(KoShape * shape, const KoXmlElement &element)
{
    QBrush fill;

    KoXmlElement e;
    forEachElement(e, element) {
        if (e.tagName() == "COLOR") {
            KoColorBackground * newFill = new KoColorBackground(loadColor(e));
            shape->setBackground(newFill);
        }
        if (e.tagName() == "GRADIENT") {
            QBrush brush = loadGradient(shape, e);
            KoGradientBackground * newFill = new KoGradientBackground(*brush.gradient());
            newFill->setMatrix(brush.matrix());
            shape->setBackground(newFill);
        } else if (e.tagName() == "PATTERN") {
            loadPattern(shape, e);
        }
    }
}

void KarbonImport::loadCommon(KoShape * shape, const KoXmlElement &element)
{
    if (! element.attribute("ID").isEmpty())
        shape->setName(element.attribute("ID"));

    QString trafo = element.attribute("transform");

    if (!trafo.isEmpty())
        shape->applyAbsoluteTransformation(KoOdfGraphicStyles::loadTransformation(trafo));

    if (dynamic_cast<KoShapeContainer*>(shape))
        return;

    // apply mirroring
    shape->applyAbsoluteTransformation(m_mirrorMatrix);
}

KoShape * KarbonImport::loadPath(const KoXmlElement &element)
{
    KoPathShape * path = new KoPathShape();

    QString data = element.attribute("d");
    if (data.length() > 0) {
        KoPathShapeLoader loader(path);
        loader.parseSvg(data, true);
        path->normalize();
    }

    path->setFillRule(element.attribute("fillRule") == 0 ? Qt::OddEvenFill : Qt::WindingFill);

    KoXmlElement child;
    forEachElement(child, element) {
        // backward compatibility for karbon before koffice 1.3.x
        if (child.tagName() == "PATH") {
            KoPathShape * subpath = new KoPathShape();

            KoXmlElement segment;
            forEachElement(segment, child) {
                if (segment.tagName() == "MOVE") {
                    subpath->moveTo(QPointF(segment.attribute("x").toDouble(), segment.attribute("y").toDouble()));
                } else if (segment.tagName() == "LINE") {
                    subpath->lineTo(QPointF(segment.attribute("x").toDouble(), segment.attribute("y").toDouble()));
                } else if (segment.tagName() == "CURVE") {
                    QPointF p0(segment.attribute("x1").toDouble(), segment.attribute("y1").toDouble());
                    QPointF p1(segment.attribute("x2").toDouble(), segment.attribute("y2").toDouble());
                    QPointF p2(segment.attribute("x3").toDouble(), segment.attribute("y3").toDouble());
                    subpath->curveTo(p0, p1, p2);
                }
            }

            if (child.attribute("isClosed") == 0 ? false : true)
                path->close();

            path->combine(subpath);
        }
    }

    loadCommon(path, element);
    loadStyle(path, element);
    path->setZIndex(nextZIndex());

    return path;
}

KoShape * KarbonImport::loadEllipse(const KoXmlElement &element)
{
    EllipseShape * ellipse = new EllipseShape();

    double rx = KoUnit::parseValue(element.attribute("rx"));
    double ry = KoUnit::parseValue(element.attribute("ry"));
    ellipse->setSize(QSizeF(2*rx, 2*ry));

    ellipse->setStartAngle(element.attribute("start-angle").toDouble());
    ellipse->setEndAngle(element.attribute("end-angle").toDouble());

    if (element.attribute("kind") == "cut")
        ellipse->setType(EllipseShape::Chord);
    else if (element.attribute("kind") == "section")
        ellipse->setType(EllipseShape::Pie);
    else if (element.attribute("kind") == "arc")
        ellipse->setType(EllipseShape::Arc);

    QPointF center(KoUnit::parseValue(element.attribute("cx")), KoUnit::parseValue(element.attribute("cy")));
    ellipse->setAbsolutePosition(center);

    loadCommon(ellipse, element);
    loadStyle(ellipse, element);
    ellipse->setZIndex(nextZIndex());

    return ellipse;
}

KoShape * KarbonImport::loadRect(const KoXmlElement &element)
{
    RectangleShape * rect = new RectangleShape();

    double w  = KoUnit::parseValue(element.attribute("width"), 10.0);
    double h = KoUnit::parseValue(element.attribute("height"), 10.0);
    rect->setSize(QSizeF(w, h));

    double x = KoUnit::parseValue(element.attribute("x"));
    double y = KoUnit::parseValue(element.attribute("y"));
    rect->setAbsolutePosition(QPointF(x, y), KoFlake::BottomLeftCorner);

    double rx  = KoUnit::parseValue(element.attribute("rx"));
    double ry  = KoUnit::parseValue(element.attribute("ry"));
    rect->setCornerRadiusX(rx / (0.5 * w) * 100.0);
    rect->setCornerRadiusY(ry / (0.5 * h) * 100.0);

    loadCommon(rect, element);
    loadStyle(rect, element);
    rect->setZIndex(nextZIndex());

    return rect;
}

KoShape * KarbonImport::loadPolyline(const KoXmlElement &element)
{
    KoPathShape * polyline = new KoPathShape();

    QString points = element.attribute("points").simplified();

    bool bFirst = true;

    points.replace(',', ' ');
    points.remove('\r');
    points.remove('\n');
    QStringList pointList = points.split(' ');
    QStringList::Iterator end(pointList.end());
    for (QStringList::Iterator it = pointList.begin(); it != end; ++it) {
        QPointF point;
        point.setX((*it).toDouble());
        point.setY((*++it).toDouble());
        if (bFirst) {
            polyline->moveTo(point);
            bFirst = false;
        } else
            polyline->lineTo(point);
    }

    loadCommon(polyline, element);
    loadStyle(polyline, element);
    polyline->setZIndex(nextZIndex());

    return polyline;
}

KoShape * KarbonImport::loadPolygon(const KoXmlElement &element)
{
    KoPathShape * polygon = new KoPathShape();

    QString points = element.attribute("points").simplified();

    bool bFirst = true;

    points.replace(',', ' ');
    points.remove('\r');
    points.remove('\n');
    QStringList pointList = points.split(' ');
    QStringList::Iterator end(pointList.end());
    for (QStringList::Iterator it = pointList.begin(); it != end; ++it) {
        QPointF point;
        point.setX((*it).toDouble());
        point.setY((*++it).toDouble());
        if (bFirst) {
            polygon->moveTo(point);
            bFirst = false;
        } else
            polygon->lineTo(point);
    }
    polygon->close();

    double x = KoUnit::parseValue(element.attribute("x"));
    double y = KoUnit::parseValue(element.attribute("y"));
    polygon->setAbsolutePosition(QPointF(x, y), KoFlake::TopLeftCorner);

    loadCommon(polygon, element);
    loadStyle(polygon, element);
    polygon->setZIndex(nextZIndex());

    return polygon;
}

KoShape * KarbonImport::loadSinus(const KoXmlElement &element)
{
    KoPathShape * sinus = new KoPathShape();

    uint periods = element.attribute("periods").toUInt();

    QPointF p1, p2, p3;
    sinus->moveTo(QPointF(0, 0));

    for (uint i = 0; i < periods; ++i) {
        p1.setX(i + 1.0 / 24.0);
        p1.setY((2.0 * KarbonGlobal::sqrt2 - 1.0) * KarbonGlobal::one_7);
        p2.setX(i + 1.0 / 12.0);
        p2.setY((4.0 * KarbonGlobal::sqrt2 - 2.0) * KarbonGlobal::one_7);
        p3.setX(i + 1.0 / 8.0);
        p3.setY(KarbonGlobal::sqrt2 * 0.5);
        sinus->curveTo(p1, p2, p3);

        p1.setX(i + 1.0 / 6.0);
        p1.setY((3.0 * KarbonGlobal::sqrt2 + 2.0) * KarbonGlobal::one_7);
        p2.setX(i + 5.0 / 24.0);
        p2.setY(1.0);
        p3.setX(i + 1.0 / 4.0);
        p3.setY(1.0);
        sinus->curveTo(p1, p2, p3);

        p1.setX(i + 7.0 / 24.0);
        p1.setY(1.0);
        p2.setX(i + 1.0 / 3.0);
        p2.setY((3.0 * KarbonGlobal::sqrt2 + 2.0) * KarbonGlobal::one_7);
        p3.setX(i + 3.0 / 8.0);
        p3.setY(KarbonGlobal::sqrt2 * 0.5);
        sinus->curveTo(p1, p2, p3);

        p1.setX(i + 5.0 / 12.0);
        p1.setY((4.0 * KarbonGlobal::sqrt2 - 2.0) * KarbonGlobal::one_7);
        p2.setX(i + 11.0 / 24.0);
        p2.setY((2.0 * KarbonGlobal::sqrt2 - 1.0) * KarbonGlobal::one_7);
        p3.setX(i + 1.0 / 2.0);
        p3.setY(0.0);
        sinus->curveTo(p1, p2, p3);

        p1.setX(i + 13.0 / 24.0);
        p1.setY(-(2.0 * KarbonGlobal::sqrt2 - 1.0) * KarbonGlobal::one_7);
        p2.setX(i + 7.0 / 12.0);
        p2.setY(-(4.0 * KarbonGlobal::sqrt2 - 2.0) * KarbonGlobal::one_7);
        p3.setX(i + 5.0 / 8.0);
        p3.setY(-KarbonGlobal::sqrt2 * 0.5);
        sinus->curveTo(p1, p2, p3);

        p1.setX(i + 2.0 / 3.0);
        p1.setY(-(3.0 * KarbonGlobal::sqrt2 + 2.0) * KarbonGlobal::one_7);
        p2.setX(i + 17.0 / 24.0);
        p2.setY(-1.0);
        p3.setX(i + 3.0 / 4.0);
        p3.setY(-1.0);
        sinus->curveTo(p1, p2, p3);

        p1.setX(i + 19.0 / 24.0);
        p1.setY(-1.0);
        p2.setX(i + 5.0 / 6.0);
        p2.setY(-(3.0 * KarbonGlobal::sqrt2 + 2.0) * KarbonGlobal::one_7);
        p3.setX(i + 7.0 / 8.0);
        p3.setY(-KarbonGlobal::sqrt2 * 0.5);
        sinus->curveTo(p1, p2, p3);

        p1.setX(i + 11.0 / 12.0);
        p1.setY(-(4.0 * KarbonGlobal::sqrt2 - 2.0) * KarbonGlobal::one_7);
        p2.setX(i + 23.0 / 24.0);
        p2.setY(-(2.0 * KarbonGlobal::sqrt2 - 1.0) * KarbonGlobal::one_7);
        p3.setX(i + 1.0);
        p3.setY(0.0);
        sinus->curveTo(p1, p2, p3);
    }

    sinus->normalize();

    double x = KoUnit::parseValue(element.attribute("x"));
    double y = KoUnit::parseValue(element.attribute("y"));

    double w  = KoUnit::parseValue(element.attribute("width"), 10.0);
    double h = KoUnit::parseValue(element.attribute("height"), 10.0);

    sinus->setAbsolutePosition(QPointF(x, y - h)/*, KoFlake::TopLeftCorner*/);
    sinus->setSize(QSizeF(w / periods, h));

    loadCommon(sinus, element);
    loadStyle(sinus, element);
    sinus->setZIndex(nextZIndex());

    return sinus;
}

KoShape * KarbonImport::loadSpiral(const KoXmlElement &element)
{
    enum SpiralType { round, rectangular };

    KoPathShape * spiral = new KoPathShape();

    double radius  = qAbs(KoUnit::parseValue(element.attribute("radius")));
    double angle = element.attribute("angle").toDouble();
    double fade = element.attribute("fade").toDouble();

    double cx = KoUnit::parseValue(element.attribute("cx"));
    double cy = KoUnit::parseValue(element.attribute("cy"));

    uint segments  = element.attribute("segments").toUInt();
    int clockwise = element.attribute("clockwise").toInt();
    int type = element.attribute("type").toInt();


    // It makes sense to have at least one segment:
    if (segments < 1)
        segments = 1;

    // Fall back, when fade is out of range:
    if (fade <= 0.0 || fade >= 1.0)
        fade = 0.5;

    spiral->setFillRule(Qt::WindingFill);

    // advance by pi/2 clockwise or cclockwise?
    double adv_ang = (clockwise ? 1.0 : -1.0) * 90.0;
    double adv_rad = (clockwise ? -1.0 : 1.0) * KarbonGlobal::pi_2;
    // radius of first segment is non-faded radius:
    double r = radius;

    QPointF oldP(0.0, (clockwise ? -1.0 : 1.0) * radius);
    QPointF newP;
    QPointF newCenter(0.0, 0.0);

    spiral->moveTo(oldP);

    double startAngle = clockwise ? 90.0 : -90.0;

    for (uint i = 0; i < segments; ++i) {

        if (type == round) {
            spiral->arcTo(r, r, startAngle, 90);
        } else {
            newP.setX(r * cos(adv_rad * (i + 2)) + newCenter.x());
            newP.setY(r * sin(adv_rad * (i + 2)) + newCenter.y());

            spiral->lineTo(newP);

            newCenter += (newP - newCenter) * (1.0 - fade);
            oldP = newP;
        }

        r *= fade;
        startAngle += adv_ang;
    }

    QPointF topLeft = spiral->outline().boundingRect().topLeft();
    spiral->normalize();

    QMatrix m;

    // sadly it's not feasible to simply add angle while creation.
    // make cw-spiral start at mouse-pointer
    // one_pi_180 = 1/(pi/180) = 180/pi.
    m.rotate((angle + (clockwise ? KarbonGlobal::pi : 0.0)) * KarbonGlobal::one_pi_180);

    spiral->applyAbsoluteTransformation(m);
    spiral->setAbsolutePosition(spiral->absolutePosition() + QPointF(cx, cy));

    loadCommon(spiral, element);
    loadStyle(spiral, element);
    spiral->setZIndex(nextZIndex());

    return spiral;
}

KoShape * KarbonImport::loadStar(const KoXmlElement &element)
{
    enum StarType { star_outline, spoke, wheel, polygon, framed_star, star, gear };

    double cx = KoUnit::parseValue(element.attribute("cx"));
    double cy = KoUnit::parseValue(element.attribute("cy"));

    double outerRadius  = qAbs(KoUnit::parseValue(element.attribute("outerradius")));
    double innerRadius  = qAbs(KoUnit::parseValue(element.attribute("innerradius")));
    uint edges  = qMax(element.attribute("edges").toUInt(), static_cast<uint>(3));

    double innerAngle  = element.attribute("innerangle").toUInt();
    double angle = element.attribute("angle").toDouble();

    double roundness  = element.attribute("roundness").toDouble();

    int type = element.attribute("type").toInt();

    KoPathShape * starShape = 0;

    if (type == star_outline || type == polygon) {
        StarShape * paramStar = new StarShape();

        paramStar->setCornerCount(edges);
        paramStar->setBaseRadius(innerRadius);
        paramStar->setTipRadius(outerRadius);
        paramStar->setBaseRoundness(roundness);
        paramStar->setTipRoundness(roundness);
        paramStar->setConvex(type == polygon);

        QPointF centerPos = paramStar->absolutePosition(KoFlake::TopLeftCorner) + paramStar->starCenter();
        QMatrix m;
        m.translate(centerPos.x(), centerPos.y());
        m.rotate((angle + KarbonGlobal::pi) * KarbonGlobal::one_pi_180);
        paramStar->applyAbsoluteTransformation(m);

        starShape = paramStar;
    } else {

        KoPathShape * pathStar = new KoPathShape();

        // We start at angle + KarbonGlobal::pi_2:
        QPointF p2, p3;
        QPointF p(
            outerRadius * cos(angle + KarbonGlobal::pi_2),
            outerRadius * sin(angle + KarbonGlobal::pi_2));
        pathStar->moveTo(p);

        double inAngle = KarbonGlobal::twopi / 360 * innerAngle;

        if (type == star) {
            int j = (edges % 2 == 0) ? (edges - 2) / 2 : (edges - 1) / 2;
            //innerRadius = getOptimalInnerRadius( outerRadius, edges, innerAngle );
            int jumpto = 0;
            bool discontinueous = (edges % 4 == 2);

            double outerRoundness = (KarbonGlobal::twopi * outerRadius * roundness) / edges;
            double nextOuterAngle;

            for (uint i = 1; i < edges + 1; ++i) {
                double nextInnerAngle = angle + inAngle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * (jumpto + 0.5);
                p.setX(innerRadius * cos(nextInnerAngle));
                p.setY(innerRadius * sin(nextInnerAngle));
                if (roundness == 0.0)
                    pathStar->lineTo(p);
                else {
                    nextOuterAngle = angle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * jumpto;
                    p2.setX(outerRadius * cos(nextOuterAngle) -
                            cos(angle + KarbonGlobal::twopi / edges * jumpto) * outerRoundness);
                    p2.setY(outerRadius * sin(nextOuterAngle) -
                            sin(angle + KarbonGlobal::twopi / edges * jumpto) * outerRoundness);

                    pathStar->curveTo(p2, p, p);
                }

                jumpto = (i * j) % edges;
                nextInnerAngle = angle + inAngle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * (jumpto - 0.5);
                p.setX(innerRadius * cos(nextInnerAngle));
                p.setY(innerRadius * sin(nextInnerAngle));
                pathStar->lineTo(p);

                nextOuterAngle = angle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * jumpto;
                p.setX(outerRadius * cos(nextOuterAngle));
                p.setY(outerRadius * sin(nextOuterAngle));

                if (roundness == 0.0)
                    pathStar->lineTo(p);
                else {
                    p2.setX(innerRadius * cos(nextInnerAngle));
                    p2.setY(innerRadius * sin(nextInnerAngle));

                    p3.setX(outerRadius * cos(nextOuterAngle) +
                            cos(angle + KarbonGlobal::twopi / edges * jumpto) * outerRoundness);
                    p3.setY(outerRadius * sin(nextOuterAngle) +
                            sin(angle + KarbonGlobal::twopi / edges * jumpto) * outerRoundness);

                    pathStar->curveTo(p2, p3, p);
                }
                if (discontinueous && i == (edges / 2)) {
                    angle += KarbonGlobal::pi;
                    nextOuterAngle = angle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * jumpto;
                    p.setX(outerRadius * cos(nextOuterAngle));
                    p.setY(outerRadius * sin(nextOuterAngle));
                    pathStar->moveTo(p);
                }
            }
        } else {
            if (type == wheel || type == spoke)
                innerRadius = 0.0;

            double innerRoundness = (KarbonGlobal::twopi * innerRadius * roundness) / edges;
            double outerRoundness = (KarbonGlobal::twopi * outerRadius * roundness) / edges;

            for (uint i = 0; i < edges; ++i) {
                double nextOuterAngle = angle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * (i + 1.0);
                double nextInnerAngle = angle + inAngle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * (i + 0.5);
                if (type != polygon) {
                    p.setX(innerRadius * cos(nextInnerAngle));
                    p.setY(innerRadius * sin(nextInnerAngle));

                    if (roundness == 0.0)
                        pathStar->lineTo(p);
                    else {
                        p2.setX(outerRadius *
                                cos(angle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * (i)) -
                                cos(angle + KarbonGlobal::twopi / edges * (i)) * outerRoundness);
                        p2.setY(outerRadius *
                                sin(angle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * (i)) -
                                sin(angle + KarbonGlobal::twopi / edges * (i)) * outerRoundness);

                        p3.setX(innerRadius * cos(nextInnerAngle) +
                                cos(angle + inAngle + KarbonGlobal::twopi / edges * (i + 0.5)) * innerRoundness);
                        p3.setY(innerRadius * sin(nextInnerAngle) +
                                sin(angle + inAngle + KarbonGlobal::twopi / edges * (i + 0.5)) * innerRoundness);

                        if (type == gear) {
                            pathStar->lineTo(p2);
                            pathStar->lineTo(p3);
                            pathStar->lineTo(p);
                        } else
                            pathStar->curveTo(p2, p3, p);
                    }
                }

                p.setX(outerRadius * cos(nextOuterAngle));
                p.setY(outerRadius * sin(nextOuterAngle));

                if (roundness == 0.0)
                    pathStar->lineTo(p);
                else {
                    p2.setX(innerRadius * cos(nextInnerAngle) -
                            cos(angle + inAngle + KarbonGlobal::twopi / edges * (i + 0.5)) * innerRoundness);
                    p2.setY(innerRadius * sin(nextInnerAngle) -
                            sin(angle + inAngle + KarbonGlobal::twopi / edges * (i + 0.5)) * innerRoundness);

                    p3.setX(outerRadius * cos(nextOuterAngle) +
                            cos(angle + KarbonGlobal::twopi / edges * (i + 1.0)) * outerRoundness);
                    p3.setY(outerRadius * sin(nextOuterAngle) +
                            sin(angle + KarbonGlobal::twopi / edges * (i + 1.0)) * outerRoundness);

                    if (type == gear) {
                        pathStar->lineTo(p2);
                        pathStar->lineTo(p3);
                        pathStar->lineTo(p);
                    } else
                        pathStar->curveTo(p2, p3, p);
                }
            }
        }
        if (type == wheel || type == framed_star) {
            pathStar->close();
            for (int i = edges - 1; i >= 0; --i) {
                double nextOuterAngle = angle + KarbonGlobal::pi_2 + KarbonGlobal::twopi / edges * (i + 1.0);
                p.setX(outerRadius * cos(nextOuterAngle));
                p.setY(outerRadius * sin(nextOuterAngle));
                pathStar->lineTo(p);
            }
        }
        pathStar->close();
        pathStar->normalize();

        starShape = pathStar;
    }

    starShape->setFillRule(Qt::OddEvenFill);

    // translate path to center:
    QMatrix m;
    m.translate(cx, cy);
    starShape->applyAbsoluteTransformation(m);

    loadCommon(starShape, element);
    loadStyle(starShape, element);
    starShape->setZIndex(nextZIndex());

    return starShape;
}

KoShape * KarbonImport::loadImage(const KoXmlElement &element)
{
    QString fname = element.attribute("fname");
    QMatrix m(element.attribute("m11", "1.0").toDouble(),
              element.attribute("m12", "0.0").toDouble(),
              element.attribute("m21", "0.0").toDouble(),
              element.attribute("m22", "1.0").toDouble(),
              element.attribute("dx", "0.0").toDouble(),
              element.attribute("dy", "0.0").toDouble());

    QImage img;
    if (!img.load(fname)) {
        kWarning() << "Could not load image " << fname;
        return 0;
    }

    KoImageData * data = m_document->imageCollection()->createImageData(img.mirrored(false, true));
    if (! data)
        return 0;

    KoShape * picture = createShape("PictureShape");
    picture->setUserData(data);
    picture->setSize(img.size());
    picture->setTransformation(m);

    loadCommon(picture, element);
    picture->setZIndex(nextZIndex());

    return picture;
}

KoShape * KarbonImport::loadText(const KoXmlElement &element)
{
    QFont font;
    font.setFamily(element.attribute("family", "Times"));
    font.setPointSize(element.attribute("size", "12").toInt());
    font.setItalic(element.attribute("italic").toInt() == 1);
    font.setWeight(QFont::Normal);
    font.setBold(element.attribute("bold").toInt() == 1);

    enum Position { Above, On, Under };

    //int position = element.attribute( "position", "0" ).toInt();
    int alignment = element.attribute("alignment", "0").toInt();
    /* TODO reactivate when we have a shadow implementation
    bool shadow = ( element.attribute( "shadow" ).toInt() == 1 );
    bool translucentShadow = ( element.attribute( "translucentshadow" ).toInt() == 1 );
    int shadowAngle = element.attribute( "shadowangle" ).toInt();
    int shadowDistance = element.attribute( "shadowdist" ).toInt();
    double offset = element.attribute( "offset" ).toDouble();
    */
    QString text = element.attribute("text", "");

    ArtisticTextShape * textShape = new ArtisticTextShape();
    if (! textShape)
        return 0;

    textShape->setFont(font);
    textShape->setText(text);
    textShape->setTextAnchor(static_cast<ArtisticTextShape::TextAnchor>(alignment));

    KoXmlElement e = element.firstChild().toElement();
    if (e.tagName() == "PATH") {
        // as long as there is no text on path support, just try to get a transformation
        // if the path is only a single line
        KoPathShape * path = dynamic_cast<KoPathShape*>(loadPath(e));
        if (path) {
            QMatrix matrix = path->absoluteTransformation(0);
            QPainterPath outline = matrix.map(path->outline());
            qreal outlineLength = outline.length();
            qreal textLength = textShape->size().width();
            qreal diffLength = textLength - outlineLength;
            if (diffLength > 0.0) {
                // elongate path so that text fits completely on it
                int subpathCount = path->subpathCount();
                int subpathPointCount = path->subpathPointCount(subpathCount - 1);
                KoPathPoint * lastPoint = path->pointByIndex(KoPathPointIndex(subpathCount - 1, subpathPointCount - 1));
                KoPathPoint * prevLastPoint = path->pointByIndex(KoPathPointIndex(subpathCount - 1, subpathPointCount - 2));
                if (lastPoint && prevLastPoint) {
                    QPointF tangent;
                    if (lastPoint->activeControlPoint1())
                        tangent = matrix.map(lastPoint->point()) - matrix.map(lastPoint->controlPoint1());
                    else if (prevLastPoint->activeControlPoint2())
                        tangent = matrix.map(lastPoint->point()) - matrix.map(prevLastPoint->controlPoint2());
                    else
                        tangent = matrix.map(lastPoint->point()) - matrix.map(prevLastPoint->point());

                    // normalize tangent vector
                    qreal tangentLength = sqrt(tangent.x() * tangent.x() + tangent.y() * tangent.y());
                    tangent /= tangentLength;
                    path->lineTo(matrix.inverted().map(matrix.map(lastPoint->point()) + diffLength * tangent));
                    path->normalize();
                    outline = path->absoluteTransformation(0).map(path->outline());
                }
            }
            textShape->putOnPath(outline);
            textShape->setStartOffset(element.attribute("offset").toDouble());
            delete path;
        }
    }

    loadCommon(textShape, element);
    loadStyle(textShape, element);
    textShape->setZIndex(nextZIndex());
    textShape->applyAbsoluteTransformation(m_mirrorMatrix.inverted());

    return textShape;
}

int KarbonImport::nextZIndex()
{
    static int zIndex = 0;

    return zIndex++;
}

KoShape * KarbonImport::createShape(const QString &shapeID) const
{
    KoShapeFactoryBase * factory = KoShapeRegistry::instance()->get(shapeID);
    if (! factory) {
        kWarning() << "Could not find factory for shape id" << shapeID;
        return 0;
    }

    KoShape * shape = factory->createDefaultShape(m_document->resourceManager());
    if (shape && shape->shapeId().isEmpty())
        shape->setShapeId(factory->id());

    KoPathShape * path = dynamic_cast<KoPathShape*>(shape);
    if (path && shapeID == KoPathShapeId)
        path->clear();
    // reset tranformation that might come from the default shape
    shape->setTransformation(QMatrix());

    return shape;
}
