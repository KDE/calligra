/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002, 2003 The Karbon Developers

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "xamlimport.h"
#include "color.h"

#include <KoFilterChain.h>

#include <KCompressionDevice>
#include <KPluginFactory>
#include <kdebug.h>

#include <shapes/vellipse.h>
#include <shapes/vrectangle.h>
#include <shapes/vpolygon.h>
#include <commands/vtransformcmd.h>
#include <core/vsegment.h>
#include <core/vtext.h>
#include <core/vglobal.h>
#include <core/vgroup.h>
#include <core/vimage.h>
#include <core/vlayer.h>
#include <QColor>
#include <QFile>
#include <QList>

K_PLUGIN_FACTORY(XAMLImportFactory, registerPlugin<XAMLImport>();)
K_EXPORT_PLUGIN(XAMLImportFactory("calligrafilters"))

XAMLImport::XAMLImport(KoFilter *, const char *, const QVariantList&) :
        KoFilter(parent),
        outdoc("DOC")
{
    m_gc.setAutoDelete(true);
}

XAMLImport::~XAMLImport()
{
}

KoFilter::ConversionStatus XAMLImport::convert(const QByteArray& from, const QByteArray& to)
{
    // check for proper conversion
    if (to != "application/x-karbon" || from != "image/wvg+xml")
        return KoFilter::NotImplemented;

    //Find the last extension
    QString strExt;
    QString fileIn(m_chain->inputFile());
    const int result = fileIn.findRev('.');
    if (result >= 0) {
        strExt = fileIn.mid(result).lower();
    }

    const KCompressionDevice::CompressionType compressionType =
        (strExt == QLatin1String(".gz"))         //in case of .svg.gz (logical extension)
         || (strExt == QLatin1String(".wvgz")) ? //in case of .wvgz (extension used prioritary)
            KCompressionDevice::GZip :
        (strExt == QLatin1String(".bz2")) ?      //in case of .svg.bz2 (logical extension)
            KCompressionDevice::BZip2 :
            KCompressionDevice::None;

    kDebug(30514) << "File extension: -" << strExt << "- Compression:" << strMime;

    QIODevice* in = new KCompressionDevice(fileIn, compressionType);

    if (!in->open(QIODevice::ReadOnly)) {
        kError(30514) << "Cannot open file! Aborting!" << Qt::endl;
        delete in;
        return KoFilter::FileNotFound;
    }

    int line, col;
    QString errormessage;
    const bool parsed = inpdoc.setContent(in, &errormessage, &line, &col);
    in->close();
    delete in;
    if (! parsed) {
        kError(30514) << "Error while parsing file: "
        << "at line " << line << " column: " << col
        << " message: " << errormessage << Qt::endl;
        // ### TODO: feedback to the user
        return KoFilter::ParsingError;
    }

    // Do the conversion!
    convert();

    KoStoreDevice* out = m_chain->storageFile("root", KoStore::Write);
    if (!out) {
        kError(30514) << "Unable to open output file!" << Qt::endl;
        return KoFilter::StorageCreationError;
    }
    QByteArray cstring = outdoc.toLatin1(); // utf-8 already
    out->write(cstring.data(), cstring.length());

    return KoFilter::OK; // was successful
}

void
XAMLImport::convert()
{
    XAMLGraphicsContext *gc = new XAMLGraphicsContext;
    QDomElement docElem = inpdoc.documentElement();
    KoRect bbox(0, 0, 550.0, 841.0);
    double width = !docElem.attribute("width").isEmpty() ? parseUnit(docElem.attribute("width"), true, false, bbox) : 550.0;
    double height = !docElem.attribute("height").isEmpty() ? parseUnit(docElem.attribute("height"), false, true, bbox) : 841.0;
    m_document.setWidth(width);
    m_document.setHeight(height);
    m_outerRect = m_document.boundingBox();

    // undo y-mirroring
    if (!docElem.attribute("viewBox").isEmpty()) {
        // allow for viewbox def with ',' or whitespace
        QString viewbox(docElem.attribute("viewBox"));
        QStringList points = QStringList::split(' ', viewbox.replace(',', ' ').simplified());

        gc->matrix.scale(width / points[2].toFloat() , height / points[3].toFloat());
        m_outerRect.setWidth(m_outerRect.width() * (points[2].toFloat() / width));
        m_outerRect.setHeight(m_outerRect.height() * (points[3].toFloat() / height));
    }
    m_gc.push(gc);
    parseGroup(0L, docElem);

    QTransform mat;
    mat.scale(1, -1);
    mat.translate(0, -m_document.height());
    VTransformCmd trafo(0L, mat);
    trafo.visit(m_document);
    outdoc = m_document.saveXML();
}

#define DPI 90

double
XAMLImport::toPercentage(QString s)
{
    if (s.endsWith(QLatin1Char('%'))) {
        s.chop(1);
        return s.toDouble();
    }

    return s.toDouble() * 100.0;
}

double
XAMLImport::fromPercentage(QString s)
{
    if (s.endsWith(QLatin1Char('%'))) {
        s.chop(1);
        return s.toDouble() / 100.0;
    }

    return s.toDouble();
}

// parses the number into parameter number
const char *
getNumber(const char *ptr, double &number)
{
    int integer, exponent;
    double decimal, frac;
    int sign, expsign;

    exponent = 0;
    integer = 0;
    frac = 1.0;
    decimal = 0;
    sign = 1;
    expsign = 1;

    // read the sign
    if (*ptr == '+')
        ptr++;
    else if (*ptr == '-') {
        ptr++;
        sign = -1;
    }

    // read the integer part
    while (*ptr != '\0' && *ptr >= '0' && *ptr <= '9')
        integer = (integer * 10) + *(ptr++) - '0';
    if (*ptr == '.') { // read the decimals
        ptr++;
        while (*ptr != '\0' && *ptr >= '0' && *ptr <= '9')
            decimal += (*(ptr++) - '0') * (frac *= 0.1);
    }

    if (*ptr == 'e' || *ptr == 'E') { // read the exponent part
        ptr++;

        // read the sign of the exponent
        if (*ptr == '+')
            ptr++;
        else if (*ptr == '-') {
            ptr++;
            expsign = -1;
        }

        exponent = 0;
        while (*ptr != '\0' && *ptr >= '0' && *ptr <= '9') {
            exponent *= 10;
            exponent += *ptr - '0';
            ptr++;
        }
    }
    number = integer + decimal;
    number *= sign * pow((double)10, double(expsign * exponent));

    return ptr;
}


double
XAMLImport::parseUnit(const QString &unit, bool horiz, bool vert, KoRect bbox)
{
    // TODO : percentage?
    double value = 0;
    const char *start = unit.toLatin1();
    if (!start) {
        return 0;
    }
    const char *end = getNumber(start, value);

    if (uint(end - start) < unit.length()) {
        if (unit.right(2) == "pt")
            value = (value / 72.0) * DPI;
        else if (unit.right(2) == "cm")
            value = (value / 2.54) * DPI;
        else if (unit.right(2) == "pc")
            value = (value / 6.0) * DPI;
        else if (unit.right(2) == "mm")
            value = (value / 25.4) * DPI;
        else if (unit.right(2) == "in")
            value = value * DPI;
        else if (unit.right(2) == "pt")
            value = (value / 72.0) * DPI;
        else if (unit.right(2) == "em")
            value = value * m_gc.current()->font.pointSize() / (sqrt(pow(m_gc.current()->matrix.m11(), 2) + pow(m_gc.current()->matrix.m22(), 2)) / sqrt(2.0));
        else if (unit.right(1) == "%") {
            if (horiz && vert)
                value = (value / 100.0) * (sqrt(pow(bbox.width(), 2) + pow(bbox.height(), 2)) / sqrt(2.0));
            else if (horiz)
                value = (value / 100.0) * bbox.width();
            else if (vert)
                value = (value / 100.0) * bbox.height();
        }
    }
    /*else
    {
     if( m_gc.current() )
     {
      if( horiz && vert )
       value *= sqrt( pow( m_gc.current()->matrix.m11(), 2 ) + pow( m_gc.current()->matrix.m22(), 2 ) ) / sqrt( 2.0 );
      else if( horiz )
       value /= m_gc.current()->matrix.m11();
      else if( vert )
       value /= m_gc.current()->matrix.m22();
     }
    }*/
    return value;
}

QColor
XAMLImport::parseColor(const QString &rgbColor)
{
    int r, g, b;
    keywordToRGB(rgbColor, r, g, b);
    return QColor(r, g, b);
}

void
XAMLImport::parseColor(VColor &color, const QString &s)
{
    if (s.startsWith("rgb(")) {
        QString parse = s.trimmed();
        QStringList colors = QStringList::split(',', parse);
        QString r = colors[0].right((colors[0].length() - 4));
        QString g = colors[1];
        QString b = colors[2].left((colors[2].length() - 1));

        if (r.contains("%")) {
            r = r.left(r.length() - 1);
            r = QString::number(int((double(255 * r.toDouble()) / 100.0)));
        }

        if (g.contains("%")) {
            g = g.left(g.length() - 1);
            g = QString::number(int((double(255 * g.toDouble()) / 100.0)));
        }

        if (b.contains("%")) {
            b = b.left(b.length() - 1);
            b = QString::number(int((double(255 * b.toDouble()) / 100.0)));
        }

        QColor c(r.toInt(), g.toInt(), b.toInt());
        color.set(c.red() / 255.0, c.green() / 255.0, c.blue() / 255.0);
    } else {
        QString rgbColor = s.trimmed();
        QColor c;
        if (rgbColor.startsWith('#')) {
            c = QColor::fromString(rgbColor);
        } else {
            c = parseColor(rgbColor);
        }
        color.set(c.red() / 255.0, c.green() / 255.0, c.blue() / 255.0);
    }
}

void
XAMLImport::parseColorStops(VGradient *gradient, const QDomElement &e)
{
    VColor c;
    for (QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling()) {
        QDomElement stop = n.toElement();
        if (stop.tagName() == "stop") {
            float offset;
            QString temp = stop.attribute("offset");
            if (temp.contains('%')) {
                temp = temp.left(temp.length() - 1);
                offset = temp.toFloat() / 100.0;
            } else
                offset = temp.toFloat();

            if (!stop.attribute("stop-color").isEmpty())
                parseColor(c, stop.attribute("stop-color"));
            else {
                // try style attr
                QString style = stop.attribute("style").simplified();
                QStringList substyles = QStringList::split(';', style);
                for (QStringList::Iterator it = substyles.begin(); it != substyles.end(); ++it) {
                    QStringList substyle = QStringList::split(':', (*it));
                    QString command = substyle[0].trimmed();
                    QString params = substyle[1].trimmed();
                    if (command == "stop-color")
                        parseColor(c, params);
                    if (command == "stop-opacity")
                        c.setOpacity(params.toDouble());
                }

            }
            if (!stop.attribute("stop-opacity").isEmpty())
                c.setOpacity(stop.attribute("stop-opacity").toDouble());
            gradient->addStop(c, offset, 0.5);
        }
    }
}

void
XAMLImport::parseGradient(const QDomElement &e)
{
    GradientHelper gradhelper;
    gradhelper.gradient.clearStops();
    gradhelper.gradient.setRepeatMethod(VGradient::none);

    QString href = e.attribute("xlink:href").mid(1);
    if (!href.isEmpty()) {
        //kDebug() <<"Indexing with href :" << href.toLatin1();
        gradhelper.gradient = m_gradients[ href ].gradient;
    }

    gradhelper.bbox = e.attribute("gradientUnits") != "userSpaceOnUse";

    if (e.tagName() == "linearGradient") {
        if (gradhelper.bbox) {
            gradhelper.gradient.setOrigin(KoPoint(toPercentage(e.attribute("x1", "0%")), toPercentage(e.attribute("y1", "0%"))));
            gradhelper.gradient.setVector(KoPoint(toPercentage(e.attribute("x2", "100%")), toPercentage(e.attribute("y2", "0%"))));
        } else {
            gradhelper.gradient.setOrigin(KoPoint(e.attribute("x1").toDouble(), e.attribute("y1").toDouble()));
            gradhelper.gradient.setVector(KoPoint(e.attribute("x2").toDouble(), e.attribute("y2").toDouble()));
        }
    } else {
        if (gradhelper.bbox) {
            gradhelper.gradient.setOrigin(KoPoint(toPercentage(e.attribute("cx", "50%")), toPercentage(e.attribute("cy", "50%"))));
            gradhelper.gradient.setVector(KoPoint(toPercentage(e.attribute("cx", "50%")) + toPercentage(e.attribute("r", "50%")),
                                                  toPercentage(e.attribute("cy", "50%"))));
            gradhelper.gradient.setFocalPoint(KoPoint(toPercentage(e.attribute("fx", "50%")), toPercentage(e.attribute("fy", "50%"))));
        } else {
            gradhelper.gradient.setOrigin(KoPoint(e.attribute("cx").toDouble(), e.attribute("cy").toDouble()));
            gradhelper.gradient.setFocalPoint(KoPoint(e.attribute("fx").toDouble(), e.attribute("fy").toDouble()));
            gradhelper.gradient.setVector(KoPoint(e.attribute("cx").toDouble() + e.attribute("r").toDouble(), e.attribute("cy").toDouble()));
        }
        gradhelper.gradient.setType(VGradient::radial);
    }
    // handle spread method
    QString spreadMethod = e.attribute("spreadMethod");
    if (!spreadMethod.isEmpty()) {
        if (spreadMethod == "reflect")
            gradhelper.gradient.setRepeatMethod(VGradient::reflect);
        else if (spreadMethod == "repeat")
            gradhelper.gradient.setRepeatMethod(VGradient::repeat);
    }
    parseColorStops(&gradhelper.gradient, e);
    //gradient.setGradientTransform( parseTransform( e.attribute( "gradientTransform" ) ) );
    gradhelper.gradientTransform = VPath::parseTransform(e.attribute("gradientTransform"));
    m_gradients.insert(e.attribute("id"), gradhelper);
}

void
XAMLImport::parsePA(VObject *obj, XAMLGraphicsContext *gc, const QString &command, const QString &params)
{
    VColor fillcolor = gc->fill.color();
    VColor strokecolor = gc->stroke.color();

    if (command == "fill") {
        if (params == "none")
            gc->fill.setType(VFill::none);
        else if (params.startsWith("url(")) {
            unsigned int start = params.find("#") + 1;
            unsigned int end = params.findRev(")");
            QString key = params.mid(start, end - start);
            gc->fill.gradient() = m_gradients[ key ].gradient;
            if (m_gradients[ key ].bbox) {
                // adjust to bbox
                KoRect bbox = obj->boundingBox();
                //kDebug() <<"bbox x :" << bbox.x();
                //kDebug() <<"!!!!!!bbox y :" << bbox.y();
                //kDebug() << gc->fill.gradient().origin().x();
                //kDebug() << gc->fill.gradient().vector().x();
                double offsetx = parseUnit(QString("%1%").arg(gc->fill.gradient().origin().x()), true, false, bbox);
                double offsety = parseUnit(QString("%1%").arg(gc->fill.gradient().origin().y()), false, true, bbox);
                gc->fill.gradient().setOrigin(KoPoint(bbox.x() + offsetx, bbox.y() + offsety));
                offsetx = parseUnit(QString("%1%").arg(gc->fill.gradient().focalPoint().x()), true, false, bbox);
                offsety = parseUnit(QString("%1%").arg(gc->fill.gradient().focalPoint().y()), false, true, bbox);
                gc->fill.gradient().setFocalPoint(KoPoint(bbox.x() + offsetx, bbox.y() + offsety));
                offsetx = parseUnit(QString("%1%").arg(gc->fill.gradient().vector().x()), true, false, bbox);
                offsety = parseUnit(QString("%1%").arg(gc->fill.gradient().vector().y()), false, true, bbox);
                gc->fill.gradient().setVector(KoPoint(bbox.x() + offsetx, bbox.y() + offsety));
                //kDebug() << offsety;
                //kDebug() << gc->fill.gradient().origin().x();
                //kDebug() << gc->fill.gradient().origin().y();
                //kDebug() << gc->fill.gradient().vector().x();
                //kDebug() << gc->fill.gradient().vector().y();
            }
            gc->fill.gradient().transform(m_gradients[ key ].gradientTransform);
            if (!m_gradients[ key ].bbox)
                gc->fill.gradient().transform(gc->matrix);
            gc->fill.setType(VFill::grad);
        } else {
            parseColor(fillcolor,  params);
            gc->fill.setType(VFill::solid);
        }
    } else if (command == "fill-rule") {
        if (params == "nonzero")
            gc->fillRule = winding;
        else if (params == "evenodd")
            gc->fillRule = evenOdd;
    } else if (command == "stroke") {
        if (params == "none")
            gc->stroke.setType(VStroke::none);
        else if (params.startsWith("url(")) {
            unsigned int start = params.find("#") + 1;
            unsigned int end = params.findRev(")");
            QString key = params.mid(start, end - start);
            gc->stroke.gradient() = m_gradients[ key ].gradient;
            gc->stroke.gradient().transform(m_gradients[ key ].gradientTransform);
            gc->stroke.gradient().transform(gc->matrix);
            gc->stroke.setType(VStroke::grad);
        } else {
            parseColor(strokecolor, params);
            gc->stroke.setType(VStroke::solid);
        }
    } else if (command == "stroke-width")
        gc->stroke.setLineWidth(parseUnit(params, true, true, m_outerRect));
    else if (command == "stroke-linejoin") {
        if (params == "miter")
            gc->stroke.setLineJoin(VStroke::joinMiter);
        else if (params == "round")
            gc->stroke.setLineJoin(VStroke::joinRound);
        else if (params == "bevel")
            gc->stroke.setLineJoin(VStroke::joinBevel);
    } else if (command == "stroke-linecap") {
        if (params == "butt")
            gc->stroke.setLineCap(VStroke::capButt);
        else if (params == "round")
            gc->stroke.setLineCap(VStroke::capRound);
        else if (params == "square")
            gc->stroke.setLineCap(VStroke::capSquare);
    } else if (command == "stroke-miterlimit")
        gc->stroke.setMiterLimit(params.toFloat());
    else if (command == "stroke-dasharray") {
        QList<float> array;
        if (params != "none") {
            QStringList dashes = QStringList::split(' ', params);
            for (QStringList::Iterator it = dashes.begin(); it != dashes.end(); ++it)
                array.append((*it).toFloat());
        }
        gc->stroke.dashPattern().setArray(array);
    } else if (command == "stroke-dashoffset")
        gc->stroke.dashPattern().setOffset(params.toFloat());
    // handle opacity
    else if (command == "stroke-opacity")
        strokecolor.setOpacity(fromPercentage(params));
    else if (command == "fill-opacity")
        fillcolor.setOpacity(fromPercentage(params));
    else if (command == "opacity") {
        fillcolor.setOpacity(fromPercentage(params));
        strokecolor.setOpacity(fromPercentage(params));
    } else if (command == "font-family") {
        QString family = params;
        family.replace('\'' , ' ');
        gc->font.setFamily(family);
    } else if (command == "font-size") {
        float pointSize = parseUnit(params);
        pointSize *= gc->matrix.m22() > 0 ? gc->matrix.m22() : -1.0 * gc->matrix.m22();
        gc->font.setPointSizeFloat(pointSize);
    } else if (command == "text-decoration") {
        if (params == "line-through")
            gc->font.setStrikeOut(true);
        else if (params == "underline")
            gc->font.setUnderline(true);
    }
    if (gc->fill.type() != VFill::none)
        gc->fill.setColor(fillcolor, false);
    //if( gc->stroke.type() == VStroke::solid )
    gc->stroke.setColor(strokecolor);
}

void
XAMLImport::addGraphicContext()
{
    XAMLGraphicsContext *gc = new XAMLGraphicsContext;
    // set as default
    if (m_gc.current())
        *gc = *(m_gc.current());
    m_gc.push(gc);
}

void
XAMLImport::setupTransform(const QDomElement &e)
{
    XAMLGraphicsContext *gc = m_gc.current();

    QTransform mat = VPath::parseTransform(e.attribute("transform"));
    gc->matrix = mat * gc->matrix;
}

void
XAMLImport::parseStyle(VObject *obj, const QDomElement &e)
{
    XAMLGraphicsContext *gc = m_gc.current();
    if (!gc) return;

    // try normal PA
    if (!e.attribute("fill").isEmpty())
        parsePA(obj, gc, "fill", e.attribute("fill"));
    if (!e.attribute("fill-rule").isEmpty())
        parsePA(obj, gc, "fill-rule", e.attribute("fill-rule"));
    if (!e.attribute("stroke").isEmpty())
        parsePA(obj, gc, "stroke", e.attribute("stroke"));
    if (!e.attribute("stroke-width").isEmpty())
        parsePA(obj, gc, "stroke-width", e.attribute("stroke-width"));
    if (!e.attribute("stroke-linejoin").isEmpty())
        parsePA(obj, gc, "stroke-linejoin", e.attribute("stroke-linejoin"));
    if (!e.attribute("stroke-linecap").isEmpty())
        parsePA(obj, gc, "stroke-linecap", e.attribute("stroke-linecap"));
    if (!e.attribute("stroke-dasharray").isEmpty())
        parsePA(obj, gc, "stroke-dasharray", e.attribute("stroke-dasharray"));
    if (!e.attribute("stroke-dashoffset").isEmpty())
        parsePA(obj, gc, "stroke-dashoffset", e.attribute("stroke-dashoffset"));
    if (!e.attribute("stroke-opacity").isEmpty())
        parsePA(obj, gc, "stroke-opacity", e.attribute("stroke-opacity"));
    if (!e.attribute("stroke-miterlimit").isEmpty())
        parsePA(obj, gc, "stroke-miterlimit", e.attribute("stroke-miterlimit"));
    if (!e.attribute("fill-opacity").isEmpty())
        parsePA(obj, gc, "fill-opacity", e.attribute("fill-opacity"));
    if (!e.attribute("opacity").isEmpty())
        parsePA(obj, gc, "opacity", e.attribute("opacity"));

    // try style attr
    QString style = e.attribute("style").simplified();
    QStringList substyles = QStringList::split(';', style);
    for (QStringList::Iterator it = substyles.begin(); it != substyles.end(); ++it) {
        QStringList substyle = QStringList::split(':', (*it));
        QString command = substyle[0].trimmed();
        QString params = substyle[1].trimmed();
        parsePA(obj, gc, command, params);
    }

    obj->setFill(gc->fill);
    if (dynamic_cast<VPath *>(obj))
        dynamic_cast<VPath *>(obj)->setFillRule(gc->fillRule);
    // stroke scaling
    double lineWidth = gc->stroke.lineWidth();
    gc->stroke.setLineWidth(lineWidth * sqrt(pow(m_gc.current()->matrix.m11(), 2) + pow(m_gc.current()->matrix.m22(), 2)) / sqrt(2.0));
    obj->setStroke(gc->stroke);
    gc->stroke.setLineWidth(lineWidth);
}

void
XAMLImport::parseFont(const QDomElement &e)
{
    XAMLGraphicsContext *gc = m_gc.current();
    if (!gc) return;

    if (! e.attribute("font-family").isEmpty())
        parsePA(0L, m_gc.current(), "font-family", e.attribute("font-family"));
    if (! e.attribute("font-size").isEmpty())
        parsePA(0L, m_gc.current(), "font-size", e.attribute("font-size"));
    if (! e.attribute("text-decoration").isEmpty())
        parsePA(0L, m_gc.current(), "text-decoration", e.attribute("text-decoration"));
}

void
XAMLImport::parseGroup(VGroup *grp, const QDomElement &e)
{
    bool isDef = false;
    if (e.tagName() == "defs")
        isDef = true;

    for (QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling()) {
        QDomElement b = n.toElement();
        if (b.isNull()) continue;
        VObject *obj = 0L;
        if (b.tagName() == "g") {
            VGroup *group;
            if (grp)
                group = new VGroup(grp);
            else
                group = new VGroup(&m_document);

            addGraphicContext();
            setupTransform(b);
            parseStyle(group, b);
            parseFont(b);
            parseGroup(group, b);

            // handle id
            if (!b.attribute("id").isEmpty())
                group->setName(b.attribute("id"));
            if (grp)
                grp->append(group);
            else
                m_document.append(group);
            delete(m_gc.pop());
            continue;
        }
        if (b.tagName() == "defs") {
            parseGroup(0L, b);    // try for gradients at least
            continue;
        } else if (b.tagName() == "linearGradient" || b.tagName() == "radialGradient") {
            parseGradient(b);
            continue;
        } else if (b.tagName() == "rect" ||
                   b.tagName() == "ellipse" ||
                   b.tagName() == "circle" ||
                   b.tagName() == "line" ||
                   b.tagName() == "polyline" ||
                   b.tagName() == "polygon" ||
                   b.tagName() == "path" ||
                   b.tagName() == "image") {
            if (!isDef)
                obj = createObject(b);
            else
                m_paths.insert(b.attribute("id"), b);
        } else if (b.tagName() == "text") {
            if (isDef)
                m_paths.insert(b.attribute("id"), b);
            else
                createText(grp, b);
        } else if (b.tagName() == "use") {
            double tx = b.attribute("x").toDouble();
            double ty = b.attribute("y").toDouble();

            if (!b.attribute("xlink:href").isEmpty()) {
                QString params = b.attribute("xlink:href");
                unsigned int start = params.find("#") + 1;
                unsigned int end = params.findRev(")");
                QString key = params.mid(start, end - start);
                if (m_paths.contains(key)) {
                    QDomElement a = m_paths[key];
                    obj = createObject(a);
                    m_gc.current()->matrix.translate(tx, ty);
                    parsePA(grp, m_gc.current(), "fill", b.attribute("fill"));
                }
            }
        }
        if (!obj) continue;
        VTransformCmd trafo(0L, m_gc.current()->matrix);
        trafo.visit(*obj);
        parseStyle(obj, b);
        // handle id
        if (!b.attribute("id").isEmpty())
            obj->setName(b.attribute("id"));
        if (grp)
            grp->append(obj);
        else
            m_document.append(obj);
        delete(m_gc.pop());
    }
}

VObject* XAMLImport::findObject(const QString &name, VGroup* group)
{
    if (! group)
        return 0L;

    VObjectListIterator itr = group->objects();

    for (uint objcount = 1; itr.current(); ++itr, objcount++)
        if (itr.current()->state() != VObject::deleted) {
            if (itr.current()->name() == name)
                return itr.current();

            if (dynamic_cast<VGroup *>(itr.current())) {
                VObject *obj = findObject(name, dynamic_cast<VGroup *>(itr.current()));
                if (obj)
                    return obj;
            }
        }

    return 0L;
}

VObject* XAMLImport::findObject(const QString &name)
{
    QVector<VLayer*> vector;
    m_document.layers().toVector(&vector);
    for (int i = vector.count() - 1; i >= 0; i--) {
        if (vector[i]->state() != VObject::deleted) {
            VObject* obj = findObject(name, dynamic_cast<VGroup *>(vector[i]));
            if (obj)
                return obj;
        }
    }

    return 0L;
}

void XAMLImport::createText(VGroup *grp, const QDomElement &b)
{
    VText *text = 0L;
    QString content;
    VSubpath base(0L);
    VPath *path = 0L;

    addGraphicContext();
    setupTransform(b);
    VTransformCmd trafo(0L, m_gc.current()->matrix);

    parseFont(b);

    if (b.hasChildNodes()) {
        if (base.isEmpty() && ! b.attribute("x").isEmpty() && ! b.attribute("y").isEmpty()) {
            double x = parseUnit(b.attribute("x"));
            double y = parseUnit(b.attribute("y"));
            base.moveTo(KoPoint(x, y));
            base.lineTo(KoPoint(x + 10, y));
        }

        for (QDomNode n = b.firstChild(); !n.isNull(); n = n.nextSibling()) {
            QDomElement e = n.toElement();
            if (e.isNull()) {
                content += n.toCharacterData().data();
            } else if (e.tagName() == "textPath") {
                if (e.attribute("xlink:href").isEmpty())
                    continue;

                QString uri = e.attribute("xlink:href");
                unsigned int start = uri.find("#") + 1;
                unsigned int end = uri.findRev(")");
                QString key = uri.mid(start, end - start);
                if (! m_paths.contains(key)) {
                    VObject* obj = findObject(key);
                    if (obj)
                        path = dynamic_cast<VPath*>(obj);
                } else {
                    QDomElement p = m_paths[key];
                    path = dynamic_cast<VPath*>(createObject(p));
                    if (path)
                        path->setState(VObject::deleted);
                }
                if (! path)
                    continue;
                base = *path->paths().getFirst();
                content += e.text();
            } else if (e.tagName() == "tspan") {
                // only use text of tspan element, as we are not supporting text
                // with different styles
                content += e.text();
                if (base.isEmpty() && ! e.attribute("x").isEmpty() && ! e.attribute("y").isEmpty()) {
                    QStringList posX = QStringList::split(", ", e.attribute("x"));
                    QStringList posY = QStringList::split(", ", e.attribute("y"));
                    if (posX.count() && posY.count()) {
                        double x = parseUnit(posX.first());
                        double y = parseUnit(posY.first());
                        base.moveTo(KoPoint(x, y));
                        base.lineTo(KoPoint(x + 10, y));
                    }
                }
            } else if (e.tagName() == "tref") {
                if (e.attribute("xlink:href").isEmpty())
                    continue;

                QString uri = e.attribute("xlink:href");
                unsigned int start = uri.find("#") + 1;
                unsigned int end = uri.findRev(")");
                QString key = uri.mid(start, end - start);

                if (! m_paths.contains(key)) {
                    VObject* obj = findObject(key);
                    if (obj)
                        content += dynamic_cast<VText*>(obj)->text();
                } else {
                    QDomElement p = m_paths[key];
                    content += p.text();
                }
            } else
                continue;
        }
        text = new VText(m_gc.current()->font, base, VText::Above, VText::Left, content.simplified());
    } else {
        VSubpath base(0L);
        double x = parseUnit(b.attribute("x"));
        double y = parseUnit(b.attribute("y"));
        base.moveTo(KoPoint(x, y));
        base.lineTo(KoPoint(x + 10, y));
        text = new VText(m_gc.current()->font, base, VText::Above, VText::Left, b.text().simplified());
    }

    if (text) {
        text->setParent(&m_document);

        parseStyle(text, b);
        trafo.visit(*text);

        if (!b.attribute("id").isEmpty())
            text->setName(b.attribute("id"));

        if (grp)
            grp->append(text);
        else
            m_document.append(text);
    }
    delete(m_gc.pop());
}

VObject* XAMLImport::createObject(const QDomElement &b)
{
    if (b.tagName() == "rect") {
        addGraphicContext();
        double x  = parseUnit(b.attribute("x"), true, false, m_outerRect);
        double y  = parseUnit(b.attribute("y"), false, true, m_outerRect);
        double width = parseUnit(b.attribute("width"), true, false, m_outerRect);
        double height = parseUnit(b.attribute("height"), false, true, m_outerRect);
        setupTransform(b);
        return new VRectangle(0L, KoPoint(x, height + y) , width, height);
    } else if (b.tagName() == "ellipse") {
        addGraphicContext();
        setupTransform(b);
        double rx  = parseUnit(b.attribute("rx"));
        double ry  = parseUnit(b.attribute("ry"));
        double left  = parseUnit(b.attribute("cx")) - rx;
        double top  = parseUnit(b.attribute("cy")) - ry;
        return new VEllipse(0L, KoPoint(left, top), rx * 2.0, ry * 2.0);
    } else if (b.tagName() == "circle") {
        addGraphicContext();
        setupTransform(b);
        double r  = parseUnit(b.attribute("r"));
        double left  = parseUnit(b.attribute("cx")) - r;
        double top  = parseUnit(b.attribute("cy")) - r;
        return new VEllipse(0L, KoPoint(left, top), r * 2.0, r * 2.0);
    } else if (b.tagName() == "line") {
        addGraphicContext();
        setupTransform(b);
        VPath *path = new VPath(&m_document);
        double x1 = b.attribute("x1").isEmpty() ? 0.0 : parseUnit(b.attribute("x1"));
        double y1 = b.attribute("y1").isEmpty() ? 0.0 : parseUnit(b.attribute("y1"));
        double x2 = b.attribute("x2").isEmpty() ? 0.0 : parseUnit(b.attribute("x2"));
        double y2 = b.attribute("y2").isEmpty() ? 0.0 : parseUnit(b.attribute("y2"));
        path->moveTo(KoPoint(x1, y1));
        path->lineTo(KoPoint(x2, y2));
        return path;
    } else if (b.tagName() == "polyline" || b.tagName() == "polygon") {
        addGraphicContext();
        setupTransform(b);
        VPath *path = new VPath(&m_document);
        bool bFirst = true;

        QString points = b.attribute("points").simplified();
        points.replace(',', ' ');
        points.remove('\r');
        points.remove('\n');
        QStringList pointList = QStringList::split(' ', points);
        for (QStringList::Iterator it = pointList.begin(); it != pointList.end(); ++it) {
            KoPoint point;
            point.setX((*it).toDouble());
            point.setY((*it).toDouble());
            if (bFirst) {
                path->moveTo(point);
                bFirst = false;
            } else
                path->lineTo(point);
        }
        if (b.tagName() == "polygon") path->close();
        return path;
    } else if (b.tagName() == "path") {
        addGraphicContext();
        setupTransform(b);
        VPath *path = new VPath(&m_document);
        path->loadSvgPath(b.attribute("d"));
        return path;
    } else if (b.tagName() == "image") {
        addGraphicContext();
        setupTransform(b);
        QString fname = b.attribute("xlink:href");
        return new VImage(0L, fname);
    }

    return 0L;
}

#include <xamlimport.moc>
