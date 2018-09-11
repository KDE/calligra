/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2003 Norbert Andres <nandres@web.de>

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
   Boston, MA 02110-1301, USA.
*/

// Local
#include "Style.h"

#include <QBrush>
#include <QHash>
#include <QPen>
#include <QFontDatabase>

#include <KoGlobal.h>

#include "SheetsDebug.h"
#include "Currency.h"
#include "Global.h"
#include "StyleManager.h"
#include "Util.h"

using namespace Calligra::Sheets;

/////////////////////////////////////////////////////////////////////////////
//
// SubStyles
//
/////////////////////////////////////////////////////////////////////////////

namespace Calligra
{
namespace Sheets
{

static uint calculateValue(QPen const & pen)
{
    uint n = pen.color().red() + pen.color().green() + pen.color().blue();
    n += 1000 * pen.width();
    n += 10000 * (uint) pen.style();
    return n;
}

// specialized debug method
template<>
QString SubStyleOne<Style::CurrencyFormat, Currency>::debugData(bool withName) const
{
    QString out; if (withName) out = name(Style::CurrencyFormat) + ' '; QDebug qdbg(&out); qdbg << value1.symbol(); return out;
}

template<Style::Key key>
class PenStyle : public SubStyleOne<key, QPen>
{
public:
    PenStyle(const QPen& p = Qt::NoPen) : SubStyleOne<key, QPen>(p) {}
};

template<Style::Key key>
class BorderPenStyle : public PenStyle<key>
{
public:
    BorderPenStyle(const QPen& p = Qt::NoPen) : PenStyle<key>(p), value(calculateValue(p)) {}
    int value;
};

QString SubStyle::name(Style::Key key)
{
    QString name;
    switch (key) {
    case Style::DefaultStyleKey:        name = "Default style"; break;
    case Style::NamedStyleKey:          name = "Named style"; break;
    case Style::LeftPen:                name = "Left pen"; break;
    case Style::RightPen:               name = "Right pen"; break;
    case Style::TopPen:                 name = "Top pen"; break;
    case Style::BottomPen:              name = "Bottom pen"; break;
    case Style::FallDiagonalPen:        name = "Fall diagonal pen"; break;
    case Style::GoUpDiagonalPen:        name = "Go up diagonal pen"; break;
    case Style::HorizontalAlignment:    name = "Horz. alignment"; break;
    case Style::VerticalAlignment:      name = "Vert. alignment"; break;
    case Style::MultiRow:               name = "Wrap text"; break;
    case Style::VerticalText:           name = "Vertical text"; break;
    case Style::Angle:                  name = "Angle"; break;
    case Style::Indentation:            name = "Indentation"; break;
    case Style::ShrinkToFit:            name = "Shrink to Fit"; break;
    case Style::Prefix:                 name = "Prefix"; break;
    case Style::Postfix:                name = "Postfix"; break;
    case Style::Precision:              name = "Precision"; break;
    case Style::ThousandsSep:           name = "Thousands separator"; break;
    case Style::FormatTypeKey:          name = "Format type"; break;
    case Style::FloatFormatKey:         name = "Float format"; break;
    case Style::FloatColorKey:          name = "Float color"; break;
    case Style::CurrencyFormat:         name = "Currency"; break;
    case Style::CustomFormat:           name = "Custom format"; break;
    case Style::BackgroundBrush:        name = "Background brush"; break;
    case Style::BackgroundColor:        name = "Background color"; break;
    case Style::FontColor:              name = "Font color"; break;
    case Style::FontFamily:             name = "Font family"; break;
    case Style::FontSize:               name = "Font size"; break;
    case Style::FontBold:               name = "Font bold"; break;
    case Style::FontItalic:             name = "Font italic"; break;
    case Style::FontStrike:             name = "Font strikeout"; break;
    case Style::FontUnderline:          name = "Font underline"; break;
    case Style::DontPrintText:          name = "Do not print text"; break;
    case Style::NotProtected:           name = "Not protected"; break;
    case Style::HideAll:                name = "Hide all"; break;
    case Style::HideFormula:            name = "Hide formula"; break;
    }
    return name;
}

SharedSubStyle SharedSubStyle::s_defaultStyle(new SubStyle());

} // namespace Sheets
} // namespace Calligra

/////////////////////////////////////////////////////////////////////////////
//
// Style::Private
//
/////////////////////////////////////////////////////////////////////////////

class Q_DECL_HIDDEN Style::Private : public QSharedData
{
public:
    QHash<Key, SharedSubStyle> subStyles;
};


/////////////////////////////////////////////////////////////////////////////
//
// Style
//
/////////////////////////////////////////////////////////////////////////////

Style::Style()
        : d(new Private)
{
}

Style::Style(const Style& style)
        : d(style.d)
{
}

Style::~Style()
{
}

Style::StyleType Style::type() const
{
    return AUTO;
}

QString Style::parentName() const
{
    if (!d->subStyles.contains(NamedStyleKey))
        return QString();
    return static_cast<const NamedStyle*>(d->subStyles[NamedStyleKey].data())->name;
}

void Style::setParentName(const QString& name)
{
    d->subStyles.insert(NamedStyleKey, SharedSubStyle(new NamedStyle(name)));
}

void Style::clearAttribute(Key key)
{
    d->subStyles.remove(key);
}

bool Style::hasAttribute(Key key) const
{
    return d->subStyles.contains(key);
}


void Style::saveXML(QDomDocument& doc, QDomElement& format, const StyleManager* styleManager) const
{
    // list of substyles to store
    QSet<Key> keysToStore;

    if (d->subStyles.contains(NamedStyleKey)) {
        const CustomStyle* namedStyle = styleManager->style(parentName());
        // check, if it's an unmodified named style
        keysToStore = difference(*namedStyle);
        if (type() == AUTO) {
            const QList<Key> keys = keysToStore.toList();
            if ((keysToStore.count() == 0) ||
                    (keysToStore.count() == 1 && keysToStore.toList().first() == NamedStyleKey)) {
                // just save the name and we are done.
                format.setAttribute("style-name", parentName());
                return;
            } else
                format.setAttribute("parent", parentName());
        } else { // custom style
            if (d->subStyles.contains(NamedStyleKey))
                format.setAttribute("parent", parentName());
        }
    } else
        keysToStore = QSet<Key>::fromList(d->subStyles.keys());

    if (keysToStore.contains(HorizontalAlignment) && halign() != HAlignUndefined)
        format.setAttribute(type() == AUTO ? "align" : "alignX", (int) halign());

    if (keysToStore.contains(VerticalAlignment) && valign() != VAlignUndefined)
        format.setAttribute("alignY", QString::number((int) valign()));

    if (keysToStore.contains(BackgroundColor) && backgroundColor().isValid())
        format.setAttribute("bgcolor", backgroundColor().name());

    if (keysToStore.contains(MultiRow) && wrapText())
        format.setAttribute("multirow", "yes");

    if (keysToStore.contains(VerticalText) && verticalText())
        format.setAttribute("verticaltext", "yes");

    if (keysToStore.contains(ShrinkToFit) && shrinkToFit())
        format.setAttribute("shrinktofit", "yes");

    if (keysToStore.contains(Precision))
        format.setAttribute("precision", QString::number(precision()));

    if (keysToStore.contains(Prefix) && !prefix().isEmpty())
        format.setAttribute("prefix", prefix());

    if (keysToStore.contains(Postfix) && !postfix().isEmpty())
        format.setAttribute("postfix", postfix());

    if (keysToStore.contains(FloatFormatKey))
        format.setAttribute("float", QString::number((int) floatFormat()));

    if (keysToStore.contains(FloatColorKey))
        format.setAttribute("floatcolor", QString::number((int)floatColor()));

    if (keysToStore.contains(FormatTypeKey))
        format.setAttribute("format", QString::number((int) formatType()));

    if (keysToStore.contains(CustomFormat) && !customFormat().isEmpty())
        format.setAttribute("custom", customFormat());

    if (keysToStore.contains(FormatTypeKey) && formatType() == Format::Money) {
        format.setAttribute("type", (int) currency().index());
        format.setAttribute("symbol", currency().symbol());
    }

    if (keysToStore.contains(Angle))
        format.setAttribute("angle", QString::number(angle()));

    if (keysToStore.contains(Indentation))
        format.setAttribute("indent", QString::number(indentation()));

    if (keysToStore.contains(DontPrintText))
        format.setAttribute("dontprinttext", printText() ? "no" : "yes");

    if (keysToStore.contains(NotProtected))
        format.setAttribute("noprotection", notProtected() ? "yes" : "no");

    if (keysToStore.contains(HideAll))
        format.setAttribute("hideall", hideAll() ? "yes" : "no");

    if (keysToStore.contains(HideFormula))
        format.setAttribute("hideformula", hideFormula() ? "yes" : "no");

    if (type() == AUTO) {
        if (keysToStore.contains(FontFamily) ||
                keysToStore.contains(FontSize) ||
                keysToStore.contains(FontBold) ||
                keysToStore.contains(FontItalic) ||
                keysToStore.contains(FontStrike) ||
                keysToStore.contains(FontUnderline)) {
            format.appendChild(NativeFormat::createElement("font", font(), doc));
        }
    } else { // custom style
        if (keysToStore.contains(FontFamily))
            format.setAttribute("font-family", fontFamily());
        if (keysToStore.contains(FontSize))
            format.setAttribute("font-size", QString::number(fontSize()));
        if (keysToStore.contains(FontBold) || keysToStore.contains(FontItalic) ||
                keysToStore.contains(FontUnderline) || keysToStore.contains(FontStrike)) {
            enum FontFlags {
                FBold      = 0x01,
                FUnderline = 0x02,
                FItalic    = 0x04,
                FStrike    = 0x08
            };
            int fontFlags = 0;
            fontFlags |= bold()      ? FBold      : 0;
            fontFlags |= italic()    ? FItalic    : 0;
            fontFlags |= underline() ? FUnderline : 0;
            fontFlags |= strikeOut() ? FStrike    : 0;
            format.setAttribute("font-flags", QString::number(fontFlags));
        }
    }

    if (keysToStore.contains(FontColor) && fontColor().isValid())
        format.appendChild(NativeFormat::createElement("pen", fontColor(), doc));

    if (keysToStore.contains(BackgroundBrush)) {
        format.setAttribute("brushcolor", backgroundBrush().color().name());
        format.setAttribute("brushstyle", QString::number((int) backgroundBrush().style()));
    }

    if (keysToStore.contains(LeftPen)) {
        QDomElement left = doc.createElement("left-border");
        left.appendChild(NativeFormat::createElement("pen", leftBorderPen(), doc));
        format.appendChild(left);
    }

    if (keysToStore.contains(TopPen)) {
        QDomElement top = doc.createElement("top-border");
        top.appendChild(NativeFormat::createElement("pen", topBorderPen(), doc));
        format.appendChild(top);
    }

    if (keysToStore.contains(RightPen)) {
        QDomElement right = doc.createElement("right-border");
        right.appendChild(NativeFormat::createElement("pen", rightBorderPen(), doc));
        format.appendChild(right);
    }

    if (keysToStore.contains(BottomPen)) {
        QDomElement bottom = doc.createElement("bottom-border");
        bottom.appendChild(NativeFormat::createElement("pen", bottomBorderPen(), doc));
        format.appendChild(bottom);
    }

    if (keysToStore.contains(FallDiagonalPen)) {
        QDomElement fallDiagonal  = doc.createElement("fall-diagonal");
        fallDiagonal.appendChild(NativeFormat::createElement("pen", fallDiagonalPen(), doc));
        format.appendChild(fallDiagonal);
    }

    if (keysToStore.contains(GoUpDiagonalPen)) {
        QDomElement goUpDiagonal = doc.createElement("up-diagonal");
        goUpDiagonal.appendChild(NativeFormat::createElement("pen", goUpDiagonalPen(), doc));
        format.appendChild(goUpDiagonal);
    }
}

bool Style::loadXML(KoXmlElement& format, Paste::Mode mode)
{
    if (format.hasAttribute("style-name")) {
        // Simply set the style name and we are done.
        insertSubStyle(NamedStyleKey, format.attribute("style-name"));
        return true;
    } else if (format.hasAttribute("parent"))
        insertSubStyle(NamedStyleKey, format.attribute("parent"));

    bool ok;
    if (format.hasAttribute(type() == AUTO ? "align" : "alignX")) {
        HAlign a = (HAlign) format.attribute(type() == AUTO ? "align" : "alignX").toInt(&ok);
        if (!ok)
            return false;
        if ((unsigned int) a >= 1 && (unsigned int) a <= 4) {
            setHAlign(a);
        }
    }
    if (format.hasAttribute("alignY")) {
        VAlign a = (VAlign) format.attribute("alignY").toInt(&ok);
        if (!ok)
            return false;
        if ((unsigned int) a >= 1 && (unsigned int) a < 4) {
            setVAlign(a);
        }
    }

    if (format.hasAttribute("bgcolor")) {
        QColor color(format.attribute("bgcolor"));
        if (color.isValid())
            setBackgroundColor(color);
    }

    if (format.hasAttribute("multirow")) {
        setWrapText(true);
    }

    if (format.hasAttribute("shrinktofit")) {
        setShrinkToFit(true);
    }

    if (format.hasAttribute("precision")) {
        int i = format.attribute("precision").toInt(&ok);
        if (i < -1) {
            debugSheetsODF << "Value out of range Cell::precision=" << i;
            return false;
        }
        // special handling for precision
        // The Style default (-1) and the storage default (0) differ.
        if (type() == AUTO && i == -1)
            i = 0;
        // The maximum is 10. Replace the Style value 0 with -11, which always results
        // in a storage value < 0 and is interpreted as Style value 0.
        else if (type() == AUTO && i == 0)
            i = -11;
        setPrecision(i);
    }

    if (format.hasAttribute("float")) {
        FloatFormat a = (FloatFormat)format.attribute("float").toInt(&ok);
        if (!ok)
            return false;
        if ((unsigned int) a >= 1 && (unsigned int) a <= 3) {
            setFloatFormat(a);
        }
    }

    if (format.hasAttribute("floatcolor")) {
        FloatColor a = (FloatColor) format.attribute("floatcolor").toInt(&ok);
        if (!ok) return false;
        if ((unsigned int) a >= 1 && (unsigned int) a <= 2) {
            setFloatColor(a);
        }
    }

    if (format.hasAttribute("format")) {
        int fo = format.attribute("format").toInt(&ok);
        if (! ok)
            return false;
        setFormatType(static_cast<Format::Type>(fo));
    }
    if (format.hasAttribute("custom")) {
        setCustomFormat(format.attribute("custom"));
    }
    if (formatType() == Format::Money) {
        ok = true;
        Currency currency;
        if (format.hasAttribute("type")) {
            currency = Currency(format.attribute("type").toInt(&ok));
            if (!ok) {
                if (format.hasAttribute("symbol"))
                    currency = Currency(format.attribute("symbol"));
            }
        } else if (format.hasAttribute("symbol"))
            currency = Currency(format.attribute("symbol"));
        setCurrency(currency);
    }
    if (format.hasAttribute("angle")) {
        setAngle(format.attribute("angle").toInt(&ok));
        if (!ok)
            return false;
    }
    if (format.hasAttribute("indent")) {
        setIndentation(format.attribute("indent").toDouble(&ok));
        if (!ok)
            return false;
    }
    if (format.hasAttribute("dontprinttext")) {
        setDontPrintText(true);
    }

    if (format.hasAttribute("noprotection")) {
        setNotProtected(true);
    }

    if (format.hasAttribute("hideall")) {
        setHideAll(true);
    }

    if (format.hasAttribute("hideformula")) {
        setHideFormula(true);
    }

    if (type() == AUTO) {
        KoXmlElement fontElement = format.namedItem("font").toElement();
        if (!fontElement.isNull()) {
            QFont font(NativeFormat::toFont(fontElement));
            setFontFamily(font.family());
            setFontSize(font.pointSize());
            if (font.italic())
                setFontItalic(true);
            if (font.bold())
                setFontBold(true);
            if (font.underline())
                setFontUnderline(true);
            if (font.strikeOut())
                setFontStrikeOut(true);
        }
    } else { // custom style
        if (format.hasAttribute("font-family"))
            setFontFamily(format.attribute("font-family"));
        if (format.hasAttribute("font-size")) {
            setFontSize(format.attribute("font-size").toInt(&ok));
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
            setFontBold(fontFlags & FBold);
            setFontItalic(fontFlags & FItalic);
            setFontUnderline(fontFlags & FUnderline);
            setFontStrikeOut(fontFlags & FStrike);
        }
    }

    if (format.hasAttribute("brushcolor")) {
        QColor color(format.attribute("brushcolor"));
        if (color.isValid()) {
            QBrush brush = backgroundBrush();
            brush.setColor(color);
            setBackgroundBrush(brush);
        }
    }

    if (format.hasAttribute("brushstyle")) {
        QBrush brush = backgroundBrush();
        brush.setStyle((Qt::BrushStyle) format.attribute("brushstyle").toInt(&ok));
        if (!ok)
            return false;
        setBackgroundBrush(brush);
    }

    KoXmlElement pen = format.namedItem("pen").toElement();
    if (!pen.isNull()) {
        setFontColor(NativeFormat::toPen(pen).color());
    }

    if (mode != Paste::NoBorder) {
        KoXmlElement left = format.namedItem("left-border").toElement();
        if (!left.isNull()) {
            KoXmlElement pen = left.namedItem("pen").toElement();
            if (!pen.isNull())
                setLeftBorderPen(NativeFormat::toPen(pen));
        }

        KoXmlElement top = format.namedItem("top-border").toElement();
        if (!top.isNull()) {
            KoXmlElement pen = top.namedItem("pen").toElement();
            if (!pen.isNull())
                setTopBorderPen(NativeFormat::toPen(pen));
        }

        KoXmlElement right = format.namedItem("right-border").toElement();
        if (!right.isNull()) {
            KoXmlElement pen = right.namedItem("pen").toElement();
            if (!pen.isNull())
                setRightBorderPen(NativeFormat::toPen(pen));
        }

        KoXmlElement bottom = format.namedItem("bottom-border").toElement();
        if (!bottom.isNull()) {
            KoXmlElement pen = bottom.namedItem("pen").toElement();
            if (!pen.isNull())
                setBottomBorderPen(NativeFormat::toPen(pen));
        }

        KoXmlElement fallDiagonal = format.namedItem("fall-diagonal").toElement();
        if (!fallDiagonal.isNull()) {
            KoXmlElement pen = fallDiagonal.namedItem("pen").toElement();
            if (!pen.isNull())
                setFallDiagonalPen(NativeFormat::toPen(pen));
        }

        KoXmlElement goUpDiagonal = format.namedItem("up-diagonal").toElement();
        if (!goUpDiagonal.isNull()) {
            KoXmlElement pen = goUpDiagonal.namedItem("pen").toElement();
            if (!pen.isNull())
                setGoUpDiagonalPen(NativeFormat::toPen(pen));
        }
    }

    if (format.hasAttribute("prefix")) {
        setPrefix(format.attribute("prefix"));
    }
    if (format.hasAttribute("postfix")) {
        setPostfix(format.attribute("postfix"));
    }

    return true;
}

uint Style::bottomPenValue() const
{
    if (!d->subStyles.contains(BottomPen))
        return BorderPenStyle<BottomPen>().value;
    return static_cast<const BorderPenStyle<BottomPen>*>(d->subStyles[BottomPen].data())->value;
}

uint Style::rightPenValue() const
{
    if (!d->subStyles.contains(RightPen))
        return BorderPenStyle<RightPen>().value;
    return static_cast<const BorderPenStyle<RightPen>*>(d->subStyles[RightPen].data())->value;
}

uint Style::leftPenValue() const
{
    if (!d->subStyles.contains(LeftPen))
        return BorderPenStyle<LeftPen>().value;
    return static_cast<const BorderPenStyle<LeftPen>*>(d->subStyles[LeftPen].data())->value;
}

uint Style::topPenValue() const
{
    if (!d->subStyles.contains(TopPen))
        return BorderPenStyle<TopPen>().value;
    return static_cast<const BorderPenStyle<TopPen>*>(d->subStyles[TopPen].data())->value;
}

QColor Style::fontColor() const
{
    if (!d->subStyles.contains(FontColor))
        return SubStyleOne<FontColor, QColor>(Qt::black).value1;
    return static_cast<const SubStyleOne<FontColor, QColor>*>(d->subStyles[FontColor].data())->value1;
}

QColor Style::backgroundColor() const
{
    if (!d->subStyles.contains(BackgroundColor))
        return SubStyleOne<BackgroundColor, QColor>().value1;
    return static_cast<const SubStyleOne<BackgroundColor, QColor>*>(d->subStyles[BackgroundColor].data())->value1;
}

QPen Style::rightBorderPen() const
{
    if (!d->subStyles.contains(RightPen))
        return BorderPenStyle<RightPen>().value1;
    return static_cast<const BorderPenStyle<RightPen>*>(d->subStyles[RightPen].data())->value1;
}

QPen Style::bottomBorderPen() const
{
    if (!d->subStyles.contains(BottomPen))
        return BorderPenStyle<BottomPen>().value1;
    return static_cast<const BorderPenStyle<BottomPen>*>(d->subStyles[BottomPen].data())->value1;
}

QPen Style::leftBorderPen() const
{
    if (!d->subStyles.contains(LeftPen))
        return BorderPenStyle<LeftPen>().value1;
    return static_cast<const BorderPenStyle<LeftPen>*>(d->subStyles[LeftPen].data())->value1;
}

QPen Style::topBorderPen() const
{
    if (!d->subStyles.contains(TopPen))
        return BorderPenStyle<TopPen>().value1;
    return static_cast<const BorderPenStyle<TopPen>*>(d->subStyles[TopPen].data())->value1;
}

QPen Style::fallDiagonalPen() const
{
    if (!d->subStyles.contains(FallDiagonalPen))
        return PenStyle<FallDiagonalPen>().value1;
    return static_cast<const PenStyle<FallDiagonalPen>*>(d->subStyles[FallDiagonalPen].data())->value1;
}

QPen Style::goUpDiagonalPen() const
{
    if (!d->subStyles.contains(GoUpDiagonalPen))
        return PenStyle<GoUpDiagonalPen>().value1;
    return static_cast<const PenStyle<GoUpDiagonalPen>*>(d->subStyles[GoUpDiagonalPen].data())->value1;
}

QBrush Style::backgroundBrush() const
{
    if (!d->subStyles.contains(BackgroundBrush))
        return SubStyleOne<BackgroundBrush, QBrush>(Qt::white).value1;
    return static_cast<const SubStyleOne<BackgroundBrush, QBrush>*>(d->subStyles[BackgroundBrush].data())->value1;
}

QString Style::customFormat() const
{
    if (!d->subStyles.contains(CustomFormat))
        return SubStyleOne<CustomFormat, QString>().value1;
    return static_cast<const SubStyleOne<CustomFormat, QString>*>(d->subStyles[CustomFormat].data())->value1;
}

QString Style::prefix() const
{
    if (!d->subStyles.contains(Prefix))
        return SubStyleOne<Prefix, QString>().value1;
    return static_cast<const SubStyleOne<Prefix, QString>*>(d->subStyles[Prefix].data())->value1;
}

QString Style::postfix() const
{
    if (!d->subStyles.contains(Postfix))
        return SubStyleOne<Postfix, QString>().value1;
    return static_cast<const SubStyleOne<Postfix, QString>*>(d->subStyles[Postfix].data())->value1;
}

QString Style::fontFamily() const
{
    if (!d->subStyles.contains(FontFamily))
        return KoGlobal::defaultFont().family(); // SubStyleOne<FontFamily, QString>().value1;
    return static_cast<const SubStyleOne<FontFamily, QString>*>(d->subStyles[FontFamily].data())->value1;
}

Style::HAlign Style::halign() const
{
    if (!d->subStyles.contains(HorizontalAlignment))
        return SubStyleOne<HorizontalAlignment, Style::HAlign>().value1;
    return static_cast<const SubStyleOne<HorizontalAlignment, Style::HAlign>*>(d->subStyles[HorizontalAlignment].data())->value1;
}

Style::VAlign Style::valign() const
{
    if (!d->subStyles.contains(VerticalAlignment))
        return SubStyleOne<VerticalAlignment, Style::VAlign>().value1;
    return static_cast<const SubStyleOne<VerticalAlignment, Style::VAlign>*>(d->subStyles[VerticalAlignment].data())->value1;
}

Style::FloatFormat Style::floatFormat() const
{
    if (!d->subStyles.contains(FloatFormatKey))
        return SubStyleOne<FloatFormatKey, FloatFormat>().value1;
    return static_cast<const SubStyleOne<FloatFormatKey, FloatFormat>*>(d->subStyles[FloatFormatKey].data())->value1;
}

Style::FloatColor Style::floatColor() const
{
    if (!d->subStyles.contains(FloatColorKey))
        return SubStyleOne<FloatColorKey, FloatColor>().value1;
    return static_cast<const SubStyleOne<FloatColorKey, FloatColor>*>(d->subStyles[FloatColorKey].data())->value1;
}

Format::Type Style::formatType() const
{
    if (!d->subStyles.contains(FormatTypeKey))
        return SubStyleOne<FormatTypeKey, Format::Type>().value1;
    return static_cast<const SubStyleOne<FormatTypeKey, Format::Type>*>(d->subStyles[FormatTypeKey].data())->value1;
}

Currency Style::currency() const
{
    if (!d->subStyles.contains(CurrencyFormat))
        return Currency();
    return static_cast<const SubStyleOne<CurrencyFormat, Currency>*>(d->subStyles[CurrencyFormat].data())->value1;
}

QFont Style::font() const
{
    QFont font;
    font.setFamily(fontFamily());
    font.setPointSize(fontSize());
    font.setBold(bold());
    font.setItalic(italic());
    font.setUnderline(underline());
    font.setStrikeOut(strikeOut());
    return font;
}

bool Style::bold() const
{
    if (!d->subStyles.contains(FontBold))
        return SubStyleOne<FontBold, bool>().value1;
    return static_cast<const SubStyleOne<FontBold, bool>*>(d->subStyles[FontBold].data())->value1;
}

bool Style::italic() const
{
    if (!d->subStyles.contains(FontItalic))
        return SubStyleOne<FontItalic, bool>().value1;
    return static_cast<const SubStyleOne<FontItalic, bool>*>(d->subStyles[FontItalic].data())->value1;
}

bool Style::underline() const
{
    if (!d->subStyles.contains(FontUnderline))
        return SubStyleOne<FontUnderline, bool>().value1;
    return static_cast<const SubStyleOne<FontUnderline, bool>*>(d->subStyles[FontUnderline].data())->value1;
}

bool Style::strikeOut() const
{
    if (!d->subStyles.contains(FontStrike))
        return SubStyleOne<FontStrike, bool>().value1;
    return static_cast<const SubStyleOne<FontStrike, bool>*>(d->subStyles[FontStrike].data())->value1;
}

int Style::fontSize() const
{
    if (!d->subStyles.contains(FontSize))
        return KoGlobal::defaultFont().pointSize(); //SubStyleOne<FontSize, int>().value1;
    return static_cast<const SubStyleOne<FontSize, int>*>(d->subStyles[FontSize].data())->value1;
}

int Style::precision() const
{
    if (!d->subStyles.contains(Precision))
        return -1; //SubStyleOne<Precision, int>().value1;
    return static_cast<const SubStyleOne<Precision, int>*>(d->subStyles[Precision].data())->value1;
}

bool Style::thousandsSep() const
{
    if (!d->subStyles.contains(ThousandsSep))
        return false;
    return static_cast<const SubStyleOne<ThousandsSep, bool>*>(d->subStyles[ThousandsSep].data())->value1;
}

int Style::angle() const
{
    if (!d->subStyles.contains(Angle))
        return SubStyleOne<Angle, int>().value1;
    return static_cast<const SubStyleOne<Angle, int>*>(d->subStyles[Angle].data())->value1;
}

double Style::indentation() const
{
    if (!d->subStyles.contains(Indentation))
        return SubStyleOne<Indentation, int>().value1;
    return static_cast<const SubStyleOne<Indentation, int>*>(d->subStyles[Indentation].data())->value1;
}

bool Style::shrinkToFit() const
{
    if (!d->subStyles.contains(ShrinkToFit))
        return SubStyleOne<ShrinkToFit, bool>().value1;
    return static_cast<const SubStyleOne<ShrinkToFit, bool>*>(d->subStyles[ShrinkToFit].data())->value1;
}

bool Style::verticalText() const
{
    if (!d->subStyles.contains(VerticalText))
        return SubStyleOne<VerticalText, bool>().value1;
    return static_cast<const SubStyleOne<VerticalText, bool>*>(d->subStyles[VerticalText].data())->value1;
}

bool Style::wrapText() const
{
    if (!d->subStyles.contains(MultiRow))
        return SubStyleOne<MultiRow, bool>().value1;
    return static_cast<const SubStyleOne<MultiRow, bool>*>(d->subStyles[MultiRow].data())->value1;
}

bool Style::printText() const
{
    if (!d->subStyles.contains(DontPrintText))
        return !SubStyleOne<DontPrintText, bool>().value1;
    return !static_cast<const SubStyleOne<DontPrintText, bool>*>(d->subStyles[DontPrintText].data())->value1;
}

bool Style::hideAll() const
{
    if (!d->subStyles.contains(HideAll))
        return SubStyleOne<HideAll, bool>().value1;
    return static_cast<const SubStyleOne<HideAll, bool>*>(d->subStyles[HideAll].data())->value1;
}

bool Style::hideFormula() const
{
    if (!d->subStyles.contains(HideFormula))
        return SubStyleOne<HideFormula, bool>().value1;
    return static_cast<const SubStyleOne<HideFormula, bool>*>(d->subStyles[HideFormula].data())->value1;
}

bool Style::notProtected() const
{
    if (!d->subStyles.contains(NotProtected))
        return SubStyleOne<NotProtected, bool>().value1;
    return static_cast<const SubStyleOne<NotProtected, bool>*>(d->subStyles[NotProtected].data())->value1;
}

bool Style::isDefault() const
{
    return isEmpty() || d->subStyles.contains(DefaultStyleKey);
}

bool Style::isEmpty() const
{
    return d->subStyles.isEmpty();
}

void Style::setHAlign(HAlign align)
{
    insertSubStyle(HorizontalAlignment, align);
}

void Style::setVAlign(VAlign align)
{
    insertSubStyle(VerticalAlignment, align);
}

void Style::setFont(QFont const & font)
{
    insertSubStyle(FontFamily,     font.family());
    insertSubStyle(FontSize,       font.pointSize());
    insertSubStyle(FontBold,       font.bold());
    insertSubStyle(FontItalic,     font.italic());
    insertSubStyle(FontStrike,     font.strikeOut());
    insertSubStyle(FontUnderline,  font.underline());
}

void Style::setFontFamily(QString const & family)
{
    QString font = family;
    // use the KDE default for sans serif, not Qt's default - this is because Qt doesn't return the default system font here
    if (font.toLower() == "sans serif") {
        QFont f = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
        font = f.family();
    }

    insertSubStyle(FontFamily, font);
}

void Style::setFontBold(bool enabled)
{
    insertSubStyle(FontBold, enabled);
}

void Style::setFontItalic(bool enabled)
{
    insertSubStyle(FontItalic, enabled);
}

void Style::setFontUnderline(bool enabled)
{
    insertSubStyle(FontUnderline, enabled);
}

void Style::setFontStrikeOut(bool enabled)
{
    insertSubStyle(FontStrike, enabled);
}

void Style::setFontSize(int size)
{
    insertSubStyle(FontSize, size);
}

void Style::setFontColor(QColor const & color)
{
    insertSubStyle(FontColor, color);
}

void Style::setBackgroundColor(QColor const & color)
{
    insertSubStyle(BackgroundColor, color);
}

void Style::setRightBorderPen(QPen const & pen)
{
    insertSubStyle(RightPen, pen);
}

void Style::setBottomBorderPen(QPen const & pen)
{
    insertSubStyle(BottomPen, pen);
}

void Style::setLeftBorderPen(QPen const & pen)
{
    insertSubStyle(LeftPen, pen);
}

void Style::setTopBorderPen(QPen const & pen)
{
    insertSubStyle(TopPen, pen);
}

void Style::setFallDiagonalPen(QPen const & pen)
{
    insertSubStyle(FallDiagonalPen, pen);
}

void Style::setGoUpDiagonalPen(QPen const & pen)
{
    insertSubStyle(GoUpDiagonalPen, pen);
}

void Style::setAngle(int angle)
{
    insertSubStyle(Angle, angle);
}

void Style::setIndentation(double indent)
{
    insertSubStyle(Indentation, indent);
}

void Style::setBackgroundBrush(QBrush const & brush)
{
    insertSubStyle(BackgroundBrush, brush);
}

void Style::setFloatFormat(FloatFormat format)
{
    insertSubStyle(FloatFormatKey, format);
}

void Style::setFloatColor(FloatColor color)
{
    insertSubStyle(FloatColorKey, color);
}

void Style::setFormatType(Format::Type format)
{
    insertSubStyle(FormatTypeKey, format);
}

void Style::setCustomFormat(QString const & strFormat)
{
    insertSubStyle(CustomFormat, strFormat);
}

void Style::setPrecision(int precision)
{
    insertSubStyle(Precision, precision);
}

void Style::setThousandsSep(bool thousandsSep)
{
    insertSubStyle(ThousandsSep, thousandsSep);
}

void Style::setPrefix(QString const & prefix)
{
    insertSubStyle(Prefix, prefix);
}

void Style::setPostfix(QString const & postfix)
{
    insertSubStyle(Postfix, postfix);
}

void Style::setCurrency(Currency const & currency)
{
    QVariant variant;
    variant.setValue(currency);
    insertSubStyle(CurrencyFormat, variant);
}

void Style::setWrapText(bool enable)
{
    insertSubStyle(MultiRow, enable);
}

void Style::setHideAll(bool enable)
{
    insertSubStyle(HideAll, enable);
}

void Style::setHideFormula(bool enable)
{
    insertSubStyle(HideFormula, enable);
}

void Style::setNotProtected(bool enable)
{
    insertSubStyle(NotProtected, enable);
}

void Style::setDontPrintText(bool enable)
{
    insertSubStyle(DontPrintText, enable);
}

void Style::setVerticalText(bool enable)
{
    insertSubStyle(VerticalText, enable);
}

void Style::setShrinkToFit(bool enable)
{
    insertSubStyle(ShrinkToFit, enable);
}

void Style::setDefault()
{
    insertSubStyle(DefaultStyleKey, true);
}

void Style::clear()
{
    d->subStyles.clear();
}

bool Style::compare(const SubStyle* one, const SubStyle* two)
{
    if (!one || !two)
        return one == two;
    if (one->type() != two->type())
        return false;
    switch (one->type()) {
    case DefaultStyleKey:
        return true;
    case NamedStyleKey:
        return static_cast<const NamedStyle*>(one)->name == static_cast<const NamedStyle*>(two)->name;
        // borders
    case LeftPen:
        return static_cast<const SubStyleOne<LeftPen, QPen>*>(one)->value1 == static_cast<const SubStyleOne<LeftPen, QPen>*>(two)->value1;
    case RightPen:
        return static_cast<const SubStyleOne<RightPen, QPen>*>(one)->value1 == static_cast<const SubStyleOne<RightPen, QPen>*>(two)->value1;
    case TopPen:
        return static_cast<const SubStyleOne<TopPen, QPen>*>(one)->value1 == static_cast<const SubStyleOne<TopPen, QPen>*>(two)->value1;
    case BottomPen:
        return static_cast<const SubStyleOne<BottomPen, QPen>*>(one)->value1 == static_cast<const SubStyleOne<BottomPen, QPen>*>(two)->value1;
    case FallDiagonalPen:
        return static_cast<const SubStyleOne<FallDiagonalPen, QPen>*>(one)->value1 == static_cast<const SubStyleOne<FallDiagonalPen, QPen>*>(two)->value1;
    case GoUpDiagonalPen:
        return static_cast<const SubStyleOne<GoUpDiagonalPen, QPen>*>(one)->value1 == static_cast<const SubStyleOne<GoUpDiagonalPen, QPen>*>(two)->value1;
        // layout
    case HorizontalAlignment:
        return static_cast<const SubStyleOne<HorizontalAlignment, HAlign>*>(one)->value1 == static_cast<const SubStyleOne<HorizontalAlignment, HAlign>*>(two)->value1;
    case VerticalAlignment:
        return static_cast<const SubStyleOne<VerticalAlignment, VAlign>*>(one)->value1 == static_cast<const SubStyleOne<VerticalAlignment, VAlign>*>(two)->value1;
    case MultiRow:
        return static_cast<const SubStyleOne<MultiRow, bool>*>(one)->value1 == static_cast<const SubStyleOne<MultiRow, bool>*>(two)->value1;
    case VerticalText:
        return static_cast<const SubStyleOne<VerticalText, bool>*>(one)->value1 == static_cast<const SubStyleOne<VerticalText, bool>*>(two)->value1;
    case ShrinkToFit:
        return static_cast<const SubStyleOne<ShrinkToFit, bool>*>(one)->value1 == static_cast<const SubStyleOne<ShrinkToFit, bool>*>(two)->value1;
    case Angle:
        return static_cast<const SubStyleOne<Angle, int>*>(one)->value1 == static_cast<const SubStyleOne<Angle, int>*>(two)->value1;
    case Indentation:
        return static_cast<const SubStyleOne<Indentation, int>*>(one)->value1 == static_cast<const SubStyleOne<Indentation, int>*>(two)->value1;
        // content format
    case Prefix:
        return static_cast<const SubStyleOne<Prefix, QString>*>(one)->value1 == static_cast<const SubStyleOne<Prefix, QString>*>(two)->value1;
    case Postfix:
        return static_cast<const SubStyleOne<Postfix, QString>*>(one)->value1 == static_cast<const SubStyleOne<Postfix, QString>*>(two)->value1;
    case Precision:
        return static_cast<const SubStyleOne<Precision, int>*>(one)->value1 == static_cast<const SubStyleOne<Precision, int>*>(two)->value1;
    case ThousandsSep:
        return static_cast<const SubStyleOne<ThousandsSep, bool>*>(one)->value1 == static_cast<const SubStyleOne<ThousandsSep, bool>*>(two)->value1;
    case FormatTypeKey:
        return static_cast<const SubStyleOne<FormatTypeKey, Format::Type>*>(one)->value1 == static_cast<const SubStyleOne<FormatTypeKey, Format::Type>*>(two)->value1;
    case FloatFormatKey:
        return static_cast<const SubStyleOne<FloatFormatKey, FloatFormat>*>(one)->value1 == static_cast<const SubStyleOne<FloatFormatKey, FloatFormat>*>(two)->value1;
    case FloatColorKey:
        return static_cast<const SubStyleOne<FloatColorKey, FloatColor>*>(one)->value1 == static_cast<const SubStyleOne<FloatColorKey, FloatColor>*>(two)->value1;
    case CurrencyFormat: {
        Currency currencyOne = static_cast<const SubStyleOne<CurrencyFormat, Currency>*>(one)->value1;
        Currency currencyTwo = static_cast<const SubStyleOne<CurrencyFormat, Currency>*>(two)->value1;
        if (currencyOne != currencyTwo)
            return false;
        return true;
    }
    case CustomFormat:
        return static_cast<const SubStyleOne<CustomFormat, QString>*>(one)->value1 == static_cast<const SubStyleOne<CustomFormat, QString>*>(two)->value1;
        // background
    case BackgroundBrush:
        return static_cast<const SubStyleOne<BackgroundBrush, QBrush>*>(one)->value1 == static_cast<const SubStyleOne<BackgroundBrush, QBrush>*>(two)->value1;
    case BackgroundColor:
        return static_cast<const SubStyleOne<BackgroundColor, QColor>*>(one)->value1 == static_cast<const SubStyleOne<BackgroundColor, QColor>*>(two)->value1;
        // font
    case FontColor:
        return static_cast<const SubStyleOne<FontColor, QColor>*>(one)->value1 == static_cast<const SubStyleOne<FontColor, QColor>*>(two)->value1;
    case FontFamily:
        return static_cast<const SubStyleOne<FontFamily, QString>*>(one)->value1 == static_cast<const SubStyleOne<FontFamily, QString>*>(two)->value1;
    case FontSize:
        return static_cast<const SubStyleOne<FontSize, int>*>(one)->value1 == static_cast<const SubStyleOne<FontSize, int>*>(two)->value1;
    case FontBold:
        return static_cast<const SubStyleOne<FontBold, bool>*>(one)->value1 == static_cast<const SubStyleOne<FontBold, bool>*>(two)->value1;
    case FontItalic:
        return static_cast<const SubStyleOne<FontItalic, bool>*>(one)->value1 == static_cast<const SubStyleOne<FontItalic, bool>*>(two)->value1;
    case FontStrike:
        return static_cast<const SubStyleOne<FontStrike, bool>*>(one)->value1 == static_cast<const SubStyleOne<FontStrike, bool>*>(two)->value1;
    case FontUnderline:
        return static_cast<const SubStyleOne<FontUnderline, bool>*>(one)->value1 == static_cast<const SubStyleOne<FontUnderline, bool>*>(two)->value1;
        //misc
    case DontPrintText:
        return static_cast<const SubStyleOne<DontPrintText, bool>*>(one)->value1 == static_cast<const SubStyleOne<DontPrintText, bool>*>(two)->value1;
    case NotProtected:
        return static_cast<const SubStyleOne<NotProtected, bool>*>(one)->value1 == static_cast<const SubStyleOne<NotProtected, bool>*>(two)->value1;
    case HideAll:
        return static_cast<const SubStyleOne<HideAll, bool>*>(one)->value1 == static_cast<const SubStyleOne<HideAll, bool>*>(two)->value1;
    case HideFormula:
        return static_cast<const SubStyleOne<HideFormula, bool>*>(one)->value1 == static_cast<const SubStyleOne<HideFormula, bool>*>(two)->value1;
    default:
        return false;
    }
}

bool Style::operator==(const Style& other) const
{
    if (other.isEmpty())
        return isEmpty() ? true : false;
    const QSet<Key> keys = QSet<Key>::fromList(d->subStyles.keys() + other.d->subStyles.keys());
    const QSet<Key>::ConstIterator end = keys.constEnd();
    for (QSet<Key>::ConstIterator it = keys.constBegin(); it != end; ++it) {
        if (!compare(d->subStyles.value(*it).data(), other.d->subStyles.value(*it).data()))
            return false;
    }
    return true;
}

uint Calligra::Sheets::qHash(const Style& style)
{
    uint hash = 0;
    foreach (const SharedSubStyle& ss, style.subStyles()) {
        hash ^= ss->koHash();
    }
    return hash;
}

void Style::operator=(const Style & other)
{
    d = other.d;
}

Style Style::operator-(const Style& other) const
{
    Style style;
    const QSet<Key> keys = difference(other);
    const QSet<Key>::ConstIterator end = keys.constEnd();
    for (QSet<Key>::ConstIterator it = keys.constBegin(); it != end; ++it)
        style.insertSubStyle(d->subStyles[*it]);
    return style;
}

void Style::merge(const Style& style)
{
    const QList<SharedSubStyle> subStyles(style.subStyles());
//     debugSheetsStyle <<"merging" << subStyles.count() <<" attributes.";
    for (int i = 0; i < subStyles.count(); ++i) {
//         debugSheetsStyle << subStyles[i]->debugData();
        insertSubStyle(subStyles[i]);
    }
}

QSet<Style::Key> Style::difference(const Style& other) const
{
    QSet<Key> result;
    const QSet<Key> keys = QSet<Key>::fromList(d->subStyles.keys() + other.d->subStyles.keys());
    const QSet<Key>::ConstIterator end = keys.constEnd();
    for (QSet<Key>::ConstIterator it = keys.constBegin(); it != end; ++it) {
        if (!other.d->subStyles.contains(*it))
            result.insert(*it);
        else if (d->subStyles.contains(*it)) { // both contain this key
            if (!compare(d->subStyles.value(*it).data(), other.d->subStyles.value(*it).data()))
                result.insert(*it);
        }
    }
    return result;
}

void Style::dump() const
{
    for (int i = 0; i < subStyles().count(); ++i)
        subStyles()[i]->dump();
}

QTextCharFormat Style::asCharFormat() const
{
    QTextCharFormat format;
    format.setFont(font());
    format.setFontWeight(bold() ? QFont::Bold : QFont::Normal);
    format.setFontItalic(italic());
    format.setFontUnderline(underline());
    format.setFontStrikeOut(strikeOut());
    return format;
}

QSet<Style::Key> Style::definedKeys(const StyleManager *styles) const
{
    QSet<Style::Key> keys;

    if (isDefault()) return keys;

    if (hasAttribute(Style::NamedStyleKey)) 
    {
        // it's not really the parent name in this case
        CustomStyle* namedStyle = styles->style(parentName());
        // remove substyles already present in named style
        if (namedStyle) keys = difference(*namedStyle);
    }
    else
        keys = QSet<Style::Key>::fromList(d->subStyles.keys());

    return keys;
}

QList<SharedSubStyle> Style::subStyles() const
{
    return d->subStyles.values();
}

SharedSubStyle Style::createSubStyle(Key key, const QVariant& value)
{
    SharedSubStyle newSubStyle;
    switch (key) {
        // special cases
    case DefaultStyleKey:
        newSubStyle = new SubStyle();
        break;
    case NamedStyleKey:
        newSubStyle = new NamedStyle(value.value<QString>());
        break;
    case LeftPen:
        newSubStyle = new BorderPenStyle<LeftPen>(value.value<QPen>());
        break;
    case RightPen:
        newSubStyle = new BorderPenStyle<RightPen>(value.value<QPen>());
        break;
    case TopPen:
        newSubStyle = new BorderPenStyle<TopPen>(value.value<QPen>());
        break;
    case BottomPen:
        newSubStyle = new BorderPenStyle<BottomPen>(value.value<QPen>());
        break;
    case FallDiagonalPen:
        newSubStyle = new BorderPenStyle<FallDiagonalPen>(value.value<QPen>());
        break;
    case GoUpDiagonalPen:
        newSubStyle = new BorderPenStyle<GoUpDiagonalPen>(value.value<QPen>());
        break;
        // layout
    case HorizontalAlignment:
        newSubStyle = new SubStyleOne<HorizontalAlignment, HAlign>((HAlign)value.value<int>());
        break;
    case VerticalAlignment:
        newSubStyle = new SubStyleOne<VerticalAlignment, VAlign>((VAlign)value.value<int>());
        break;
    case MultiRow:
        newSubStyle = new SubStyleOne<MultiRow, bool>(value.value<bool>());
        break;
    case VerticalText:
        newSubStyle = new SubStyleOne<VerticalText, bool>(value.value<bool>());
        break;
    case Angle:
        newSubStyle = new SubStyleOne<Angle, int>(value.value<int>());
        break;
    case Indentation:
        newSubStyle = new SubStyleOne<Indentation, int>(value.value<int>());
        break;
    case ShrinkToFit:
        newSubStyle = new SubStyleOne<ShrinkToFit,bool>(value.value<bool>());
        break;
        // content format
    case Prefix:
        newSubStyle = new SubStyleOne<Prefix, QString>(value.value<QString>());
        break;
    case Postfix:
        newSubStyle = new SubStyleOne<Postfix, QString>(value.value<QString>());
        break;
    case Precision:
        newSubStyle = new SubStyleOne<Precision, int>(value.value<int>());
        break;
    case ThousandsSep:
        newSubStyle = new SubStyleOne<ThousandsSep, bool>(value.value<bool>());
        break;
    case FormatTypeKey:
        newSubStyle = new SubStyleOne<FormatTypeKey, Format::Type>((Format::Type)value.value<int>());
        break;
    case FloatFormatKey:
        newSubStyle = new SubStyleOne<FloatFormatKey, FloatFormat>((FloatFormat)value.value<int>());
        break;
    case FloatColorKey:
        newSubStyle = new SubStyleOne<FloatColorKey, FloatColor>((FloatColor)value.value<int>());
        break;
    case CurrencyFormat:
        newSubStyle = new SubStyleOne<CurrencyFormat, Currency>(value.value<Currency>());
        break;
    case CustomFormat:
        newSubStyle = new SubStyleOne<CustomFormat, QString>(value.value<QString>());
        break;
        // background
    case BackgroundBrush:
        newSubStyle = new SubStyleOne<BackgroundBrush, QBrush>(value.value<QBrush>());
        break;
    case BackgroundColor:
        newSubStyle = new SubStyleOne<BackgroundColor, QColor>(value.value<QColor>());
        break;
        // font
    case FontColor:
        newSubStyle = new SubStyleOne<FontColor, QColor>(value.value<QColor>());
        break;
    case FontFamily:
        newSubStyle = new SubStyleOne<FontFamily, QString>(value.value<QString>());
        break;
    case FontSize:
        newSubStyle = new SubStyleOne<FontSize, int>(value.value<int>());
        break;
    case FontBold:
        newSubStyle = new SubStyleOne<FontBold, bool>(value.value<bool>());
        break;
    case FontItalic:
        newSubStyle = new SubStyleOne<FontItalic, bool>(value.value<bool>());
        break;
    case FontStrike:
        newSubStyle = new SubStyleOne<FontStrike, bool>(value.value<bool>());
        break;
    case FontUnderline:
        newSubStyle = new SubStyleOne<FontUnderline, bool>(value.value<bool>());
        break;
        //misc
    case DontPrintText:
        newSubStyle = new SubStyleOne<DontPrintText, bool>(value.value<bool>());
        break;
    case NotProtected:
        newSubStyle = new SubStyleOne<NotProtected, bool>(value.value<bool>());
        break;
    case HideAll:
        newSubStyle = new SubStyleOne<HideAll, bool>(value.value<bool>());
        break;
    case HideFormula:
        newSubStyle = new SubStyleOne<HideFormula, bool>(value.value<bool>());
        break;
    }
    return newSubStyle;
}

void Style::insertSubStyle(Key key, const QVariant& value)
{
    const SharedSubStyle subStyle = createSubStyle(key, value);
    Q_ASSERT(!!subStyle);
    insertSubStyle(subStyle);
}

void Style::insertSubStyle(const SharedSubStyle& subStyle)
{
    if (!subStyle)
        return;
    releaseSubStyle(subStyle->type());
    d->subStyles.insert(subStyle->type(), subStyle);
}

bool Style::releaseSubStyle(Key key)
{
    if (!d->subStyles.contains(key))
        return false;

    d->subStyles.remove(key);
    return true;
}

/////////////////////////////////////////////////////////////////////////////
//
// CustomStyle::Private
//
/////////////////////////////////////////////////////////////////////////////

class Q_DECL_HIDDEN CustomStyle::Private : public QSharedData
{
public:
    QString name;
    StyleType type;
};


/////////////////////////////////////////////////////////////////////////////
//
// CustomStyle
//
/////////////////////////////////////////////////////////////////////////////

CustomStyle::CustomStyle()
        : Style()
        , d(new Private)
{
    d->name = "Default";
    d->type = BUILTIN;
    setDefault();
}

CustomStyle::CustomStyle(QString const & name, CustomStyle * parent)
        : Style()
        , d(new Private)
{
    d->name = name;
    d->type = CUSTOM;
    if (parent)
        setParentName(parent->name());
}

CustomStyle::CustomStyle(const CustomStyle& style)
        : Style(style), d(style.d)
{
}

CustomStyle::~CustomStyle()
{
}

CustomStyle& CustomStyle::operator=(const CustomStyle& style)
{
	Style::operator=(style);
	d = style.d;
	return *this;
}

Style::StyleType CustomStyle::type() const
{
    return d->type;
}

void CustomStyle::setType(StyleType type)
{
    Q_ASSERT(type != AUTO);
    d->type = type;
}

const QString& CustomStyle::name() const
{
    return d->name;
}

void CustomStyle::setName(QString const & name)
{
    d->name = name;
}

void CustomStyle::save(QDomDocument& doc, QDomElement& styles, const StyleManager* styleManager)
{
    if (name().isEmpty())
        return;

    QDomElement style(doc.createElement("style"));
    style.setAttribute("type", QString::number((int) type()));
    if (!parentName().isNull())
        style.setAttribute("parent", parentName());
    style.setAttribute("name", name());

    QDomElement format(doc.createElement("format"));
    saveXML(doc, format, styleManager);
    style.appendChild(format);

    styles.appendChild(style);
}

bool CustomStyle::loadXML(KoXmlElement const & style, QString const & name)
{
    setName(name);

    if (style.hasAttribute("parent"))
        setParentName(style.attribute("parent"));

    if (!style.hasAttribute("type"))
        return false;

    bool ok = true;
    setType((StyleType) style.attribute("type").toInt(&ok));
    if (!ok)
        return false;

    KoXmlElement f(style.namedItem("format").toElement());
    if (!f.isNull())
        if (!Style::loadXML(f))
            return false;

    return true;
}

int CustomStyle::usage() const
{
    return d->ref;
}

QSet<Style::Key> CustomStyle::definedKeys(const StyleManager *) const
{
    QList<SharedSubStyle> subs = subStyles();
    QSet<Style::Key> keys;
    for (int i = 0; i < subs.count(); ++i)
        keys.insert(subs[i].data()->type());
    return keys;
}

QDebug operator<<(QDebug dbg, const Calligra::Sheets::Style *s)
{
    if (s) {
        dbg << (*s);
    } else {
        dbg << "Style[0x0]";
    }
    return dbg;
}

QDebug operator<<(QDebug dbg, const Calligra::Sheets::Style &s)
{
    static const char *types[] = {"Builtin", "Custom", "Auto", "Tentative"};
    dbg << "Style["<<types[s.type()]<<"]";
    return dbg;
}
