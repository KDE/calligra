/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
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

#include "SvgOutputDev.h"

#include "PdfImportDebug.h"

// Poppler includes
#include <GfxState.h>
#include <Stream.h>
#include <GfxFont.h>

#include <QFile>
#include <QTextStream>
#include <QSizeF>
#include <QBuffer>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QImage>

class SvgOutputDev::Private
{
public:
    Private(const QString &fname)
            : svgFile(fname), defs(0), body(0), state(true)
            , brush(Qt::SolidPattern) {}

    ~Private() {
        delete defs;
        delete body;
    }

    QFile svgFile;
    QString bodyData;
    QString defsData;
    QTextStream * defs;
    QTextStream * body;
    bool state;
    QSizeF pageSize;
    QPen pen;
    QBrush brush;
};

SvgOutputDev::SvgOutputDev(const QString &fileName)
        : d(new Private(fileName))
{
    if (! d->svgFile.open(QIODevice::WriteOnly)) {
        d->state = false;
        return;
    }

    d->body = new QTextStream(&d->bodyData, QIODevice::ReadWrite);
    d->defs = new QTextStream(&d->defsData, QIODevice::ReadWrite);
}

SvgOutputDev::~SvgOutputDev()
{
    delete d;
}

bool SvgOutputDev::isOk()
{
    return d->state;
}

bool SvgOutputDev::upsideDown()
{
    return true;
}

bool SvgOutputDev::useDrawChar()
{
    return false;
}

bool SvgOutputDev::interpretType3Chars()
{
    return false;
}

void SvgOutputDev::startPage(int pageNum, GfxState *state, XRef */*xref*/)
{
    debugPdf << "starting page" << pageNum;
    d->pageSize = QSizeF(state->getPageWidth(), state->getPageHeight());
    debugPdf << "page size =" << d->pageSize;

    *d->body << "<g id=\"" << QString("%1").arg(pageNum, (int)3, (int)10, QLatin1Char('0')) << "\"" << endl;
    if (pageNum != 1)
        *d->body << " display=\"none\"";
    *d->body << ">" << endl;
}

void SvgOutputDev::endPage()
{
    debugPdf << "ending page";
    *d->body << "</g>" << endl;
}

void SvgOutputDev::dumpContent()
{
    debugPdf << "dumping pages";

    QTextStream stream(&d->svgFile);

    stream << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl;
    stream << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\" ";
    stream << "\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">" << endl;

    // add some PR.  one line is more than enough.
    stream << "<!-- Created using Karbon, part of Calligra: http://www.calligra.org/karbon -->" << endl;

    stream << "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" ";
    stream << "width=\"" << d->pageSize.width() << "px\" height=\"" << d->pageSize.height() << "px\">" << endl;

    stream << "<defs>" << endl;
    stream << d->defsData;
    stream << "</defs>" << endl;

    stream << d->bodyData;
    stream << "</svg>" << endl;

    d->svgFile.close();
}

void SvgOutputDev::stroke(GfxState * state)
{
    QString path = convertPath(state->getPath());
    *d->body << "<path";
    *d->body << " transform=\"" << convertMatrix(state->getCTM()) << "\"";
    *d->body << printStroke();
    *d->body << " fill=\"none\"";
    *d->body << " d=\"" << path << "\"";
    *d->body << "/>" << endl;
}

void SvgOutputDev::fill(GfxState * state)
{
    QString path = convertPath(state->getPath());
    *d->body << "<path";
    *d->body << " transform=\"" << convertMatrix(state->getCTM()) << "\"";
    *d->body << printFill();
    *d->body << " fill-rule=\"nonzero\"";
    *d->body << " d=\"" << path << "\"";
    *d->body << "/>" << endl;
}

void SvgOutputDev::eoFill(GfxState *state)
{
    QString path = convertPath(state->getPath());
    *d->body << "<path";
    *d->body << " transform=\"" << convertMatrix(state->getCTM()) << "\"";
    *d->body << printFill();
    *d->body << " fill-rule=\"evenodd\"";
    *d->body << " d=\"" << path << "\"";
    *d->body << "/>" << endl;
}

#ifdef HAVE_POPPLER_PRE_0_83
QString SvgOutputDev::convertPath(GfxPath *path)
#else
QString SvgOutputDev::convertPath(const GfxPath *path)
#endif
{
    if (! path)
        return QString();

    QString output;

    for (int i = 0; i < path->getNumSubpaths(); ++i) {
#ifdef HAVE_POPPLER_PRE_0_83
        GfxSubpath * subpath = path->getSubpath(i);
#else
        const GfxSubpath * subpath = path->getSubpath(i);
#endif
        if (subpath->getNumPoints() > 0) {
            output += QString("M%1 %2").arg(subpath->getX(0)).arg(subpath->getY(0));
            int j = 1;
            while (j < subpath->getNumPoints()) {
                if (subpath->getCurve(j)) {
                    output += QString("C%1 %2 %3 %4 %5 %6")
                              .arg(subpath->getX(j)).arg(subpath->getY(j))
                              .arg(subpath->getX(j + 1)).arg(subpath->getY(j + 1))
                              .arg(subpath->getX(j + 2)).arg(subpath->getY(j + 2));
                    j += 3;
                } else {
                    output += QString("L%1 %2").arg(subpath->getX(j)).arg(subpath->getY(j));
                    ++j;
                }
            }
            if (subpath->isClosed()) {
                output += QString("Z");
            }
        }
    }
    return output;
}

QString SvgOutputDev::convertMatrix(const QMatrix &matrix)
{
    return QString("matrix(%1 %2 %3 %4 %5 %6)")
           .arg(matrix.m11()).arg(matrix.m12())
           .arg(matrix.m21()).arg(matrix.m22())
           .arg(matrix.dx()) .arg(matrix.dy());
}

QString SvgOutputDev::convertMatrix(const double * matrix)
{
    return QString("matrix(%1 %2 %3 %4 %5 %6)")
           .arg(matrix[0]).arg(matrix[1])
           .arg(matrix[2]).arg(matrix[3])
           .arg(matrix[4]) .arg(matrix[5]);
}

void SvgOutputDev::updateAll(GfxState *state)
{
    debugPdf << "update complete state";

    //updateLineDash(state);
    updateLineJoin(state);
    updateLineCap(state);
    updateLineWidth(state);
    //updateFlatness(state);
    updateMiterLimit(state);
    updateFillColor(state);
    updateStrokeColor(state);
    updateFillOpacity(state);
    updateStrokeOpacity(state);
}

void SvgOutputDev::updateFillColor(GfxState *state)
{
    GfxRGB rgb;
    state->getFillRGB(&rgb);

    QColor brushColour = d->brush.color();
    brushColour.setRgbF(colToDbl(rgb.r), colToDbl(rgb.g), colToDbl(rgb.b), brushColour.alphaF());
    d->brush.setColor(brushColour);

    debugPdf << "update fill color" << brushColour;
}

void SvgOutputDev::updateStrokeColor(GfxState *state)
{
    GfxRGB rgb;
    state->getStrokeRGB(&rgb);

    QColor penColour = d->pen.color();
    penColour.setRgbF(colToDbl(rgb.r), colToDbl(rgb.g), colToDbl(rgb.b), penColour.alphaF());
    d->pen.setColor(penColour);

    debugPdf << "update stroke color" << penColour;
}

void SvgOutputDev::updateFillOpacity(GfxState *state)
{
    QColor brushColour = d->brush.color();
    brushColour.setAlphaF(state->getFillOpacity());
    d->brush.setColor(brushColour);

    debugPdf << "update fill opacity" << state->getFillOpacity();
}

void SvgOutputDev::updateStrokeOpacity(GfxState *state)
{
    QColor penColour = d->pen.color();
    penColour.setAlphaF(state->getStrokeOpacity());
    d->pen.setColor(penColour);

    debugPdf << "update stroke opacity" << state->getStrokeOpacity();
}

void SvgOutputDev::updateLineJoin(GfxState *state)
{
    switch (state->getLineJoin()) {
    case 0:
        d->pen.setJoinStyle(Qt::MiterJoin);
        break;
    case 1:
        d->pen.setJoinStyle(Qt::RoundJoin);
        break;
    case 2:
        d->pen.setJoinStyle(Qt::BevelJoin);
        break;
    }
}

void SvgOutputDev::updateLineCap(GfxState *state)
{
    switch (state->getLineCap()) {
    case 0:
        d->pen.setCapStyle(Qt::FlatCap);
        break;
    case 1:
        d->pen.setCapStyle(Qt::RoundCap);
        break;
    case 2:
        d->pen.setCapStyle(Qt::SquareCap);
        break;
    }
}

void SvgOutputDev::updateMiterLimit(GfxState *state)
{
    d->pen.setMiterLimit(state->getMiterLimit());
}

void SvgOutputDev::updateLineWidth(GfxState *state)
{
    //d->pen.setWidthF( state->getTransformedLineWidth() );
    d->pen.setWidthF(state->getLineWidth());
}

QString SvgOutputDev::printFill()
{
    QString fill;
    fill += " fill=\"";

    switch (d->brush.style()) {
    case Qt::NoBrush:
        fill += "none";
        break;
    case Qt::SolidPattern:
        fill += d->brush.color().name();
        break;
    default:
        debugPdf << "unhandled fill style (" << d->brush.style() << ")";
        return QString();
        break;
    }

    fill += "\"";
    if (d->brush.color().alphaF() < 1.0)
        fill += QString(" fill-opacity=\"%1\"").arg(d->brush.color().alphaF());

    return fill;
}

QString SvgOutputDev::printStroke()
{
    QString stroke;
    stroke += " stroke=\"";
    if (d->pen.style() == Qt::NoPen)
        stroke += "none";
    else
        stroke += d->pen.color().name();
    stroke += "\"";

    if (d->pen.color().alphaF() < 1.0)
        stroke += QString(" stroke-opacity=\"%1\"").arg(d->pen.color().alphaF());
    stroke += QString(" stroke-width=\"%1\"").arg(d->pen.widthF());

    if (d->pen.capStyle() == Qt::FlatCap)
        stroke += " stroke-linecap=\"butt\"";
    else if (d->pen.capStyle() == Qt::RoundCap)
        stroke += " stroke-linecap=\"round\"";
    else if (d->pen.capStyle() == Qt::SquareCap)
        stroke += " stroke-linecap=\"square\"";

    if (d->pen.joinStyle() == Qt::MiterJoin) {
        stroke += " stroke-linejoin=\"miter\"";
        stroke += QString(" stroke-miterlimit=\"%1\"").arg(d->pen.miterLimit());
    } else if (d->pen.joinStyle() == Qt::RoundJoin)
        stroke += " stroke-linejoin=\"round\"";
    else if (d->pen.joinStyle() == Qt::BevelJoin)
        stroke += " stroke-linejoin=\"bevel\"";

    // dash
    if (d->pen.style() > Qt::SolidLine) {
        //*stream << " stroke-dashoffset=\"" << line->dashPattern().offset() << "\"";
        stroke += " stroke-dasharray=\" ";

        foreach(qreal dash, d->pen.dashPattern()) {
            stroke += dash + ' ';
        }
        stroke += "\"";
    }

    return stroke;
}

#ifdef HAVE_POPPLER_PRE_0_64
void SvgOutputDev::drawString(GfxState * state, GooString * s)
#else
void SvgOutputDev::drawString(GfxState * state, const GooString * s)
#endif
{
    int render = state->getRender();
    // check for invisible text -- this is used by Acrobat Capture
    if (render == 3)
        return;

    // ignore empty strings
    if (s->getLength() == 0)
        return;

    GfxFont * font = state->getFont();

    QString str;

#ifdef HAVE_POPPLER_PRE_0_64
    char * p = s->getCString();
#else
    #ifdef HAVE_POPPLER_PRE_0_72
        const char * p = s->getCString();
    #else
        const char * p = s->c_str();
    #endif
#endif
    int len = s->getLength();
    CharCode code;
#ifdef HAVE_POPPLER_PRE_0_82
    Unicode *u = nullptr;
#else
    const Unicode *u = nullptr;
#endif
    int uLen;
    double dx, dy, originX, originY;
    while (len > 0) {
        int n = font->getNextChar(p, len, &code, &u, &uLen, &dx, &dy, &originX, &originY);
        p += n;
        len -= n;
        if (!u)
            break;
        str += QChar(*u);
    }

    str = str.simplified();
    if (str.isEmpty())
        return;

    // escape special characters
    str.replace('&', "&amp;");
    str.replace('<', "&lt;");
    str.replace('>', "&gt;");

    double x = state->getCurX();
    double y = state->getCurY();

    const double * ctm = state->getCTM();
    QMatrix transform(ctm[0], ctm[1], ctm[2], ctm[3], ctm[4], ctm[5]);

    QMatrix mirror;
    mirror.translate(x, y);
    mirror.scale(1.0, -1.0);
    mirror.translate(-x, -y);

    transform = mirror * transform;
    bool writeTransform = true;
    if (transform.m11() == 1.0 && transform.m12() == 0.0 &&
            transform.m21() == 0.0 && transform.m22() == 1.0) {
        writeTransform = false;
        x += transform.dx();
        y += transform.dy();
    }

    *d->body << "<text";
    *d->body << " x=\"" << x << "px\"";
    *d->body << " y=\"" << y << "px\"";

    if (font && font->getFamily()) {
#ifdef HAVE_POPPLER_PRE_0_72
        *d->body << " font-family=\"" << QString::fromLatin1(font->getFamily()->getCString()) << "\"";
#else
        *d->body << " font-family=\"" << QString::fromLatin1(font->getFamily()->c_str()) << "\"";
#endif
        //debugPdf << "font family:" << QString::fromLatin1( font->getFamily()->getCString() );
    } else if (font && font->getName()) {
#ifdef HAVE_POPPLER_PRE_0_72
        *d->body << " font-family=\"" << QString::fromLatin1(font->getName()->getCString()) << "\"";
#else
        *d->body << " font-family=\"" << QString::fromLatin1(font->getName()->c_str()) << "\"";
#endif
        //debugPdf << "font name:" << QString::fromLatin1( font->getName()->getCString() );
    }
    *d->body << " font-size=\"" << qMax(state->getFontSize(), state->getTransformedFontSize()) << "px\"";

    if (writeTransform)
        *d->body << " transform=\"" << convertMatrix(transform) << "\"";

    // fill
    if (!(render & 1))
        *d->body << printFill();
    // stroke
    if ((render & 3) == 1 || (render & 3) == 2)
        *d->body << printStroke();

    *d->body << ">";
    *d->body << str;
    *d->body << "</text>" << endl;
}

#ifdef HAVE_POPPLER_PRE_0_82
void SvgOutputDev::drawImage(GfxState *state, Object *ref, Stream *str,
                             int width, int height, GfxImageColorMap *colorMap,
                             bool interpolate, int *maskColors, bool inlineImg)
#else
void SvgOutputDev::drawImage(GfxState *state, Object *ref, Stream *str,
                             int width, int height, GfxImageColorMap *colorMap,
                             bool interpolate, const int *maskColors, bool inlineImg)
#endif
{
    Q_UNUSED(interpolate)
    Q_UNUSED(inlineImg)
    ImageStream * imgStr = new ImageStream(str, width, colorMap->getNumPixelComps(), colorMap->getBits());
    imgStr->reset();

    unsigned int *dest = 0;
    unsigned char * buffer = new unsigned char[width * height * 4];

    QImage * image = 0;
    if (maskColors) {
        for (int y = 0; y < height; y++) {
            dest = (unsigned int *)(buffer + y * 4 * width);
            unsigned char * pix = imgStr->getLine();
            colorMap->getRGBLine(pix, dest, width);

            for (int x = 0; x < width; x++) {
                for (int i = 0; i < colorMap->getNumPixelComps(); ++i) {
                    if (pix[i] < maskColors[2*i] * 255 || pix[i] > maskColors[2*i+1] * 255) {
                        *dest = *dest | 0xff000000;
                        break;
                    }
                }
                pix += colorMap->getNumPixelComps();
                dest++;
            }
        }

        image = new QImage(buffer, width, height, QImage::Format_ARGB32);
    } else {
        for (int y = 0; y < height; y++) {
            dest = (unsigned int *)(buffer + y * 4 * width);
            unsigned char * pix = imgStr->getLine();
            colorMap->getRGBLine(pix, dest, width);
        }

        image = new QImage(buffer, width, height, QImage::Format_RGB32);
    }

    if (image == nullptr || image->isNull()) {
        debugPdf << "Null image";
        delete imgStr;
        delete[] buffer;
        delete image;
        return;
    }

    const double * ctm = state->getCTM();
    QMatrix m;
    m.setMatrix(ctm[0] / width, ctm[1] / width, -ctm[2] / height, -ctm[3] / height, ctm[2] + ctm[4], ctm[3] + ctm[5]);

    QByteArray ba;
    QBuffer device(&ba);
    device.open(QIODevice::WriteOnly);
    if (image->save(&device, "PNG")) {
        *d->body << "<image";
        *d->body << " transform=\"" << convertMatrix(m) << "\"";
        *d->body << " width=\"" << width << "px\"";
        *d->body << " height=\"" << height << "px\"";
        *d->body << " xlink:href=\"data:image/png;base64," << ba.toBase64() <<  "\"";
        *d->body << " />" << endl;
    }

    delete image;
    delete[] buffer;
    delete imgStr;
}

