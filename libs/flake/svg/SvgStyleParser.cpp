/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2002-2005, 2007 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2002-2004 Nicolas Goutte <nicolasg@snafu.de>
 * SPDX-FileCopyrightText: 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * SPDX-FileCopyrightText: 2005-2009 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2005, 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006-2007 Inge Wallin <inge@lysator.liu.se>
 * SPDX-FileCopyrightText: 2007-2008, 2010 Thorsten Zachmann <zachmann@kde.org>

 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SvgStyleParser.h"
#include "SvgGraphicContext.h"
#include "SvgLoadingContext.h"
#include "SvgUtil.h"

#include <QColor>
#include <QGradientStops>
#include <QStringList>

class Q_DECL_HIDDEN SvgStyleParser::Private
{
public:
    Private(SvgLoadingContext &loadingContext)
        : context(loadingContext)
    {
        // the order of the font attributes is important, don't change without reason !!!
        fontAttributes << "font-family"
                       << "font-size"
                       << "font-weight";
        fontAttributes << "text-decoration"
                       << "letter-spacing"
                       << "word-spacing"
                       << "baseline-shift";
        // the order of the style attributes is important, don't change without reason !!!
        styleAttributes << "color"
                        << "display";
        styleAttributes << "fill"
                        << "fill-rule"
                        << "fill-opacity";
        styleAttributes << "stroke"
                        << "stroke-width"
                        << "stroke-linejoin"
                        << "stroke-linecap";
        styleAttributes << "stroke-dasharray"
                        << "stroke-dashoffset"
                        << "stroke-opacity"
                        << "stroke-miterlimit";
        styleAttributes << "opacity"
                        << "filter"
                        << "clip-path"
                        << "clip-rule";
    }

    SvgLoadingContext &context;
    QStringList fontAttributes; ///< font related attributes
    QStringList styleAttributes; ///< style related attributes
};

SvgStyleParser::SvgStyleParser(SvgLoadingContext &context)
    : d(new Private(context))
{
}

SvgStyleParser::~SvgStyleParser()
{
    delete d;
}

void SvgStyleParser::parseStyle(const SvgStyles &styles)
{
    SvgGraphicsContext *gc = d->context.currentGC();
    if (!gc)
        return;

    // make sure we parse the style attributes in the right order
    foreach (const QString &command, d->styleAttributes) {
        const QString &params = styles.value(command);
        if (params.isEmpty())
            continue;
        parsePA(gc, command, params);
    }
}

void SvgStyleParser::parseFont(const SvgStyles &styles)
{
    SvgGraphicsContext *gc = d->context.currentGC();
    if (!gc)
        return;

    // make sure to only parse font attributes here
    foreach (const QString &command, d->fontAttributes) {
        const QString &params = styles.value(command);
        if (params.isEmpty())
            continue;
        parsePA(gc, command, params);
    }
}

void SvgStyleParser::parsePA(SvgGraphicsContext *gc, const QString &command, const QString &params)
{
    QColor fillcolor = gc->fillColor;
    QColor strokecolor = gc->stroke.color();

    if (params == "inherit")
        return;

    if (command == "fill") {
        if (params == "none") {
            gc->fillType = SvgGraphicsContext::None;
        } else if (params.startsWith(QLatin1String("url("))) {
            unsigned int start = params.indexOf('#') + 1;
            unsigned int end = params.indexOf(')', start);
            gc->fillId = params.mid(start, end - start);
            gc->fillType = SvgGraphicsContext::Complex;
            // check if there is a fallback color
            parseColor(fillcolor, params.mid(end + 1).trimmed());
        } else {
            // great we have a solid fill
            gc->fillType = SvgGraphicsContext::Solid;
            parseColor(fillcolor, params);
        }
    } else if (command == "fill-rule") {
        if (params == "nonzero")
            gc->fillRule = Qt::WindingFill;
        else if (params == "evenodd")
            gc->fillRule = Qt::OddEvenFill;
    } else if (command == "stroke") {
        if (params == "none") {
            gc->strokeType = SvgGraphicsContext::None;
        } else if (params.startsWith(QLatin1String("url("))) {
            unsigned int start = params.indexOf('#') + 1;
            unsigned int end = params.indexOf(')', start);
            gc->strokeId = params.mid(start, end - start);
            gc->strokeType = SvgGraphicsContext::Complex;
            // check if there is a fallback color
            parseColor(strokecolor, params.mid(end + 1).trimmed());
        } else {
            // great we have a solid stroke
            gc->strokeType = SvgGraphicsContext::Solid;
            parseColor(strokecolor, params);
        }
    } else if (command == "stroke-width") {
        gc->stroke.setLineWidth(SvgUtil::parseUnitXY(gc, params));
    } else if (command == "stroke-linejoin") {
        if (params == "miter")
            gc->stroke.setJoinStyle(Qt::MiterJoin);
        else if (params == "round")
            gc->stroke.setJoinStyle(Qt::RoundJoin);
        else if (params == "bevel")
            gc->stroke.setJoinStyle(Qt::BevelJoin);
    } else if (command == "stroke-linecap") {
        if (params == "butt")
            gc->stroke.setCapStyle(Qt::FlatCap);
        else if (params == "round")
            gc->stroke.setCapStyle(Qt::RoundCap);
        else if (params == "square")
            gc->stroke.setCapStyle(Qt::SquareCap);
    } else if (command == "stroke-miterlimit") {
        gc->stroke.setMiterLimit(params.toFloat());
    } else if (command == "stroke-dasharray") {
        QVector<qreal> array;
        if (params != "none") {
            QString dashString = params;
            const QStringList dashes = dashString.replace(',', ' ').simplified().split(' ');
            for (QStringList::ConstIterator it = dashes.begin(); it != dashes.end(); ++it)
                array.append((*it).toFloat());
        }
        gc->stroke.setLineStyle(Qt::CustomDashLine, array);
    } else if (command == "stroke-dashoffset") {
        gc->stroke.setDashOffset(params.toFloat());
    }
    // handle opacity
    else if (command == "stroke-opacity")
        strokecolor.setAlphaF(SvgUtil::fromPercentage(params));
    else if (command == "fill-opacity") {
        float opacity = SvgUtil::fromPercentage(params);
        if (opacity < 0.0)
            opacity = 0.0;
        if (opacity > 1.0)
            opacity = 1.0;
        fillcolor.setAlphaF(opacity);
    } else if (command == "opacity") {
        gc->opacity = SvgUtil::fromPercentage(params);
    } else if (command == "font-family") {
        QString family = params;
        family.replace('\'', ' ');
        gc->font.setFamily(family);
    } else if (command == "font-size") {
        float pointSize = SvgUtil::parseUnitY(gc, params);
        if (pointSize > 0.0f)
            gc->font.setPointSizeF(pointSize);
    } else if (command == "font-weight") {
        QFont::Weight weight = QFont::Normal;

        // map svg weight to qt weight
        // svg value        qt value
        // 100,200,300      1, 17, 33
        // 400              50          (normal)
        // 500,600          58,66
        // 700              75          (bold)
        // 800,900          87,99

        if (params == "bold")
            weight = QFont::Bold;
        else if (params == "lighter" || params == "bolder") {
            weight = gc->font.weight();
        } else {
            bool ok;
            // try to read numerical weight value
            int weightInt = params.toInt(&ok, 10);

            if (!ok)
                return;

            weight = static_cast<QFont::Weight>(weightInt);
        }
        gc->font.setWeight(weight);
    } else if (command == "text-decoration") {
        if (params == "line-through")
            gc->font.setStrikeOut(true);
        else if (params == "underline")
            gc->font.setUnderline(true);
    } else if (command == "letter-spacing") {
        gc->letterSpacing = SvgUtil::parseUnitX(gc, params);
    } else if (command == "baseline-shift") {
        gc->baselineShift = params;
    } else if (command == "word-spacing") {
        gc->wordSpacing = SvgUtil::parseUnitX(gc, params);
    } else if (command == "color") {
        QColor color;
        parseColor(color, params);
        gc->currentColor = color;
    } else if (command == "display") {
        if (params == "none")
            gc->display = false;
    } else if (command == "filter") {
        if (params != "none" && params.startsWith("url(")) {
            unsigned int start = params.indexOf('#') + 1;
            unsigned int end = params.indexOf(')', start);
            gc->filterId = params.mid(start, end - start);
        }
    } else if (command == "clip-path") {
        if (params != "none" && params.startsWith("url(")) {
            unsigned int start = params.indexOf('#') + 1;
            unsigned int end = params.indexOf(')', start);
            gc->clipPathId = params.mid(start, end - start);
        }
    } else if (command == "clip-rule") {
        if (params == "nonzero")
            gc->clipRule = Qt::WindingFill;
        else if (params == "evenodd")
            gc->clipRule = Qt::OddEvenFill;
    }

    gc->fillColor = fillcolor;
    gc->stroke.setColor(strokecolor);
}

bool SvgStyleParser::parseColor(QColor &color, const QString &s)
{
    if (s.isEmpty() || s == "none")
        return false;

    if (s.startsWith(QLatin1String("rgb("))) {
        QString parse = s.trimmed();
        QStringList colors = parse.split(',');
        QString r = colors[0].right((colors[0].length() - 4));
        QString g = colors[1];
        QString b = colors[2].left((colors[2].length() - 1));

        if (r.contains('%')) {
            r = r.left(r.length() - 1);
            r = QString::number(int((double(255 * r.toDouble()) / 100.0)));
        }

        if (g.contains('%')) {
            g = g.left(g.length() - 1);
            g = QString::number(int((double(255 * g.toDouble()) / 100.0)));
        }

        if (b.contains('%')) {
            b = b.left(b.length() - 1);
            b = QString::number(int((double(255 * b.toDouble()) / 100.0)));
        }

        color = QColor(r.toInt(), g.toInt(), b.toInt());
    } else if (s == "currentColor") {
        color = d->context.currentGC()->currentColor;
    } else {
        // QColor understands #RRGGBB and svg color names
        color = QColor::fromString(s.trimmed());
    }

    return true;
}

void SvgStyleParser::parseColorStops(QGradient *gradient, const KoXmlElement &e)
{
    QGradientStops stops;
    QColor c;

    for (KoXmlNode n = e.firstChild(); !n.isNull(); n = n.nextSibling()) {
        KoXmlElement stop = n.toElement();
        if (stop.tagName() == "stop") {
            float offset;
            QString temp = stop.attribute("offset");
            if (temp.contains('%')) {
                temp = temp.left(temp.length() - 1);
                offset = temp.toFloat() / 100.0;
            } else
                offset = temp.toFloat();

            QString stopColorStr = stop.attribute("stop-color");
            if (!stopColorStr.isEmpty()) {
                if (stopColorStr == "inherit") {
                    stopColorStr = inheritedAttribute("stop-color", stop);
                }
                parseColor(c, stopColorStr);
            } else {
                // try style attr
                QString style = stop.attribute("style").simplified();
                const QStringList substyles = style.split(';', Qt::SkipEmptyParts);
                for (QStringList::ConstIterator it = substyles.begin(); it != substyles.end(); ++it) {
                    QStringList substyle = it->split(':');
                    QString command = substyle[0].trimmed();
                    QString params = substyle[1].trimmed();
                    if (command == "stop-color")
                        parseColor(c, params);
                    if (command == "stop-opacity")
                        c.setAlphaF(params.toDouble());
                }
            }
            QString opacityStr = stop.attribute("stop-opacity");
            if (!opacityStr.isEmpty()) {
                if (opacityStr == "inherit") {
                    opacityStr = inheritedAttribute("stop-opacity", stop);
                }
                c.setAlphaF(opacityStr.toDouble());
            }
            stops.append(QPair<qreal, QColor>(offset, c));
        }
    }
    if (stops.count())
        gradient->setStops(stops);
}

SvgStyles SvgStyleParser::collectStyles(const KoXmlElement &e)
{
    SvgStyles styleMap;

    // collect individual presentation style attributes which have the priority 0
    foreach (const QString &command, d->styleAttributes) {
        const QString attribute = e.attribute(command);
        if (!attribute.isEmpty())
            styleMap[command] = attribute;
    }
    foreach (const QString &command, d->fontAttributes) {
        const QString attribute = e.attribute(command);
        if (!attribute.isEmpty())
            styleMap[command] = attribute;
    }

    // match css style rules to element
    QStringList cssStyles = d->context.matchingStyles(e);

    // collect all css style attributes
    foreach (const QString &style, cssStyles) {
        const QStringList substyles = style.split(';', Qt::SkipEmptyParts);
        if (!substyles.count())
            continue;
        for (QStringList::ConstIterator it = substyles.begin(); it != substyles.end(); ++it) {
            QStringList substyle = it->split(':');
            if (substyle.count() != 2)
                continue;
            QString command = substyle[0].trimmed();
            QString params = substyle[1].trimmed();
            // only use style and font attributes
            if (d->styleAttributes.contains(command) || d->fontAttributes.contains(command))
                styleMap[command] = params;
        }
    }

    // replace keyword "inherit" for style values
    QMutableMapIterator<QString, QString> it(styleMap);
    while (it.hasNext()) {
        it.next();
        if (it.value() == QLatin1StringView("inherit")) {
            it.setValue(inheritedAttribute(it.key(), e));
        }
    }

    return styleMap;
}

SvgStyles SvgStyleParser::mergeStyles(const SvgStyles &referencedBy, const SvgStyles &referencedStyles)
{
    // 1. use all styles of the referencing styles
    SvgStyles mergedStyles = referencedBy;
    // 2. use all styles of the referenced style which are not in the referencing styles
    SvgStyles::const_iterator it = referencedStyles.constBegin();
    for (; it != referencedStyles.constEnd(); ++it) {
        if (!referencedBy.contains(it.key())) {
            mergedStyles.insert(it.key(), it.value());
        }
    }
    return mergedStyles;
}

SvgStyles SvgStyleParser::mergeStyles(const KoXmlElement &e1, const KoXmlElement &e2)
{
    return mergeStyles(collectStyles(e1), collectStyles(e2));
}

QString SvgStyleParser::inheritedAttribute(const QString &attributeName, const KoXmlElement &e)
{
    KoXmlNode parent = e.parentNode();
    while (!parent.isNull()) {
        KoXmlElement currentElement = parent.toElement();
        if (currentElement.hasAttribute(attributeName)) {
            return currentElement.attribute(attributeName);
        }
        parent = currentElement.parentNode();
    }
    return QString();
}
