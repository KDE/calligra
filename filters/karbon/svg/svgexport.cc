/* This file is part of the KDE project
   Copyright (C) 2002 Lars Siebold <khandha5@gmx.net>
   Copyright (C) 2002-2003,2005 Rob Buis <buis@kde.org>
   Copyright (C) 2002,2005-2006 David Faure <faure@kde.org>
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2004 Nicolas Goutte <nicolasg@snafu.de>
   Copyright (C) 2005 Boudewijn Rempt <boud@valdyas.org>
   Copyright (C) 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>
   Copyright (C) 2005,2007-2008 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2006 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>
   Copyright (C) 2006 Gábor Lehel <illissius@gmail.com>
   Copyright (C) 2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2006 Christian Mueller <cmueller@gmx.de>
   Copyright (C) 2006 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2010 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "svgexport.h"
#include "SvgUtil.h"

#include <KarbonDocument.h>
#include <KarbonPart.h>

#include <KoDocument.h>
#include <KoFilterChain.h>
#include <KoShapeLayer.h>
#include <KoPathShape.h>
#include <KoLineBorder.h>
#include <KoColorBackground.h>
#include <KoGradientBackground.h>
#include <KoPatternBackground.h>
#include <plugins/artistictextshape/ArtisticTextShape.h>
#include <pathshapes/rectangle/RectangleShape.h>
#include <pathshapes/ellipse/EllipseShape.h>
#include <KoImageData.h>
#include "KoFilterEffect.h"
#include "KoFilterEffectStack.h"
#include "KoXmlWriter.h"

#include <KGenericFactory>
#include <KMimeType>
#include <KTemporaryFile>
#include <KIO/NetAccess>
#include <KIO/CopyJob>

#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QBuffer>
#include <QtCore/QFileInfo>
#include <QtGui/QLinearGradient>
#include <QtGui/QRadialGradient>

static void printIndentation(QTextStream *stream, unsigned int indent)
{
    static const QString INDENT("  ");
    for (unsigned int i = 0; i < indent;++i)
        *stream << INDENT;
}

typedef KGenericFactory<SvgExport> SvgExportFactory;
K_EXPORT_COMPONENT_FACTORY(libkarbonsvgexport, SvgExportFactory("kofficefilters"))

SvgExport::SvgExport(QObject*parent, const QStringList&)
        : KoFilter(parent), m_indent(0), m_indent2(0)
{
    double scaleToUserSpace = SvgUtil::toUserSpace(1.0);
    m_userSpaceMatrix.scale(scaleToUserSpace, scaleToUserSpace);
}

KoFilter::ConversionStatus SvgExport::convert(const QByteArray& from, const QByteArray& to)
{
    if (to != "image/svg+xml" || from != "application/vnd.oasis.opendocument.graphics")
        return KoFilter::NotImplemented;

    KoDocument * document = m_chain->inputDocument();
    if (! document)
        return KoFilter::ParsingError;

    KarbonPart * karbonPart = dynamic_cast<KarbonPart*>(document);
    if (! karbonPart)
        return KoFilter::WrongFormat;

    QFile fileOut(m_chain->outputFile());
    if (!fileOut.open(QIODevice::WriteOnly))
        return KoFilter::StupidError;

    m_stream = new QTextStream(&fileOut);
    QString body;
    m_body = new QTextStream(&body, QIODevice::ReadWrite);
    QString defs;
    m_defs = new QTextStream(&defs, QIODevice::ReadWrite);

    saveDocument(karbonPart->document());

    *m_stream << defs;
    *m_stream << body;

    fileOut.close();

    delete m_stream;
    delete m_defs;
    delete m_body;

    return KoFilter::OK;
}

void SvgExport::saveDocument(KarbonDocument& document)
{
    // get the bounding box of the page
    QSizeF pageSize = document.pageSize();

    // standard header:
    *m_defs <<
    "<?xml version=\"1.0\" standalone=\"no\"?>\n" <<
    "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\" " <<
    "\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">"
    << endl;

    // add some PR.  one line is more than enough.
    *m_defs <<
    "<!-- Created using Karbon14, part of koffice: http://www.koffice.org/karbon -->" << endl;

    *m_defs <<
    "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" width=\"" <<
    pageSize.width() << "pt\" height=\"" << pageSize.height() << "pt\">" << endl;
    printIndentation(m_defs, ++m_indent2);
    *m_defs << "<defs>" << endl;

    m_indent++;
    m_indent2++;

    // export layers:
    foreach(KoShapeLayer * layer, document.layers()) {
        saveLayer(layer);
    }

    // end tag:
    printIndentation(m_defs, --m_indent2);
    *m_defs << "</defs>" << endl;
    *m_body << "</svg>" << endl;
}

void SvgExport::saveLayer(KoShapeLayer * layer)
{
    printIndentation(m_body, m_indent++);
    *m_body << "<g" << getID(layer) << ">" << endl;

    QList<KoShape*> sortedShapes = layer->shapes();
    qSort(sortedShapes.begin(), sortedShapes.end(), KoShape::compareShapeZIndex);

    foreach(KoShape * shape, sortedShapes) {
        KoShapeContainer * container = dynamic_cast<KoShapeContainer*>(shape);
        if (container)
            saveGroup(container);
        else
            saveShape(shape);
    }

    printIndentation(m_body, --m_indent);
    *m_body << "</g>" << endl;
}

void SvgExport::saveGroup(KoShapeContainer * group)
{
    printIndentation(m_body, m_indent++);
    *m_body << "<g" << getID(group);
    *m_body << getTransform(group->transformation(), " transform");
    getStyle(group, m_body);
    *m_body << ">" << endl;

    QList<KoShape*> sortedShapes = group->shapes();
    qSort(sortedShapes.begin(), sortedShapes.end(), KoShape::compareShapeZIndex);

    foreach(KoShape * shape, sortedShapes) {
        KoShapeContainer * container = dynamic_cast<KoShapeContainer*>(shape);
        if (container)
            saveGroup(container);
        else
            saveShape(shape);
    }

    printIndentation(m_body, --m_indent);
    *m_body << "</g>" << endl;
}

void SvgExport::saveShape(KoShape * shape)
{
    KoPathShape * path = dynamic_cast<KoPathShape*>(shape);
    if (path) {
        KoParameterShape * parameterShape = dynamic_cast<KoParameterShape*>(path);
        bool isParametric = parameterShape && parameterShape->isParametricShape();
        if (path->pathShapeId() == RectangleShapeId && isParametric) {
            saveRectangle(static_cast<RectangleShape*>(path));
        } else if (path->pathShapeId() == EllipseShapeId && isParametric) {
            saveEllipse(static_cast<EllipseShape*>(path));
        } else {
            savePath(path);
        }
    } else {
        if (shape->shapeId() == ArtisticTextShapeID) {
            saveText(static_cast<ArtisticTextShape*>(shape));
        } else if (shape->shapeId() == "PictureShape") {
            saveImage(shape);
        }
    }
}

void SvgExport::savePath(KoPathShape * path)
{
    printIndentation(m_body, m_indent);
    *m_body << "<path" << getID(path);

    getStyle(path, m_body);

    *m_body << " d=\"" << path->toString(m_userSpaceMatrix) << "\" ";
    *m_body << getTransform(path->transformation(), " transform");

    *m_body << " />" << endl;
}

void SvgExport::saveEllipse(EllipseShape * ellipse)
{
    if (ellipse->type() == EllipseShape::Arc && ellipse->startAngle() == ellipse->endAngle()) {
        printIndentation(m_body, m_indent);
        QSizeF size = ellipse->size();
        if (size.width() == size.height()) {
            *m_body << "<circle" << getID(ellipse);
            *m_body << " r=\"" << 0.5 * size.width() << "pt\"";
        } else {
            *m_body << "<ellipse" << getID(ellipse);
            *m_body << " rx=\"" << 0.5 * size.width() << "pt\"";
            *m_body << " ry=\"" << 0.5 * size.height() << "pt\"";
        }
        *m_body << " cx=\"" << 0.5 * size.width() << "pt\"";
        *m_body << " cy=\"" << 0.5 * size.height() << "pt\"";
        *m_body << getTransform(ellipse->transformation(), " transform");
        getStyle(ellipse, m_body);
        *m_body << "/>" << endl;
    } else {
        savePath(ellipse);
    }
}

void SvgExport::saveRectangle(RectangleShape * rectangle)
{
    printIndentation(m_body, m_indent);
    *m_body << "<rect" << getID(rectangle);
    *m_body << getTransform(rectangle->transformation(), " transform");

    getStyle(rectangle, m_body);

    QSizeF size = rectangle->size();
    *m_body << " width=\"" << size.width() << "pt\"";
    *m_body << " height=\"" << size.height() << "pt\"";

    double rx = rectangle->cornerRadiusX();
    if (rx > 0.0)
        *m_body << " rx=\"" << 0.01 * rx * 0.5 * size.width() << "pt\"";
    double ry = rectangle->cornerRadiusY();
    if (ry > 0.0)
        *m_body << " ry=\"" << 0.01 * ry * 0.5 * size.height() << "pt\"";

    *m_body << "/>" << endl;
}

static QString createUID()
{
    static unsigned int nr = 0;

    return "defitem" + QString().setNum(nr++);
}

QString SvgExport::createID(const KoShape * obj)
{
    QString id;
    if (! m_shapeIds.contains(obj)) {
        id = obj->name().isEmpty() ? createUID() : obj->name();
        m_shapeIds.insert(obj, id);
    } else {
        id = m_shapeIds[obj];
    }
    return id;
}

QString SvgExport::getID(const KoShape *obj)
{
    return QString(" id=\"%1\"").arg(createID(obj));
}

QString SvgExport::getTransform(const QTransform &matrix, const QString &attributeName)
{
    if (matrix.isIdentity())
        return "";

    QString transform = attributeName + "=\"";
    if (isTranslation(matrix)) {
        transform += QString("translate(%1, %2)")
                     .arg(SvgUtil::toUserSpace(matrix.dx()))
                     .arg(SvgUtil::toUserSpace(matrix.dy()));
    } else {
        transform += QString("matrix(%1 %2 %3 %4 %5 %6)")
                     .arg(matrix.m11()).arg(matrix.m12())
                     .arg(matrix.m21()).arg(matrix.m22())
                     .arg(SvgUtil::toUserSpace(matrix.dx()))
                     .arg(SvgUtil::toUserSpace(matrix.dy()));
    }

    return transform + "\"";
}

bool SvgExport::isTranslation(const QTransform &m)
{
    return (m.m11() == 1.0 && m.m12() == 0.0 && m.m21() == 0.0 && m.m22() == 1.0);
}

void SvgExport::getColorStops(const QGradientStops & colorStops)
{
    m_indent2++;
    foreach(const QGradientStop &stop, colorStops) {
        printIndentation(m_defs, m_indent2);
        *m_defs << "<stop stop-color=\"" << stop.second.name();
        *m_defs << "\" offset=\"" << QString().setNum(stop.first);
        *m_defs << "\" stop-opacity=\"" << stop.second.alphaF() << "\"" << " />" << endl;
    }
    m_indent2--;
}

void SvgExport::getGradient(const QGradient * gradient, const QTransform &gradientTransform)
{
    if (! gradient)
        return;

    Q_ASSERT(gradient->coordinateMode() == QGradient::ObjectBoundingMode);

    const QString spreadMethod[3] = {
        QString("spreadMethod=\"pad\" "),
        QString("spreadMethod=\"reflect\" "),
        QString("spreadMethod=\"repeat\" ")
    };

    QString uid = createUID();
    if (gradient->type() == QGradient::LinearGradient) {
        const QLinearGradient * g = static_cast<const QLinearGradient*>(gradient);
        // do linear grad
        printIndentation(m_defs, m_indent2);
        *m_defs << "<linearGradient id=\"" << uid << "\" ";
        *m_defs << getTransform(gradientTransform, "gradientTransform") << " ";
        *m_defs << "gradientUnits=\"objectBoundingBox\" ";
        *m_defs << "x1=\"" << g->start().x() << "\" ";
        *m_defs << "y1=\"" << g->start().y() << "\" ";
        *m_defs << "x2=\"" << g->finalStop().x() << "\" ";
        *m_defs << "y2=\"" << g->finalStop().y() << "\" ";
        *m_defs << spreadMethod[g->spread()];
        *m_defs << ">" << endl;

        // color stops
        getColorStops(gradient->stops());

        printIndentation(m_defs, m_indent2);
        *m_defs << "</linearGradient>" << endl;
        *m_body << "url(#" << uid << ")";
    } else if (gradient->type() == QGradient::RadialGradient) {
        const QRadialGradient * g = static_cast<const QRadialGradient*>(gradient);
        // do radial grad
        printIndentation(m_defs, m_indent2);
        *m_defs << "<radialGradient id=\"" << uid << "\" ";
        *m_defs << getTransform(gradientTransform, "gradientTransform") << " ";
        *m_defs << "gradientUnits=\"objectBoundingBox\" ";
        *m_defs << "cx=\"" << g->center().x() << "\" ";
        *m_defs << "cy=\"" << g->center().y() << "\" ";
        *m_defs << "fx=\"" << g->focalPoint().x() << "\" ";
        *m_defs << "fy=\"" << g->focalPoint().y() << "\" ";
        *m_defs << "r=\"" << QString().setNum(g->radius()) << "\" ";
        *m_defs << spreadMethod[g->spread()];
        *m_defs << ">" << endl;

        // color stops
        getColorStops(gradient->stops());

        printIndentation(m_defs, m_indent2);
        *m_defs << "</radialGradient>" << endl;
        *m_body << "url(#" << uid << ")";
    } else if (gradient->type() == QGradient::ConicalGradient) {
        //const QConicalGradient * g = static_cast<const QConicalGradient*>( gradient );
        // fake conical grad as radial.
        // fugly but better than data loss.
        /*
        printIndentation( m_defs, m_indent2 );
        *m_defs << "<radialGradient id=\"" << uid << "\" ";
        *m_defs << "gradientUnits=\"userSpaceOnUse\" ";
        *m_defs << "cx=\"" << g->center().x() << "\" ";
        *m_defs << "cy=\"" << g->center().y() << "\" ";
        *m_defs << "fx=\"" << grad.focalPoint().x() << "\" ";
        *m_defs << "fy=\"" << grad.focalPoint().y() << "\" ";
        double r = sqrt( pow( grad.vector().x() - grad.origin().x(), 2 ) + pow( grad.vector().y() - grad.origin().y(), 2 ) );
        *m_defs << "r=\"" << QString().setNum( r ) << "\" ";
        *m_defs << spreadMethod[g->spread()];
        *m_defs << ">" << endl;

        // color stops
        getColorStops( gradient->stops() );

        printIndentation( m_defs, m_indent2 );
        *m_defs << "</radialGradient>" << endl;
        *m_body << "url(#" << uid << ")";
        */
    }
}

// better than nothing
void SvgExport::getPattern(KoPatternBackground * pattern, KoShape * shape)
{
    QString uid = createUID();

    QSizeF shapeSize = shape->size();
    QSizeF patternSize = pattern->patternDisplaySize();
    QSize imageSize = pattern->pattern().size();

    // calculate offset in point
    QPointF offset = pattern->referencePointOffset();
    offset.rx() = 0.01 * offset.x() * patternSize.width();
    offset.ry() = 0.01 * offset.y() * patternSize.height();

    // now take the reference point into account
    switch (pattern->referencePoint()) {
    case KoPatternBackground::TopLeft:
        break;
    case KoPatternBackground::Top:
        offset += QPointF(0.5 * shapeSize.width(), 0.0);
        break;
    case KoPatternBackground::TopRight:
        offset += QPointF(shapeSize.width(), 0.0);
        break;
    case KoPatternBackground::Left:
        offset += QPointF(0.0, 0.5 * shapeSize.height());
        break;
    case KoPatternBackground::Center:
        offset += QPointF(0.5 * shapeSize.width(), 0.5 * shapeSize.height());
        break;
    case KoPatternBackground::Right:
        offset += QPointF(shapeSize.width(), 0.5 * shapeSize.height());
        break;
    case KoPatternBackground::BottomLeft:
        offset += QPointF(0.0, shapeSize.height());
        break;
    case KoPatternBackground::Bottom:
        offset += QPointF(0.5 * shapeSize.width(), shapeSize.height());
        break;
    case KoPatternBackground::BottomRight:
        offset += QPointF(shapeSize.width(), shapeSize.height());
        break;
    }

    offset = shape->absoluteTransformation(0).map(offset);

    printIndentation(m_defs, m_indent2);
    *m_defs << "<pattern id=\"" << uid << "\"";
    *m_defs << " x=\"" << SvgUtil::toUserSpace(offset.x()) << "\"";
    *m_defs << " y=\"" << SvgUtil::toUserSpace(offset.y()) << "\"";

    if (pattern->repeat() == KoPatternBackground::Stretched) {
        *m_defs << " width=\"100%\"";
        *m_defs << " height=\"100%\"";
        *m_defs << " patternUnits=\"objectBoundingBox\"";
    } else {
        *m_defs << " width=\"" << SvgUtil::toUserSpace(patternSize.width()) << "\"";
        *m_defs << " height=\"" << SvgUtil::toUserSpace(patternSize.height()) << "\"";
        *m_defs << " patternUnits=\"userSpaceOnUse\"";
    }

    *m_defs << " viewBox=\"0 0 " << imageSize.width() << " " << imageSize.height() << "\"";
    //*m_defs << " patternContentUnits=\"userSpaceOnUse\"";
    *m_defs << ">" << endl;

    printIndentation(m_defs, m_indent2++);

    *m_defs << "<image";

    *m_defs << " x=\"0\"";
    *m_defs << " y=\"0\"";
    *m_defs << " width=\"" << imageSize.width() << "px\"";
    *m_defs << " height=\"" << imageSize.height() << "px\"";

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    if (pattern->pattern().save(&buffer, "PNG")) {
        const QString mimeType(KMimeType::findByContent(ba)->name());
        *m_defs << " xlink:href=\"data:" << mimeType << ";base64," << ba.toBase64() <<  "\"";
    }

    *m_defs << "/>" << endl;

    printIndentation(m_defs, --m_indent2);
    *m_defs << "</pattern>" << endl;

    *m_body << "url(#" << uid << ")";
}

void SvgExport::getStyle(KoShape * shape, QTextStream * stream)
{
    getFill(shape, stream);
    getStroke(shape, stream);
    getEffects(shape, stream);
    if (! shape->isVisible())
        *stream << " display=\"none\"";
    if (shape->transparency() > 0.0)
        *stream << " opacity=\"" << 1.0 - shape->transparency() << "\"";
}

void SvgExport::getFill(KoShape * shape, QTextStream *stream)
{
    if (! shape->background()) {
        *stream << " fill=\"none\"";
    }

    QBrush fill(Qt::NoBrush);
    KoColorBackground * cbg = dynamic_cast<KoColorBackground*>(shape->background());
    if (cbg) {
        *stream << " fill=\"" << cbg->color().name() << "\"";
        if (cbg->color().alphaF() < 1.0)
            *stream << " fill-opacity=\"" << cbg->color().alphaF() << "\"";
    }
    KoGradientBackground * gbg = dynamic_cast<KoGradientBackground*>(shape->background());
    if (gbg) {
        *stream << " fill=\"";
        getGradient(gbg->gradient(), gbg->transform());
        *stream << "\"";
    }
    KoPatternBackground * pbg = dynamic_cast<KoPatternBackground*>(shape->background());
    if (pbg) {
        *stream << " fill=\"";
        getPattern(pbg, shape);
        *stream << "\"";
    }

    KoPathShape * path = dynamic_cast<KoPathShape*>(shape);
    if (path && shape->background()) {
        // non-zero is default, so only write fillrule if evenodd is set
        if (path->fillRule() == Qt::OddEvenFill)
            *stream << " fill-rule=\"evenodd\"";
    }
}

void SvgExport::getStroke(KoShape *shape, QTextStream *stream)
{
    const KoLineBorder * line = dynamic_cast<const KoLineBorder*>(shape->border());
    if (! line)
        return;

    *stream << " stroke=\"";
    if (line->lineStyle() == Qt::NoPen)
        *stream << "none";
    else if (line->lineBrush().gradient())
        getGradient(line->lineBrush().gradient(), line->lineBrush().transform());
    else
        *stream << line->color().name();
    *stream << "\"";

    if (line->color().alphaF() < 1.0)
        *stream << " stroke-opacity=\"" << line->color().alphaF() << "\"";
    *stream << " stroke-width=\"" << SvgUtil::toUserSpace(line->lineWidth()) << "\"";

    if (line->capStyle() == Qt::FlatCap)
        *stream << " stroke-linecap=\"butt\"";
    else if (line->capStyle() == Qt::RoundCap)
        *stream << " stroke-linecap=\"round\"";
    else if (line->capStyle() == Qt::SquareCap)
        *stream << " stroke-linecap=\"square\"";

    if (line->joinStyle() == Qt::MiterJoin) {
        *stream << " stroke-linejoin=\"miter\"";
        *stream << " stroke-miterlimit=\"" << line->miterLimit() << "\"";
    } else if (line->joinStyle() == Qt::RoundJoin)
        *stream << " stroke-linejoin=\"round\"";
    else if (line->joinStyle() == Qt::BevelJoin)
        *stream << " stroke-linejoin=\"bevel\"";

    // dash
    if (line->lineStyle() > Qt::SolidLine) {
        qreal dashFactor = line->lineWidth();

        if (line->dashOffset() != 0)
            *stream << " stroke-dashoffset=\"" << dashFactor * line->dashOffset() << "\"";
        *stream << " stroke-dasharray=\" ";

        const QVector<qreal> dashes = line->lineDashes();
        int dashCount = dashes.size();
        for (int i = 0; i < dashCount; ++i) {
            if (i > 0)
                *stream << ",";
            *stream << dashes[i] * dashFactor;
        }
        *stream << "\"";
    }
}

void SvgExport::getEffects(KoShape *shape, QTextStream *stream)
{
    KoFilterEffectStack * filterStack = shape->filterEffectStack();
    if (!filterStack)
        return;

    QList<KoFilterEffect*> filterEffects = filterStack->filterEffects();
    if (!filterEffects.count())
        return;

    QString uid = createUID();

    printIndentation(m_defs, m_indent2);

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter writer(&buffer, m_indent2*2);

    filterStack->save(writer, uid);

    *m_defs << ba;
    *m_defs << endl;

    *stream << " filter=\"url(#" << uid << ")\"";
}

void SvgExport::saveText(ArtisticTextShape * text)
{
    printIndentation(m_body, m_indent++);
    *m_body << "<text" << getID(text);

    getStyle(text, m_body);

    QFont font = text->font();

    *m_body << " font-family=\"" << font.family() << "\"";
    *m_body << " font-size=\"" << font.pointSize() << "pt\"";

    if (font.bold())
        *m_body << " font-weight=\"bold\"";
    if (font.italic())
        *m_body << " font-style=\"italic\"";

    qreal anchorOffset = 0.0;
    if (text->textAnchor() == ArtisticTextShape::AnchorMiddle) {
        anchorOffset += 0.5 * text->size().width();
        *m_body << " text-anchor=\"middle\"";
    } else if (text->textAnchor() == ArtisticTextShape::AnchorEnd) {
        anchorOffset += text->size().width();
        *m_body << " text-anchor=\"end\"";
    }

    printIndentation(m_body, m_indent);

    // check if we are set on a path
    if (text->layout() == ArtisticTextShape::Straight) {
        QTransform m = text->transformation();
        if (isTranslation(m)) {
            QPointF position = text->position();
            *m_body << " x=\"" << position.x() + anchorOffset << "pt\"";
            *m_body << " y=\"" << position.y() + text->baselineOffset() << "pt\"";
        } else {
            *m_body << " x=\"" << anchorOffset << "pt\"";
            *m_body << " y=\"" << text->baselineOffset() << "pt\"";
            *m_body << getTransform(text->transformation(), " transform");
        }
        *m_body << ">" << endl;
        *m_body << text->text();
    } else {
        KoPathShape * baseline = KoPathShape::createShapeFromPainterPath(text->baseline());

        QString id;

        printIndentation(m_defs, m_indent);

        id = createUID();
        *m_defs << "<path id=\"" << id << "\"";
        *m_defs << " d=\"" << baseline->toString(baseline->absoluteTransformation(0) * m_userSpaceMatrix) << "\" ";
        *m_defs << " />" << endl;

        *m_body << ">" << endl;
        *m_body << "<textPath xlink:href=\"#" << id << "\"";
        if (text->startOffset() > 0.0)
            *m_body << " startOffset=\"" << text->startOffset() * 100.0 << "%\"";
        *m_body << ">";
        *m_body << text->text();
        *m_body << "</textPath>" << endl;

        delete baseline;
    }

    printIndentation(m_body, --m_indent);
    *m_body << "</text>" << endl;
}

void SvgExport::saveImage(KoShape *picture)
{
    KoImageData *imageData = qobject_cast<KoImageData*>(picture->userData());
    if (! imageData) {
        qWarning() << "Picture has no image data. Omitting.";
        return;
    }

    printIndentation(m_body, m_indent);

    *m_body << "<image" << getID(picture);
    QTransform m = picture->transformation();
    if (isTranslation(m)) {
        QPointF position = picture->position();
        *m_body << " x=\"" << position.x() << "pt\"";
        *m_body << " y=\"" << position.y() << "pt\"";
    } else {
        *m_body << getTransform(picture->transformation(), " transform");
    }

    *m_body << " width=\"" << picture->size().width() << "pt\"";
    *m_body << " height=\"" << picture->size().height() << "pt\"";

    const bool saveInline = true;

    if (saveInline) {
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        if (imageData->saveData(buffer)) {
            const QString mimeType(KMimeType::findByContent(ba)->name());
            *m_body << " xlink:href=\"data:" << mimeType << ";base64," << ba.toBase64() <<  "\"";
        }
    } else {
        // write to a temp file first
        KTemporaryFile imgFile;
        if (imageData->saveData(imgFile)) {
            // tz: TODO the new version of KoImageData has the extension save inside maybe that can be used
            // get the mime type from the temp file content
            KMimeType::Ptr mimeType = KMimeType::findByFileContent(imgFile.fileName());
            // get url of destination directory
            KUrl url(m_chain->outputFile());
            QString dstBaseFilename = QFileInfo(url.fileName()).baseName();
            url.setDirectory(url.directory());
            // create a filename for the image file at the destination directory
            QString fname = dstBaseFilename + '_' + createID(picture);
            // get extension from mimetype
            QString ext = "";
            QStringList patterns = mimeType->patterns();
            if (patterns.count())
                ext = patterns.first().mid(1);
            url.setFileName(fname + ext);
            // check if file exists already
            int i = 0;
            // change filename as long as the filename already exists
            while (KIO::NetAccess::exists(url, KIO::NetAccess::DestinationSide, 0))
                url.setFileName(fname + QString("_%1").arg(++i) + ext);
            // move the temp file to the destination directory
            KIO::Job * job = KIO::move(KUrl(imgFile.fileName()), url);
            if (job && KIO::NetAccess::synchronousRun(job, 0))
                *m_body << " xlink:href=\"" << url.fileName() << "\"";
            else
                KIO::NetAccess::removeTempFile(imgFile.fileName());
        }
    }
    *m_body << " />" << endl;
}

#include "svgexport.moc"

