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

#include "SvgWriter.h"
#include "SvgUtil.h"
#include "SvgSavingContext.h"
#include "SvgSerializable.h"

#include <KoShapeLayer.h>
#include <KoShapeGroup.h>
#include <KoPathShape.h>
#include <KoLineBorder.h>
#include <KoColorBackground.h>
#include <KoGradientBackground.h>
#include <KoPatternBackground.h>
#include <plugins/artistictextshape/ArtisticTextShape.h>
#include <plugins/artistictextshape/ArtisticTextRange.h>
#include <KoImageData.h>
#include <KoFilterEffect.h>
#include <KoFilterEffectStack.h>
#include <KoXmlWriter.h>
#include <KoClipPath.h>
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

SvgWriter::SvgWriter(const QList<KoShapeLayer*> &layers, const QSizeF &pageSize)
    : m_pageSize(pageSize)
    , m_writeInlineImages(true)
{
    const qreal scaleToUserSpace = SvgUtil::toUserSpace(1.0);
    m_userSpaceMatrix.scale(scaleToUserSpace, scaleToUserSpace);

    foreach(KoShapeLayer *layer, layers)
        m_toplevelShapes.append(layer);
}

SvgWriter::SvgWriter(const QList<KoShape*> &toplevelShapes, const QSizeF &pageSize)
    : m_toplevelShapes(toplevelShapes)
    , m_pageSize(pageSize)
    , m_writeInlineImages(true)
{
    const qreal scaleToUserSpace = SvgUtil::toUserSpace(1.0);
    m_userSpaceMatrix.scale(scaleToUserSpace, scaleToUserSpace);
}

SvgWriter::~SvgWriter()
{

}

bool SvgWriter::save(const QString &filename, bool writeInlineImages)
{
    QFile fileOut(filename);
    if (!fileOut.open(QIODevice::WriteOnly))
        return false;

    m_filename = filename;
    m_writeInlineImages = writeInlineImages;

    const bool success = save(fileOut);

    m_writeInlineImages = true;
    m_filename.clear();

    fileOut.close();

    return success;
}

bool SvgWriter::save(QIODevice &outputDevice)
{
    QTextStream svgStream(&outputDevice);

    // standard header:
    svgStream << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl;
    svgStream << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\" ";
    svgStream << "\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">" << endl;

    // add some PR.  one line is more than enough.
    svgStream << "<!-- Created using Karbon, part of Calligra: http://www.calligra-suite.org/karbon -->" << endl;

    svgStream << "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\"";
    svgStream << " width=\"" << m_pageSize.width() << "pt\"";
    svgStream << " height=\"" << m_pageSize.height() << "pt\">" << endl;

    {
        SvgSavingContext savingContext(outputDevice);

        // top level shapes
        foreach(KoShape *shape, m_toplevelShapes) {
            KoShapeLayer *layer = dynamic_cast<KoShapeLayer*>(shape);
            if(layer) {
                saveLayer(layer, savingContext);
            } else {
                KoShapeGroup *group = dynamic_cast<KoShapeGroup*>(shape);
                if (group)
                    saveGroup(group, savingContext);
                else
                    saveShape(shape, savingContext);
            }
        }
    }

    // end tag:
    svgStream << endl << "</svg>" << endl;

    return true;
}

void SvgWriter::saveLayer(KoShapeLayer *layer, SvgSavingContext &context)
{
    context.shapeWriter().startElement("g");
    context.shapeWriter().addAttribute("id", context.getID(layer));

    QList<KoShape*> sortedShapes = layer->shapes();
    qSort(sortedShapes.begin(), sortedShapes.end(), KoShape::compareShapeZIndex);

    foreach(KoShape * shape, sortedShapes) {
        KoShapeGroup * group = dynamic_cast<KoShapeGroup*>(shape);
        if (group)
            saveGroup(group, context);
        else
            saveShape(shape, context);
    }

    context.shapeWriter().endElement();
}

void SvgWriter::saveGroup(KoShapeGroup * group, SvgSavingContext &context)
{
    context.shapeWriter().startElement("g");
    context.shapeWriter().addAttribute("id", context.getID(group));
    context.shapeWriter().addAttribute("transform", SvgUtil::transformToString(group->transformation()));

    saveStyle(group, context);

    QList<KoShape*> sortedShapes = group->shapes();
    qSort(sortedShapes.begin(), sortedShapes.end(), KoShape::compareShapeZIndex);

    foreach(KoShape * shape, sortedShapes) {
        KoShapeGroup * childGroup = dynamic_cast<KoShapeGroup*>(shape);
        if (childGroup)
            saveGroup(childGroup, context);
        else
            saveShape(shape, context);
    }

    context.shapeWriter().endElement();
}

void SvgWriter::saveShape(KoShape *shape, SvgSavingContext &context)
{
    SvgSerializable *svgShape = dynamic_cast<SvgSerializable*>(shape);
    if (svgShape) {
        if (svgShape->saveSvg(context))
            return;
    }

    // TODO: implement generic saving of shape via a switch element

    /*
    KoPathShape * path = dynamic_cast<KoPathShape*>(shape);
    if (path) {
        savePath(path, context);
    } else {
        if (shape->shapeId() == ArtisticTextShapeID) {
            saveText(static_cast<ArtisticTextShape*>(shape), context);
        } else if (shape->shapeId() == "PictureShape") {
            saveImage(shape, context);
        }
    }
    */
}

void SvgWriter::savePath(KoPathShape *path, SvgSavingContext &context)
{
    context.shapeWriter().startElement("path");
    context.shapeWriter().addAttribute("id", context.getID(path));
    context.shapeWriter().addAttribute("transform", SvgUtil::transformToString(path->transformation()));

    saveStyle(path, context);

    context.shapeWriter().addAttribute("d", path->toString(m_userSpaceMatrix));
    context.shapeWriter().endElement();
}

void SvgWriter::saveColorStops(const QGradientStops &colorStops, SvgSavingContext &context)
{
    foreach(const QGradientStop &stop, colorStops) {
        context.styleWriter().startElement("stop");
        context.styleWriter().addAttribute("stop-color", stop.second.name());
        context.styleWriter().addAttribute("offset", stop.first);
        context.styleWriter().addAttribute("stop-opacity", stop.second.alphaF());
        context.styleWriter().endElement();
    }
}

QString SvgWriter::saveGradient(const QGradient *gradient, const QTransform &gradientTransform, SvgSavingContext &context)
{
    if (! gradient)
        return QString();

    Q_ASSERT(gradient->coordinateMode() == QGradient::ObjectBoundingMode);

    const QString spreadMethod[3] = {
        QString("pad"),
        QString("reflect"),
        QString("repeat")
    };

    const QString uid = context.createUID("gradient");

    if (gradient->type() == QGradient::LinearGradient) {
        const QLinearGradient * g = static_cast<const QLinearGradient*>(gradient);
        context.styleWriter().startElement("linearGradient");
        context.styleWriter().addAttribute("id", uid);
        context.styleWriter().addAttribute("gradientTransform", SvgUtil::transformToString(gradientTransform));
        context.styleWriter().addAttribute("gradientUnits", "objectBoundingBox");
        context.styleWriter().addAttribute("x1", g->start().x());
        context.styleWriter().addAttribute("y1", g->start().y());
        context.styleWriter().addAttribute("x2", g->finalStop().x());
        context.styleWriter().addAttribute("y2", g->finalStop().y());
        context.styleWriter().addAttribute("spreadMethod", spreadMethod[g->spread()]);
        // color stops
        saveColorStops(gradient->stops(), context);
        context.styleWriter().endElement();
    } else if (gradient->type() == QGradient::RadialGradient) {
        const QRadialGradient * g = static_cast<const QRadialGradient*>(gradient);
        context.styleWriter().startElement("radialGradient");
        context.styleWriter().addAttribute("id", uid);
        context.styleWriter().addAttribute("gradientTransform", SvgUtil::transformToString(gradientTransform));
        context.styleWriter().addAttribute("gradientUnits", "objectBoundingBox");
        context.styleWriter().addAttribute("cx", g->center().x());
        context.styleWriter().addAttribute("cy", g->center().y());
        context.styleWriter().addAttribute("fx", g->focalPoint().x());
        context.styleWriter().addAttribute("fy", g->focalPoint().y());
        context.styleWriter().addAttribute("r", g->radius());
        context.styleWriter().addAttribute("spreadMethod", spreadMethod[g->spread()]);
        // color stops
        saveColorStops(gradient->stops(), context);
        context.styleWriter().endElement();
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

    return uid;
}

// better than nothing
QString SvgWriter::savePattern(KoPatternBackground *pattern, KoShape *shape, SvgSavingContext &context)
{
    const QString uid = context.createUID("pattern");

    const QSizeF shapeSize = shape->size();
    const QSizeF patternSize = pattern->patternDisplaySize();
    const QSize imageSize = pattern->pattern().size();

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

    context.styleWriter().startElement("pattern");
    context.styleWriter().addAttribute("id", uid);
    context.styleWriter().addAttribute("x", SvgUtil::toUserSpace(offset.x()));
    context.styleWriter().addAttribute("y", SvgUtil::toUserSpace(offset.y()));

    if (pattern->repeat() == KoPatternBackground::Stretched) {
        context.styleWriter().addAttribute("width", "100%");
        context.styleWriter().addAttribute("height", "100%");
        context.styleWriter().addAttribute("patternUnits", "objectBoundingBox");
    } else {
        context.styleWriter().addAttribute("width", SvgUtil::toUserSpace(patternSize.width()));
        context.styleWriter().addAttribute("height", SvgUtil::toUserSpace(patternSize.height()));
        context.styleWriter().addAttribute("patternUnits", "userSpaceOnUse");
    }

    context.styleWriter().addAttribute("viewBox", QString("0 0 %1 %2").arg(imageSize.width()).arg(imageSize.height()));
    //*m_defs << " patternContentUnits=\"userSpaceOnUse\"";

    context.styleWriter().startElement("image");
    context.styleWriter().addAttribute("x", "0");
    context.styleWriter().addAttribute("y", "0");
    context.styleWriter().addAttribute("width", QString("%1px").arg(imageSize.width()));
    context.styleWriter().addAttribute("height", QString("%1px").arg(imageSize.height()));

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    if (pattern->pattern().save(&buffer, "PNG")) {
        const QString mimeType(KMimeType::findByContent(ba)->name());
        context.styleWriter().addAttribute("xlink:href", "data:"+ mimeType + ";base64," + ba.toBase64());
    }

    context.styleWriter().endElement(); // image
    context.styleWriter().endDocument(); // pattern

    return uid;
}

void SvgWriter::saveStyle(KoShape *shape, SvgSavingContext &context)
{
    saveFill(shape, context);
    saveStroke(shape, context);
    saveEffects(shape, context);
    saveClipping(shape, context);
    if (! shape->isVisible())
        context.shapeWriter().addAttribute("display", "none");
    if (shape->transparency() > 0.0)
        context.shapeWriter().addAttribute("opacity", 1.0 - shape->transparency());
}

void SvgWriter::saveFill(KoShape * shape, SvgSavingContext &context)
{
    if (! shape->background()) {
        context.shapeWriter().addAttribute("fill", "none");
    }

    QBrush fill(Qt::NoBrush);
    KoColorBackground * cbg = dynamic_cast<KoColorBackground*>(shape->background());
    if (cbg) {
        context.shapeWriter().addAttribute("fill", cbg->color().name());
        if (cbg->color().alphaF() < 1.0)
            context.shapeWriter().addAttribute("fill-opacity", cbg->color().alphaF());
    }
    KoGradientBackground * gbg = dynamic_cast<KoGradientBackground*>(shape->background());
    if (gbg) {
        QString gradientId = saveGradient(gbg->gradient(), gbg->transform(), context);
        context.shapeWriter().addAttribute("fill", "url(#" + gradientId + ")");
    }
    KoPatternBackground * pbg = dynamic_cast<KoPatternBackground*>(shape->background());
    if (pbg) {
        const QString patternId = savePattern(pbg, shape, context);
        context.shapeWriter().addAttribute("fill", "url(#" + patternId + ")");
    }

    KoPathShape * path = dynamic_cast<KoPathShape*>(shape);
    if (path && shape->background()) {
        // non-zero is default, so only write fillrule if evenodd is set
        if (path->fillRule() == Qt::OddEvenFill)
            context.shapeWriter().addAttribute("fill-rule", "evenodd");
    }
}

void SvgWriter::saveStroke(KoShape *shape, SvgSavingContext &context)
{
    const KoLineBorder * line = dynamic_cast<const KoLineBorder*>(shape->border());
    if (! line)
        return;

    QString strokeStr("none");
    if (line->lineBrush().gradient()) {
        QString gradientId = saveGradient(line->lineBrush().gradient(), line->lineBrush().transform(), context);
        strokeStr = "url(#" + gradientId + ")";
    } else {
        strokeStr = line->color().name();
    }
    if (!strokeStr.isEmpty())
        context.shapeWriter().addAttribute("stroke", strokeStr);

    if (line->color().alphaF() < 1.0)
        context.shapeWriter().addAttribute("stroke-opacity", line->color().alphaF());
    context.shapeWriter().addAttribute("stroke-width", SvgUtil::toUserSpace(line->lineWidth()));

    if (line->capStyle() == Qt::FlatCap)
        context.shapeWriter().addAttribute("stroke-linecap", "butt");
    else if (line->capStyle() == Qt::RoundCap)
        context.shapeWriter().addAttribute("stroke-linecap", "round");
    else if (line->capStyle() == Qt::SquareCap)
        context.shapeWriter().addAttribute("stroke-linecap", "square");

    if (line->joinStyle() == Qt::MiterJoin) {
        context.shapeWriter().addAttribute("stroke-linejoin", "miter");
        context.shapeWriter().addAttribute("stroke-miterlimit", line->miterLimit());
    } else if (line->joinStyle() == Qt::RoundJoin)
        context.shapeWriter().addAttribute("stroke-linejoin", "round");
    else if (line->joinStyle() == Qt::BevelJoin)
        context.shapeWriter().addAttribute("stroke-linejoin", "bevel");

    // dash
    if (line->lineStyle() > Qt::SolidLine) {
        qreal dashFactor = line->lineWidth();

        if (line->dashOffset() != 0)
            context.shapeWriter().addAttribute("stroke-dashoffset", dashFactor * line->dashOffset());

        QString dashStr;
        const QVector<qreal> dashes = line->lineDashes();
        int dashCount = dashes.size();
        for (int i = 0; i < dashCount; ++i) {
            if (i > 0)
                dashStr += ",";
            dashStr += QString("%1").arg(dashes[i] * dashFactor);
        }
        context.shapeWriter().addAttribute("stroke-dasharray", dashStr);
    }
}

void SvgWriter::saveEffects(KoShape *shape, SvgSavingContext &context)
{
    KoFilterEffectStack * filterStack = shape->filterEffectStack();
    if (!filterStack)
        return;

    QList<KoFilterEffect*> filterEffects = filterStack->filterEffects();
    if (!filterEffects.count())
        return;

    const QString uid = context.createUID("filter");

    filterStack->save(context.styleWriter(), uid);

    context.shapeWriter().addAttribute("filter", "url(#" + uid + ")");
}

void SvgWriter::saveClipping(KoShape *shape, SvgSavingContext &context)
{
    KoClipPath *clipPath = shape->clipPath();
    if (!clipPath)
        return;

    const QSizeF shapeSize = shape->outlineRect().size();
    KoPathShape *path = KoPathShape::createShapeFromPainterPath(clipPath->pathForSize(shapeSize));
    if (!path)
        return;

    path->close();

    const QString uid = context.createUID("clippath");

    context.styleWriter().startElement("clipPath");
    context.styleWriter().addAttribute("id", uid);
    context.styleWriter().addAttribute("clipPathUnits", "userSpaceOnUse");

    context.styleWriter().startElement("path");
    context.styleWriter().addAttribute("d", path->toString(path->absoluteTransformation(0)*m_userSpaceMatrix));
    context.styleWriter().endElement(); // path

    context.styleWriter().endElement(); // clipPath

    context.shapeWriter().addAttribute("clip-path", "url(#" + uid + ")");
    if (clipPath->clipRule() != Qt::WindingFill)
        context.shapeWriter().addAttribute("clip-rule", "evenodd");
}

void SvgWriter::saveFont(const QFont &font, SvgSavingContext &context)
{
    context.shapeWriter().addAttribute("font-family", font.family());
    context.shapeWriter().addAttributePt("font-size", font.pointSizeF());

    if (font.bold())
        context.shapeWriter().addAttribute("font-weight", "bold");
    if (font.italic())
        context.shapeWriter().addAttribute("font-style", "italic");
}

void SvgWriter::saveTextRange(const ArtisticTextRange &range, SvgSavingContext &context, bool saveRangeFont, qreal baselineOffset)
{
    /*
    context.shapeWriter().startElement("tspan");
    if (range.hasXOffsets()) {
        const char *attributeName = (range.xOffsetType() == ArtisticTextRange::AbsoluteOffset ? "x" : "dx");
        QString attributeValue;
        int charIndex = 0;
        while(range.hasXOffset(charIndex)) {
            if (charIndex)
                attributeValue += ",";
            attributeValue += QString("%1").arg(SvgUtil::toUserSpace(range.xOffset(charIndex++)));
        }
        context.shapeWriter().addAttribute(attributeName, attributeValue);
    }
    if (range.hasYOffsets()) {
        if (range.yOffsetType() != ArtisticTextRange::AbsoluteOffset)
            baselineOffset = 0;
        const char *attributeName = (range.yOffsetType() == ArtisticTextRange::AbsoluteOffset ? " y" : " dy");
        QString attributeValue;
        int charIndex = 0;
        while(range.hasYOffset(charIndex)) {
            if (charIndex)
                attributeValue += ",";
            attributeValue += QString("%1").arg(SvgUtil::toUserSpace(baselineOffset+range.yOffset(charIndex++)));
        }
        context.shapeWriter().addAttribute(attributeName, attributeValue);
    }
    if (range.hasRotations()) {
        QString attributeValue;
        int charIndex = 0;
        while(range.hasRotation(charIndex)) {
            if (charIndex)
                attributeValue += ",";
            attributeValue += QString("%1").arg(range.rotation(charIndex++));
        }
        context.shapeWriter().addAttribute("rotate", attributeValue);
    }
    if (range.baselineShift() != ArtisticTextRange::None) {
        switch(range.baselineShift()) {
        case ArtisticTextRange::Sub:
            context.shapeWriter().addAttribute("baseline-shift", "sub");
            break;
        case ArtisticTextRange::Super:
            context.shapeWriter().addAttribute("baseline-shift", "super");
            break;
        case ArtisticTextRange::Percent:
            context.shapeWriter().addAttribute("baseline-shift", QString("%1%").arg(range.baselineShiftValue()*100));
            break;
        case ArtisticTextRange::Length:
            context.shapeWriter().addAttribute("baseline-shift", QString("%1%").arg(SvgUtil::toUserSpace(range.baselineShiftValue())));
            break;
        default:
            break;
        }
    }
    if (saveRangeFont)
        saveFont(range.font(), context);
    context.shapeWriter().addTextNode(range.text());
    context.shapeWriter().endElement();
    */
}

void SvgWriter::saveText(ArtisticTextShape *text, SvgSavingContext &context)
{
    /*
    context.shapeWriter().startElement("text");
    context.shapeWriter().addAttribute("id", context.getID(text));

    saveStyle(text, context);

    const QList<ArtisticTextRange> formattedText = text->text();

    // if we have only a single text range, save the font on the text element
    const bool hasSingleRange = formattedText.size() == 1;
    if (hasSingleRange) {
        saveFont(formattedText.first().font(), context);
    }

    qreal anchorOffset = 0.0;
    if (text->textAnchor() == ArtisticTextShape::AnchorMiddle) {
        anchorOffset += 0.5 * text->size().width();
        context.shapeWriter().addAttribute("text-anchor", "middle");
    } else if (text->textAnchor() == ArtisticTextShape::AnchorEnd) {
        anchorOffset += text->size().width();
        context.shapeWriter().addAttribute("text-anchor", "end");
    }

    // check if we are set on a path
    if (text->layout() == ArtisticTextShape::Straight) {
        QTransform m = text->transformation();
        if (m.type() == QTransform::TxTranslate) {
            QPointF position = text->position();
            context.shapeWriter().addAttributePt("x", position.x() + anchorOffset);
            context.shapeWriter().addAttributePt("y", position.y() + text->baselineOffset());
        } else {
            context.shapeWriter().addAttributePt("x", anchorOffset);
            context.shapeWriter().addAttributePt("y", text->baselineOffset());
            context.shapeWriter().addAttribute("transform", SvgUtil::transformToString(text->transformation()));
        }
        foreach(const ArtisticTextRange &range, formattedText) {
            saveTextRange(range, context, !hasSingleRange, text->baselineOffset());
        }
    } else {
        KoPathShape * baseline = KoPathShape::createShapeFromPainterPath(text->baseline());

        QString id = context.createUID("baseline");
        context.styleWriter().startElement("path");
        context.styleWriter().addAttribute("id", id);
        context.styleWriter().addAttribute("d", baseline->toString(baseline->absoluteTransformation(0) * m_userSpaceMatrix));
        context.styleWriter().endElement();

        context.shapeWriter().startElement("textPath");
        context.shapeWriter().addAttribute("xlink:href", "#"+id);
        if (text->startOffset() > 0.0)
            context.shapeWriter().addAttribute("startOffset", QString("%1%").arg(text->startOffset() * 100.0));
        foreach(const ArtisticTextRange &range, formattedText) {
            saveTextRange(range, context, !hasSingleRange, text->baselineOffset());
        }
        context.shapeWriter().endElement();

        delete baseline;
    }

    context.shapeWriter().endElement();
    */
}

void SvgWriter::saveImage(KoShape *picture, SvgSavingContext &context)
{
    /*
    KoImageData *imageData = qobject_cast<KoImageData*>(picture->userData());
    if (! imageData) {
        qWarning() << "Picture has no image data. Omitting.";
        return;
    }

    context.shapeWriter().startElement("image");
    context.shapeWriter().addAttribute("id", context.getID(picture));

    QTransform m = picture->transformation();
    if (m.type() == QTransform::TxTranslate) {
        const QPointF position = picture->position();
        context.shapeWriter().addAttributePt("x", position.x());
        context.shapeWriter().addAttributePt("y", position.y());
    } else {
        context.shapeWriter().addAttribute("transform", SvgUtil::transformToString(picture->transformation()));
    }

    context.shapeWriter().addAttributePt("width", picture->size().width());
    context.shapeWriter().addAttributePt("height", picture->size().height());

    if (m_writeInlineImages) {
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        if (imageData->saveData(buffer)) {
            const QString mimeType(KMimeType::findByContent(ba)->name());
            const QString header("data:" + mimeType + ";base64,");
            context.shapeWriter().addAttribute("xlink:href", header + ba.toBase64());
        }
    } else {
        // write to a temp file first
        KTemporaryFile imgFile;
        if (imageData->saveData(imgFile)) {
            // tz: TODO the new version of KoImageData has the extension save inside maybe that can be used
            // get the mime type from the temp file content
            KMimeType::Ptr mimeType = KMimeType::findByFileContent(imgFile.fileName());
            // get url of destination directory
            KUrl url(m_filename);
            QString dstBaseFilename = QFileInfo(url.fileName()).baseName();
            url.setDirectory(url.directory());
            // create a filename for the image file at the destination directory
            QString fname = dstBaseFilename + '_' + context.createUID("picture");
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
                context.shapeWriter().addAttribute("xlink:href", url.fileName());
            else
                KIO::NetAccess::removeTempFile(imgFile.fileName());
        }
    }
    context.shapeWriter().endElement();
    */
}
