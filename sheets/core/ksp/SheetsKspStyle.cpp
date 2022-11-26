/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2021 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "SheetsKsp.h"
#include "SheetsKspPrivate.h"

#include "Style.h"
#include "StyleManager.h"

#include <KoXmlReader.h>

#include <QFont>

namespace Calligra {
namespace Sheets {

namespace Ksp {
    QPen toPen(KoXmlElement & element);
    QFont toFont(KoXmlElement & element);
    QDomElement createElement(const QString & tagName, const QFont & font, QDomDocument & doc);
    QDomElement createElement(const QString & tagname, const QPen & pen, QDomDocument & doc);
}

QDomElement Ksp::saveStyles(StyleManager *manager, QDomDocument & doc)
{
    QDomElement styles = doc.createElement("styles");

    CustomStyle *def = manager->defaultStyle();
    saveCustomStyle (def, doc, styles, manager);

    QStringList styleNames = manager->styleNames(false);
    foreach (QString name, styleNames)
    {
        CustomStyle *style = manager->style (name);
        saveCustomStyle (style, doc, styles, manager);
    }

    return styles;
}

bool Ksp::loadStyles (StyleManager *manager, KoXmlElement const & styles)
{
    bool ok = true;
    CustomStyle *def = manager->defaultStyle();

    KoXmlElement e = styles.firstChild().toElement();
    while (!e.isNull()) {
        QString name;
        if (e.hasAttribute("name"))
            name = e.attribute("name");
        Style::StyleType type = (Style::StyleType)(e.attribute("type").toInt(&ok));
        if (!ok)
            return false;

        if (name == "Default" && type == Style::BUILTIN) {
            if (!loadCustomStyle (def, e, name))
                return false;
            def->setType(Style::BUILTIN);
        } else if (!name.isNull()) {
            CustomStyle* style = 0;
            if (e.hasAttribute("parent") && e.attribute("parent") == "Default")
                style = new CustomStyle(name, def);
            else
                style = new CustomStyle(name);

            if (!loadCustomStyle(style, e, name)) {
                delete style;
                return false;
            }

            if (style->type() == Style::AUTO)
                style->setType(Style::CUSTOM);
            manager->insertStyle(style);
            debugSheetsODF << "Style" << name << ":" << style;
        }

        e = e.nextSibling().toElement();
    }

    // reparent all styles
    const QStringList names = manager->styleNames();
    QStringList::ConstIterator it;
    for (it = names.begin(); it != names.end(); ++it) {
        if (*it != "Default") {
            CustomStyle * styleData = manager->style(*it);
            if (styleData && !styleData->parentName().isNull()) {
                CustomStyle *pstyle = manager->style(styleData->parentName());
                if (pstyle) styleData->setParentName(pstyle->name());
            }
        }
    }

    return true;
}


void Ksp::saveStyle(const Style &style, QDomDocument& doc, QDomElement& format, const StyleManager* styleManager)
{
    QSet<Style::Key> keysToStore = style.definedKeys(styleManager);

    if (keysToStore.contains(Style::HorizontalAlignment) && style.halign() != Style::HAlignUndefined)
        format.setAttribute(style.type() == Style::AUTO ? "align" : "alignX", (int) style.halign());

    if (keysToStore.contains(Style::VerticalAlignment) && style.valign() != Style::VAlignUndefined)
        format.setAttribute("alignY", QString::number((int) style.valign()));

    if (keysToStore.contains(Style::BackgroundColor) && style.backgroundColor().isValid())
        format.setAttribute("bgcolor", style.backgroundColor().name());

    if (keysToStore.contains(Style::MultiRow) && style.wrapText())
        format.setAttribute("multirow", "yes");

    if (keysToStore.contains(Style::VerticalText) && style.verticalText())
        format.setAttribute("verticaltext", "yes");

    if (keysToStore.contains(Style::ShrinkToFit) && style.shrinkToFit())
        format.setAttribute("shrinktofit", "yes");

    if (keysToStore.contains(Style::Precision))
        format.setAttribute("precision", QString::number(style.precision()));

    if (keysToStore.contains(Style::Prefix) && !style.prefix().isEmpty())
        format.setAttribute("prefix", style.prefix());

    if (keysToStore.contains(Style::Postfix) && !style.postfix().isEmpty())
        format.setAttribute("postfix", style.postfix());

    if (keysToStore.contains(Style::FloatFormatKey))
        format.setAttribute("float", QString::number((int) style.floatFormat()));

    if (keysToStore.contains(Style::FloatColorKey))
        format.setAttribute("floatcolor", QString::number((int)style.floatColor()));

    if (keysToStore.contains(Style::FormatTypeKey))
        format.setAttribute("format", QString::number((int) style.formatType()));

    if (keysToStore.contains(Style::CustomFormat) && !style.customFormat().isEmpty())
        format.setAttribute("custom", style.customFormat());

    if (keysToStore.contains(Style::FormatTypeKey) && style.formatType() == Format::Money) {
        format.setAttribute("type", 0);
        format.setAttribute("symbol", style.currency().symbol());
    }

    if (keysToStore.contains(Style::Angle))
        format.setAttribute("angle", QString::number(style.angle()));

    if (keysToStore.contains(Style::Indentation))
        format.setAttribute("indent", QString::number(style.indentation()));

    if (keysToStore.contains(Style::DontPrintText))
        format.setAttribute("dontprinttext", style.printText() ? "no" : "yes");

    if (keysToStore.contains(Style::NotProtected))
        format.setAttribute("noprotection", style.notProtected() ? "yes" : "no");

    if (keysToStore.contains(Style::HideAll))
        format.setAttribute("hideall", style.hideAll() ? "yes" : "no");

    if (keysToStore.contains(Style::HideFormula))
        format.setAttribute("hideformula", style.hideFormula() ? "yes" : "no");

    if (style.type() == Style::AUTO) {
        if (keysToStore.contains(Style::FontFamily) ||
                keysToStore.contains(Style::FontSize) ||
                keysToStore.contains(Style::FontBold) ||
                keysToStore.contains(Style::FontItalic) ||
                keysToStore.contains(Style::FontStrike) ||
                keysToStore.contains(Style::FontUnderline)) {
            format.appendChild(createElement("font", style.font(), doc));
        }
    } else { // custom style
        if (keysToStore.contains(Style::FontFamily))
            format.setAttribute("font-family", style.fontFamily());
        if (keysToStore.contains(Style::FontSize))
            format.setAttribute("font-size", QString::number(style.fontSize()));
        if (keysToStore.contains(Style::FontBold) || keysToStore.contains(Style::FontItalic) ||
                keysToStore.contains(Style::FontUnderline) || keysToStore.contains(Style::FontStrike)) {
            enum FontFlags {
                FBold      = 0x01,
                FUnderline = 0x02,
                FItalic    = 0x04,
                FStrike    = 0x08
            };
            int fontFlags = 0;
            fontFlags |= style.bold()      ? FBold      : 0;
            fontFlags |= style.italic()    ? FItalic    : 0;
            fontFlags |= style.underline() ? FUnderline : 0;
            fontFlags |= style.strikeOut() ? FStrike    : 0;
            format.setAttribute("font-flags", QString::number(fontFlags));
        }
    }

    if (keysToStore.contains(Style::FontColor) && style.fontColor().isValid())
        format.appendChild(createElement("pen", style.fontColor(), doc));

    if (keysToStore.contains(Style::BackgroundBrush)) {
        format.setAttribute("brushcolor", style.backgroundBrush().color().name());
        format.setAttribute("brushstyle", QString::number((int) style.backgroundBrush().style()));
    }

    if (keysToStore.contains(Style::LeftPen)) {
        QDomElement left = doc.createElement("left-border");
        left.appendChild(createElement("pen", style.leftBorderPen(), doc));
        format.appendChild(left);
    }

    if (keysToStore.contains(Style::TopPen)) {
        QDomElement top = doc.createElement("top-border");
        top.appendChild(createElement("pen", style.topBorderPen(), doc));
        format.appendChild(top);
    }

    if (keysToStore.contains(Style::RightPen)) {
        QDomElement right = doc.createElement("right-border");
        right.appendChild(createElement("pen", style.rightBorderPen(), doc));
        format.appendChild(right);
    }

    if (keysToStore.contains(Style::BottomPen)) {
        QDomElement bottom = doc.createElement("bottom-border");
        bottom.appendChild(createElement("pen", style.bottomBorderPen(), doc));
        format.appendChild(bottom);
    }

    if (keysToStore.contains(Style::FallDiagonalPen)) {
        QDomElement fallDiagonal  = doc.createElement("fall-diagonal");
        fallDiagonal.appendChild(createElement("pen", style.fallDiagonalPen(), doc));
        format.appendChild(fallDiagonal);
    }

    if (keysToStore.contains(Style::GoUpDiagonalPen)) {
        QDomElement goUpDiagonal = doc.createElement("up-diagonal");
        goUpDiagonal.appendChild(createElement("pen", style.goUpDiagonalPen(), doc));
        format.appendChild(goUpDiagonal);
    }
}

bool Ksp::loadStyle(Style *style, KoXmlElement& format)
{
    if (format.hasAttribute("parent"))
        style->setParentName(format.attribute("parent"));

    bool ok;
    if (format.hasAttribute(style->type() == Style::AUTO ? "align" : "alignX")) {
        Style::HAlign a = (Style::HAlign) format.attribute(style->type() == Style::AUTO ? "align" : "alignX").toInt(&ok);
        if (!ok)
            return false;
        if ((unsigned int) a >= 1 && (unsigned int) a <= 4) {
            style->setHAlign(a);
        }
    }
    if (format.hasAttribute("alignY")) {
        Style::VAlign a = (Style::VAlign) format.attribute("alignY").toInt(&ok);
        if (!ok)
            return false;
        if ((unsigned int) a >= 1 && (unsigned int) a < 4) {
            style->setVAlign(a);
        }
    }

    if (format.hasAttribute("bgcolor")) {
        QColor color(format.attribute("bgcolor"));
        if (color.isValid())
            style->setBackgroundColor(color);
    }

    if (format.hasAttribute("multirow")) {
        style->setWrapText(true);
    }

    if (format.hasAttribute("shrinktofit")) {
        style->setShrinkToFit(true);
    }

    if (format.hasAttribute("precision")) {
        int i = format.attribute("precision").toInt(&ok);
        if (i < -1) {
            debugSheetsODF << "Value out of range Cell::precision=" << i;
            return false;
        }
        // special handling for precision
        // The Style default (-1) and the storage default (0) differ.
        if (style->type() == Style::AUTO && i == -1)
            i = 0;
        // The maximum is 10. Replace the Style value 0 with -11, which always results
        // in a storage value < 0 and is interpreted as Style value 0.
        else if (style->type() == Style::AUTO && i == 0)
            i = -11;
        style->setPrecision(i);
    }

    if (format.hasAttribute("float")) {
        Style::FloatFormat a = (Style::FloatFormat)format.attribute("float").toInt(&ok);
        if (!ok)
            return false;
        if ((unsigned int) a >= 1 && (unsigned int) a <= 3) {
            style->setFloatFormat(a);
        }
    }

    if (format.hasAttribute("floatcolor")) {
        Style::FloatColor a = (Style::FloatColor) format.attribute("floatcolor").toInt(&ok);
        if (!ok) return false;
        if ((unsigned int) a >= 1 && (unsigned int) a <= 2) {
            style->setFloatColor(a);
        }
    }

    if (format.hasAttribute("format")) {
        int fo = format.attribute("format").toInt(&ok);
        if (! ok)
            return false;
        style->setFormatType(static_cast<Format::Type>(fo));
    }
    if (format.hasAttribute("custom")) {
        style->setCustomFormat(format.attribute("custom"));
    }
    if (style->formatType() == Format::Money) {
        ok = true;
        Currency currency;
        if (format.hasAttribute("type")) {
            if (format.hasAttribute("symbol"))
                currency = Currency(format.attribute("symbol"));
        } else if (format.hasAttribute("symbol"))
            currency = Currency(format.attribute("symbol"));
        style->setCurrency(currency);
    }
    if (format.hasAttribute("angle")) {
        style->setAngle(format.attribute("angle").toInt(&ok));
        if (!ok)
            return false;
    }
    if (format.hasAttribute("indent")) {
        style->setIndentation(format.attribute("indent").toDouble(&ok));
        if (!ok)
            return false;
    }
    if (format.hasAttribute("dontprinttext")) {
        style->setDontPrintText(true);
    }

    if (format.hasAttribute("noprotection")) {
        style->setNotProtected(true);
    }

    if (format.hasAttribute("hideall")) {
        style->setHideAll(true);
    }

    if (format.hasAttribute("hideformula")) {
        style->setHideFormula(true);
    }

    if (style->type() == Style::AUTO) {
        KoXmlElement fontElement = format.namedItem("font").toElement();
        if (!fontElement.isNull()) {
            QFont font(toFont(fontElement));
            style->setFontFamily(font.family());
            style->setFontSize(font.pointSize());
            if (font.italic())
                style->setFontItalic(true);
            if (font.bold())
                style->setFontBold(true);
            if (font.underline())
                style->setFontUnderline(true);
            if (font.strikeOut())
                style->setFontStrikeOut(true);
        }
    } else { // custom style
        if (format.hasAttribute("font-family"))
            style->setFontFamily(format.attribute("font-family"));
        if (format.hasAttribute("font-size")) {
            style->setFontSize(format.attribute("font-size").toInt(&ok));
            if (!ok)
                return false;
        }
        if (format.hasAttribute("font-flags")) {
            int fontFlags = format.attribute("font-flags").toInt(&ok);
            if (!ok)
                return false;

            enum FontFlags {
                FBold      = 0x01,
                FUnderline = 0x02,
                FItalic    = 0x04,
                FStrike    = 0x08
            };
            style->setFontBold(fontFlags & FBold);
            style->setFontItalic(fontFlags & FItalic);
            style->setFontUnderline(fontFlags & FUnderline);
            style->setFontStrikeOut(fontFlags & FStrike);
        }
    }

    if (format.hasAttribute("brushcolor")) {
        QColor color(format.attribute("brushcolor"));
        if (color.isValid()) {
            QBrush brush = style->backgroundBrush();
            brush.setColor(color);
            style->setBackgroundBrush(brush);
        }
    }

    if (format.hasAttribute("brushstyle")) {
        QBrush brush = style->backgroundBrush();
        brush.setStyle((Qt::BrushStyle) format.attribute("brushstyle").toInt(&ok));
        if (!ok)
            return false;
        style->setBackgroundBrush(brush);
    }

    KoXmlElement pen = format.namedItem("pen").toElement();
    if (!pen.isNull()) {
        style->setFontColor(toPen(pen).color());
    }

    KoXmlElement left = format.namedItem("left-border").toElement();
    if (!left.isNull()) {
        KoXmlElement pen = left.namedItem("pen").toElement();
        if (!pen.isNull())
            style->setLeftBorderPen(toPen(pen));
    }

    KoXmlElement top = format.namedItem("top-border").toElement();
    if (!top.isNull()) {
        KoXmlElement pen = top.namedItem("pen").toElement();
        if (!pen.isNull())
            style->setTopBorderPen(toPen(pen));
    }

    KoXmlElement right = format.namedItem("right-border").toElement();
    if (!right.isNull()) {
        KoXmlElement pen = right.namedItem("pen").toElement();
        if (!pen.isNull())
            style->setRightBorderPen(toPen(pen));
    }

    KoXmlElement bottom = format.namedItem("bottom-border").toElement();
    if (!bottom.isNull()) {
        KoXmlElement pen = bottom.namedItem("pen").toElement();
        if (!pen.isNull())
            style->setBottomBorderPen(toPen(pen));
    }

    KoXmlElement fallDiagonal = format.namedItem("fall-diagonal").toElement();
    if (!fallDiagonal.isNull()) {
        KoXmlElement pen = fallDiagonal.namedItem("pen").toElement();
        if (!pen.isNull())
            style->setFallDiagonalPen(toPen(pen));
    }

    KoXmlElement goUpDiagonal = format.namedItem("up-diagonal").toElement();
    if (!goUpDiagonal.isNull()) {
        KoXmlElement pen = goUpDiagonal.namedItem("pen").toElement();
        if (!pen.isNull())
            style->setGoUpDiagonalPen(toPen(pen));
    }

    if (format.hasAttribute("prefix")) {
        style->setPrefix(format.attribute("prefix"));
    }
    if (format.hasAttribute("postfix")) {
        style->setPostfix(format.attribute("postfix"));
    }

    return true;
}

void Ksp::saveCustomStyle(CustomStyle *s, QDomDocument& doc, QDomElement& styles, const StyleManager* styleManager)
{
    if (s->name().isEmpty())
        return;

    QDomElement style(doc.createElement("style"));
    style.setAttribute("type", QString::number((int) s->type()));
    if (!s->parentName().isNull())
        style.setAttribute("parent", s->parentName());
    style.setAttribute("name", s->name());

    QDomElement format(doc.createElement("format"));
    saveStyle(*s, doc, format, styleManager);
    style.appendChild(format);

    styles.appendChild(style);
}

bool Ksp::loadCustomStyle(CustomStyle *s, KoXmlElement const & style, QString const & name)
{
    s->setName(name);

    if (style.hasAttribute("parent"))
        s->setParentName(style.attribute("parent"));

    if (!style.hasAttribute("type"))
        return false;

    bool ok = true;
    s->setType((Style::StyleType) style.attribute("type").toInt(&ok));
    if (!ok)
        return false;

    KoXmlElement f(style.namedItem("format").toElement());
    if (!f.isNull())
        if (!loadStyle(s, f))
            return false;

    return true;
}


QPen Ksp::toPen(KoXmlElement & element)
{
    bool ok;
    QPen p;

    p.setStyle((Qt::PenStyle)element.attribute("style").toInt(&ok));
    if (!ok)
        return QPen();

    p.setWidth(element.attribute("width").toInt(&ok));
    if (!ok)
        return QPen();

    p.setColor(QColor(element.attribute("color")));

    return p;
}

QFont Ksp::toFont(KoXmlElement & element)
{
    QFont f;
    f.setFamily(element.attribute("family"));

    bool ok;
    const int size = element.attribute("size").toInt(&ok);
    if (ok)
        f.setPointSize(size);

    const int weight = element.attribute("weight").toInt(&ok);
    if (!ok)
        f.setWeight(weight);

    if (element.hasAttribute("italic") && element.attribute("italic") == "yes")
        f.setItalic(true);

    if (element.hasAttribute("bold") && element.attribute("bold") == "yes")
        f.setBold(true);

    if (element.hasAttribute("underline") && element.attribute("underline") == "yes")
        f.setUnderline(true);

    if (element.hasAttribute("strikeout") && element.attribute("strikeout") == "yes")
        f.setStrikeOut(true);

    /* Uncomment when charset is added to kspread_dlg_layout
       + save a document-global charset
       if ( element.hasAttribute( "charset" ) )
         KCharsets::charsets()->setQFont( f, element.attribute("charset") );
        else
    */
    // ######## Not needed anymore in 3.0?
    //KCharsets::charsets()->setQFont( f, KLocale::global()->charset() );

    return f;
}

QDomElement Ksp::createElement(const QString & tagName, const QFont & font, QDomDocument & doc)
{
    QDomElement e(doc.createElement(tagName));

    e.setAttribute("family", font.family());
    e.setAttribute("size", QString::number(font.pointSize()));
    e.setAttribute("weight", QString::number(font.weight()));
    if (font.bold())
        e.setAttribute("bold", "yes");
    if (font.italic())
        e.setAttribute("italic", "yes");
    if (font.underline())
        e.setAttribute("underline", "yes");
    if (font.strikeOut())
        e.setAttribute("strikeout", "yes");
    //e.setAttribute( "charset", KCharsets::charsets()->name( font ) );

    return e;
}

QDomElement Ksp::createElement(const QString & tagname, const QPen & pen, QDomDocument & doc)
{
    QDomElement e(doc.createElement(tagname));
    e.setAttribute("color", pen.color().name());
    e.setAttribute("style", QString::number((int)pen.style()));
    e.setAttribute("width", QString::number((int)pen.width())); // not qreal, would need spec change
    return e;
}



}  // Sheets
}  // Calligra

