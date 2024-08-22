/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "Style.h"

#include <QFontDatabase>
#include <QTextCharFormat>

#include <KoGlobal.h>

#include "Currency.h"
#include "StyleManager.h"

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

static uint calculateValue(QPen const &pen)
{
    uint n = pen.color().red() + pen.color().green() + pen.color().blue();
    n += 1000 * pen.width();
    n += 10000 * (uint)pen.style();
    return n;
}

// specialized debug method
template<>
QString SubStyleOne<Currency>::debugData(bool withName) const
{
    QString out;
    if (withName)
        out = name(m_type) + ' ';
    QDebug qdbg(&out);
    qdbg << value1.symbol();
    return out;
}

class PenStyle : public SubStyleOne<QPen>
{
public:
    PenStyle(Style::Key type, const QPen &p = Qt::NoPen)
        : SubStyleOne<QPen>(type, p)
    {
    }
};

class BorderPenStyle : public PenStyle
{
public:
    BorderPenStyle(Style::Key key, const QPen &p = Qt::NoPen)
        : PenStyle(key, p)
        , value(calculateValue(p))
    {
    }
    int value;
};

QString SubStyle::name(Style::Key key)
{
    QString name;
    switch (key) {
    case Style::DefaultStyleKey:
        name = "Default style";
        break;
    case Style::NamedStyleKey:
        name = "Named style";
        break;
    case Style::LeftPen:
        name = "Left pen";
        break;
    case Style::RightPen:
        name = "Right pen";
        break;
    case Style::TopPen:
        name = "Top pen";
        break;
    case Style::BottomPen:
        name = "Bottom pen";
        break;
    case Style::FallDiagonalPen:
        name = "Fall diagonal pen";
        break;
    case Style::GoUpDiagonalPen:
        name = "Go up diagonal pen";
        break;
    case Style::HorizontalAlignment:
        name = "Horz. alignment";
        break;
    case Style::VerticalAlignment:
        name = "Vert. alignment";
        break;
    case Style::MultiRow:
        name = "Wrap text";
        break;
    case Style::VerticalText:
        name = "Vertical text";
        break;
    case Style::Angle:
        name = "Angle";
        break;
    case Style::Indentation:
        name = "Indentation";
        break;
    case Style::ShrinkToFit:
        name = "Shrink to Fit";
        break;
    case Style::Prefix:
        name = "Prefix";
        break;
    case Style::Postfix:
        name = "Postfix";
        break;
    case Style::Precision:
        name = "Precision";
        break;
    case Style::ThousandsSep:
        name = "Thousands separator";
        break;
    case Style::FormatTypeKey:
        name = "Format type";
        break;
    case Style::FloatFormatKey:
        name = "Float format";
        break;
    case Style::FloatColorKey:
        name = "Float color";
        break;
    case Style::CurrencyFormat:
        name = "Currency";
        break;
    case Style::CustomFormat:
        name = "Custom format";
        break;
    case Style::BackgroundBrush:
        name = "Background brush";
        break;
    case Style::BackgroundColor:
        name = "Background color";
        break;
    case Style::FontColor:
        name = "Font color";
        break;
    case Style::FontFamily:
        name = "Font family";
        break;
    case Style::FontSize:
        name = "Font size";
        break;
    case Style::FontBold:
        name = "Font bold";
        break;
    case Style::FontItalic:
        name = "Font italic";
        break;
    case Style::FontStrike:
        name = "Font strikeout";
        break;
    case Style::FontUnderline:
        name = "Font underline";
        break;
    case Style::DontPrintText:
        name = "Do not print text";
        break;
    case Style::NotProtected:
        name = "Not protected";
        break;
    case Style::HideAll:
        name = "Hide all";
        break;
    case Style::HideFormula:
        name = "Hide formula";
        break;
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
    QMap<Key, SharedSubStyle> subStyles;
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

Style::Style(const Style &style)

    = default;

Style::~Style() = default;

Style::StyleType Style::type() const
{
    return AUTO;
}

QString Style::parentName() const
{
    if (!d->subStyles.contains(NamedStyleKey))
        return QString();
    return static_cast<const NamedStyle *>(d->subStyles[NamedStyleKey].data())->name;
}

void Style::setParentName(const QString &name)
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

bool Style::getBoolValue(Key key) const
{
    if (!d->subStyles.contains(key))
        return SubStyleOne<bool>(key).value1;
    return static_cast<const SubStyleOne<bool> *>(d->subStyles[key].data())->value1;
}

uint Style::bottomPenValue() const
{
    if (!d->subStyles.contains(BottomPen))
        return BorderPenStyle(BottomPen).value;
    return static_cast<const BorderPenStyle *>(d->subStyles[BottomPen].data())->value;
}

uint Style::rightPenValue() const
{
    if (!d->subStyles.contains(RightPen))
        return BorderPenStyle(RightPen).value;
    return static_cast<const BorderPenStyle *>(d->subStyles[RightPen].data())->value;
}

uint Style::leftPenValue() const
{
    if (!d->subStyles.contains(LeftPen))
        return BorderPenStyle(LeftPen).value;
    return static_cast<const BorderPenStyle *>(d->subStyles[LeftPen].data())->value;
}

uint Style::topPenValue() const
{
    if (!d->subStyles.contains(TopPen))
        return BorderPenStyle(TopPen).value;
    return static_cast<const BorderPenStyle *>(d->subStyles[TopPen].data())->value;
}

QPen Style::rightBorderPen() const
{
    if (!d->subStyles.contains(RightPen))
        return BorderPenStyle(RightPen).value1;
    return static_cast<const BorderPenStyle *>(d->subStyles[RightPen].data())->value1;
}

QPen Style::bottomBorderPen() const
{
    if (!d->subStyles.contains(BottomPen))
        return BorderPenStyle(BottomPen).value1;
    return static_cast<const BorderPenStyle *>(d->subStyles[BottomPen].data())->value1;
}

QPen Style::leftBorderPen() const
{
    if (!d->subStyles.contains(LeftPen))
        return BorderPenStyle(LeftPen).value1;
    return static_cast<const BorderPenStyle *>(d->subStyles[LeftPen].data())->value1;
}

QPen Style::topBorderPen() const
{
    if (!d->subStyles.contains(TopPen))
        return BorderPenStyle(TopPen).value1;
    return static_cast<const BorderPenStyle *>(d->subStyles[TopPen].data())->value1;
}

QPen Style::fallDiagonalPen() const
{
    if (!d->subStyles.contains(FallDiagonalPen))
        return BorderPenStyle(FallDiagonalPen).value1;
    return static_cast<const BorderPenStyle *>(d->subStyles[FallDiagonalPen].data())->value1;
}

QPen Style::goUpDiagonalPen() const
{
    if (!d->subStyles.contains(GoUpDiagonalPen))
        return BorderPenStyle(GoUpDiagonalPen).value1;
    return static_cast<const BorderPenStyle *>(d->subStyles[GoUpDiagonalPen].data())->value1;
}

QColor Style::fontColor() const
{
    if (!d->subStyles.contains(FontColor))
        return SubStyleOne<QColor>(FontColor, Qt::black).value1;
    return static_cast<const SubStyleOne<QColor> *>(d->subStyles[FontColor].data())->value1;
}

QColor Style::backgroundColor() const
{
    if (!d->subStyles.contains(BackgroundColor))
        return SubStyleOne<QColor>(BackgroundColor).value1;
    return static_cast<const SubStyleOne<QColor> *>(d->subStyles[BackgroundColor].data())->value1;
}

QBrush Style::backgroundBrush() const
{
    if (!d->subStyles.contains(BackgroundBrush))
        return SubStyleOne<QBrush>(BackgroundBrush, Qt::white).value1;
    return static_cast<const SubStyleOne<QBrush> *>(d->subStyles[BackgroundBrush].data())->value1;
}

QString Style::customFormat() const
{
    if (!d->subStyles.contains(CustomFormat))
        return SubStyleOne<QString>(CustomFormat).value1;
    return static_cast<const SubStyleOne<QString> *>(d->subStyles[CustomFormat].data())->value1;
}

QString Style::prefix() const
{
    if (!d->subStyles.contains(Prefix))
        return SubStyleOne<QString>(Prefix).value1;
    return static_cast<const SubStyleOne<QString> *>(d->subStyles[Prefix].data())->value1;
}

QString Style::postfix() const
{
    if (!d->subStyles.contains(Postfix))
        return SubStyleOne<QString>(Postfix).value1;
    return static_cast<const SubStyleOne<QString> *>(d->subStyles[Postfix].data())->value1;
}

QString Style::fontFamily() const
{
    if (!d->subStyles.contains(FontFamily))
        return KoGlobal::defaultFont().family(); // SubStyleOne<FontFamily, QString>().value1;
    return static_cast<const SubStyleOne<QString> *>(d->subStyles[FontFamily].data())->value1;
}

Style::HAlign Style::halign() const
{
    if (!d->subStyles.contains(HorizontalAlignment))
        return SubStyleOne<Style::HAlign>(HorizontalAlignment).value1;
    return static_cast<const SubStyleOne<Style::HAlign> *>(d->subStyles[HorizontalAlignment].data())->value1;
}

Style::VAlign Style::valign() const
{
    if (!d->subStyles.contains(VerticalAlignment))
        return SubStyleOne<Style::VAlign>(VerticalAlignment).value1;
    return static_cast<const SubStyleOne<Style::VAlign> *>(d->subStyles[VerticalAlignment].data())->value1;
}

Style::FloatFormat Style::floatFormat() const
{
    if (!d->subStyles.contains(FloatFormatKey))
        return SubStyleOne<FloatFormat>(FloatFormatKey).value1;
    return static_cast<const SubStyleOne<FloatFormat> *>(d->subStyles[FloatFormatKey].data())->value1;
}

Style::FloatColor Style::floatColor() const
{
    if (!d->subStyles.contains(FloatColorKey))
        return SubStyleOne<FloatColor>(FloatColorKey).value1;
    return static_cast<const SubStyleOne<FloatColor> *>(d->subStyles[FloatColorKey].data())->value1;
}

Format::Type Style::formatType() const
{
    if (!d->subStyles.contains(FormatTypeKey))
        return SubStyleOne<Format::Type>(FormatTypeKey).value1;
    return static_cast<const SubStyleOne<Format::Type> *>(d->subStyles[FormatTypeKey].data())->value1;
}

Currency Style::currency() const
{
    if (!d->subStyles.contains(CurrencyFormat))
        return Currency();
    return static_cast<const SubStyleOne<Currency> *>(d->subStyles[CurrencyFormat].data())->value1;
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

int Style::fontSize() const
{
    if (!d->subStyles.contains(FontSize))
        return KoGlobal::defaultFont().pointSize(); // SubStyleOne<int>(FontSize).value1;
    return static_cast<const SubStyleOne<int> *>(d->subStyles[FontSize].data())->value1;
}

int Style::precision() const
{
    if (!d->subStyles.contains(Precision))
        return -1; // SubStyleOne<int>(Precision).value1;
    return static_cast<const SubStyleOne<int> *>(d->subStyles[Precision].data())->value1;
}

int Style::angle() const
{
    if (!d->subStyles.contains(Angle))
        return SubStyleOne<int>(Angle).value1;
    return static_cast<const SubStyleOne<int> *>(d->subStyles[Angle].data())->value1;
}

double Style::indentation() const
{
    if (!d->subStyles.contains(Indentation))
        return SubStyleOne<int>(Indentation).value1;
    return static_cast<const SubStyleOne<int> *>(d->subStyles[Indentation].data())->value1;
}

bool Style::isDefault() const
{
    return isEmpty() || d->subStyles.contains(DefaultStyleKey);
}

bool Style::isEmpty() const
{
    return d->subStyles.isEmpty();
}

void Style::setFont(QFont const &font)
{
    insertSubStyle(FontFamily, font.family());
    insertSubStyle(FontSize, font.pointSize());
    insertSubStyle(FontBold, font.bold());
    insertSubStyle(FontItalic, font.italic());
    insertSubStyle(FontStrike, font.strikeOut());
    insertSubStyle(FontUnderline, font.underline());
}

void Style::setFontFamily(QString const &family)
{
    QString font = family;
    // use the KDE default for sans serif, not Qt's default - this is because Qt doesn't return the default system font here
    if (font.toLower() == "sans serif") {
        QFont f = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
        font = f.family();
    }

    insertSubStyle(FontFamily, font);
}

void Style::setCurrency(Currency const &currency)
{
    QVariant variant;
    variant.setValue(currency);
    insertSubStyle(CurrencyFormat, variant);
}

void Style::setDefault()
{
    insertSubStyle(DefaultStyleKey, true);
}

void Style::clear()
{
    d->subStyles.clear();
}

bool Style::compare(const SubStyle *one, const SubStyle *two)
{
    if (!one || !two)
        return one == two;
    if (one->type() != two->type())
        return false;
    switch (one->type()) {
    case DefaultStyleKey:
        return true;
    case NamedStyleKey:
        return static_cast<const NamedStyle *>(one)->name == static_cast<const NamedStyle *>(two)->name;
        // borders
    case LeftPen:
    case RightPen:
    case TopPen:
    case BottomPen:
    case FallDiagonalPen:
    case GoUpDiagonalPen:
        return static_cast<const SubStyleOne<QPen> *>(one)->value1 == static_cast<const SubStyleOne<QPen> *>(two)->value1;
        // layout
    case HorizontalAlignment:
        return static_cast<const SubStyleOne<HAlign> *>(one)->value1 == static_cast<const SubStyleOne<HAlign> *>(two)->value1;
    case VerticalAlignment:
        return static_cast<const SubStyleOne<VAlign> *>(one)->value1 == static_cast<const SubStyleOne<VAlign> *>(two)->value1;
    case MultiRow:
    case VerticalText:
    case ShrinkToFit:
        return static_cast<const SubStyleOne<bool> *>(one)->value1 == static_cast<const SubStyleOne<bool> *>(two)->value1;
    case Angle:
        return static_cast<const SubStyleOne<int> *>(one)->value1 == static_cast<const SubStyleOne<int> *>(two)->value1;
    case Indentation:
        return static_cast<const SubStyleOne<int> *>(one)->value1 == static_cast<const SubStyleOne<int> *>(two)->value1;
        // content format
    case Prefix:
        return static_cast<const SubStyleOne<QString> *>(one)->value1 == static_cast<const SubStyleOne<QString> *>(two)->value1;
    case Postfix:
        return static_cast<const SubStyleOne<QString> *>(one)->value1 == static_cast<const SubStyleOne<QString> *>(two)->value1;
    case Precision:
        return static_cast<const SubStyleOne<int> *>(one)->value1 == static_cast<const SubStyleOne<int> *>(two)->value1;
    case ThousandsSep:
        return static_cast<const SubStyleOne<bool> *>(one)->value1 == static_cast<const SubStyleOne<bool> *>(two)->value1;
    case FormatTypeKey:
        return static_cast<const SubStyleOne<Format::Type> *>(one)->value1 == static_cast<const SubStyleOne<Format::Type> *>(two)->value1;
    case FloatFormatKey:
        return static_cast<const SubStyleOne<FloatFormat> *>(one)->value1 == static_cast<const SubStyleOne<FloatFormat> *>(two)->value1;
    case FloatColorKey:
        return static_cast<const SubStyleOne<FloatColor> *>(one)->value1 == static_cast<const SubStyleOne<FloatColor> *>(two)->value1;
    case CurrencyFormat: {
        Currency currencyOne = static_cast<const SubStyleOne<Currency> *>(one)->value1;
        Currency currencyTwo = static_cast<const SubStyleOne<Currency> *>(two)->value1;
        if (currencyOne != currencyTwo)
            return false;
        return true;
    }
    case CustomFormat:
        return static_cast<const SubStyleOne<QString> *>(one)->value1 == static_cast<const SubStyleOne<QString> *>(two)->value1;
        // background
    case BackgroundBrush:
        return static_cast<const SubStyleOne<QBrush> *>(one)->value1 == static_cast<const SubStyleOne<QBrush> *>(two)->value1;
    case BackgroundColor:
        return static_cast<const SubStyleOne<QColor> *>(one)->value1 == static_cast<const SubStyleOne<QColor> *>(two)->value1;
        // font
    case FontColor:
        return static_cast<const SubStyleOne<QColor> *>(one)->value1 == static_cast<const SubStyleOne<QColor> *>(two)->value1;
    case FontFamily:
        return static_cast<const SubStyleOne<QString> *>(one)->value1 == static_cast<const SubStyleOne<QString> *>(two)->value1;
    case FontSize:
        return static_cast<const SubStyleOne<int> *>(one)->value1 == static_cast<const SubStyleOne<int> *>(two)->value1;
    case FontBold:
    case FontItalic:
    case FontStrike:
    case FontUnderline:
    case DontPrintText:
    case NotProtected:
    case HideAll:
    case HideFormula:
        return static_cast<const SubStyleOne<bool> *>(one)->value1 == static_cast<const SubStyleOne<bool> *>(two)->value1;
    default:
        return false;
    }
}

bool Style::operator==(const Style &other) const
{
    if (other.isEmpty())
        return isEmpty() ? true : false;
    const auto list = d->subStyles.keys() + other.d->subStyles.keys();
    const QSet<Key> keys = QSet<Key>(list.begin(), list.end());
    const QSet<Key>::ConstIterator end = keys.constEnd();
    for (QSet<Key>::ConstIterator it = keys.constBegin(); it != end; ++it) {
        if (!compare(d->subStyles.value(*it).data(), other.d->subStyles.value(*it).data()))
            return false;
    }
    return true;
}

size_t Calligra::Sheets::qHash(const Style &style, size_t seed)
{
    size_t hash = 0;
    for (const SharedSubStyle &ss : style.subStyles()) {
        hash ^= ss->koHash(seed);
    }
    return hash;
}

void Style::operator=(const Style &other)
{
    d = other.d;
}

Style Style::operator-(const Style &other) const
{
    Style style;
    const QSet<Key> keys = difference(other);
    const QSet<Key>::ConstIterator end = keys.constEnd();
    for (QSet<Key>::ConstIterator it = keys.constBegin(); it != end; ++it)
        style.insertSubStyle(d->subStyles[*it]);
    return style;
}

void Style::merge(const Style &style)
{
    const QList<SharedSubStyle> subStyles(style.subStyles());
    //     debugSheetsStyle <<"merging" << subStyles.count() <<" attributes.";
    for (int i = 0; i < subStyles.count(); ++i) {
        //         debugSheetsStyle << subStyles[i]->debugData();
        insertSubStyle(subStyles[i]);
    }
}

QSet<Style::Key> Style::difference(const Style &other) const
{
    QSet<Key> result;
    const auto list = d->subStyles.keys() + other.d->subStyles.keys();
    const QSet<Key> keys = QSet<Key>(list.begin(), list.end());
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

    if (isDefault())
        return keys;

    if (hasAttribute(Style::NamedStyleKey)) {
        // it's not really the parent name in this case
        CustomStyle *namedStyle = styles->style(parentName());
        // remove substyles already present in named style
        if (namedStyle)
            keys = difference(*namedStyle);
    } else {
        const auto k = d->subStyles.keys();
        keys = QSet<Style::Key>(k.begin(), k.end());
    }

    return keys;
}

QList<SharedSubStyle> Style::subStyles() const
{
    return d->subStyles.values();
}

SharedSubStyle Style::createSubStyle(Key key, const QVariant &value)
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
    case RightPen:
    case TopPen:
    case BottomPen:
    case FallDiagonalPen:
    case GoUpDiagonalPen:
        newSubStyle = new BorderPenStyle(key, value.value<QPen>());
        break;
    case MultiRow:
    case VerticalText:
    case ShrinkToFit:
    case ThousandsSep:
    case FontBold:
    case FontItalic:
    case FontStrike:
    case FontUnderline:
    case DontPrintText:
    case NotProtected:
    case HideAll:
    case HideFormula:
        newSubStyle = new SubStyleOne<bool>(key, value.value<bool>());
        break;
    case Angle:
    case Indentation:
    case Precision:
    case FontSize:
        newSubStyle = new SubStyleOne<int>(key, value.value<int>());
        break;
    case Prefix:
    case Postfix:
    case CustomFormat:
    case FontFamily:
        newSubStyle = new SubStyleOne<QString>(key, value.value<QString>());
        break;
    case HorizontalAlignment:
        newSubStyle = new SubStyleOne<HAlign>(key, (HAlign)value.value<int>());
        break;
    case VerticalAlignment:
        newSubStyle = new SubStyleOne<VAlign>(key, (VAlign)value.value<int>());
        break;
    case FormatTypeKey:
        newSubStyle = new SubStyleOne<Format::Type>(key, (Format::Type)value.value<int>());
        break;
    case FloatFormatKey:
        newSubStyle = new SubStyleOne<FloatFormat>(key, (FloatFormat)value.value<int>());
        break;
    case FloatColorKey:
        newSubStyle = new SubStyleOne<FloatColor>(key, (FloatColor)value.value<int>());
        break;
    case CurrencyFormat:
        newSubStyle = new SubStyleOne<Currency>(key, value.value<Currency>());
        break;
    case BackgroundBrush:
        newSubStyle = new SubStyleOne<QBrush>(key, value.value<QBrush>());
        break;
    case BackgroundColor:
    case FontColor:
        newSubStyle = new SubStyleOne<QColor>(key, value.value<QColor>());
        break;
    }
    return newSubStyle;
}

void Style::insertSubStyle(Key key, const QVariant &value)
{
    const SharedSubStyle subStyle = createSubStyle(key, value);
    Q_ASSERT(!!subStyle);
    insertSubStyle(subStyle);
}

void Style::insertSubStyle(const SharedSubStyle &subStyle)
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

QString Style::debugData() const
{
    QString s;
    for (SharedSubStyle &st : subStyles())
        s += st->debugData() + "; ";
    return s;
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

CustomStyle::CustomStyle(QString const &name, CustomStyle *parent)
    : Style()
    , d(new Private)
{
    d->name = name;
    d->type = CUSTOM;
    if (parent)
        setParentName(parent->name());
}

CustomStyle::CustomStyle(const CustomStyle &style)

    = default;

CustomStyle::~CustomStyle() = default;

CustomStyle &CustomStyle::operator=(const CustomStyle &style) = default;

Style::StyleType CustomStyle::type() const
{
    return d->type;
}

void CustomStyle::setType(StyleType type)
{
    Q_ASSERT(type != AUTO);
    d->type = type;
}

const QString &CustomStyle::name() const
{
    return d->name;
}

void CustomStyle::setName(QString const &name)
{
    d->name = name;
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
    dbg << "Style[" << types[s.type()];
    dbg << s.debugData();
    dbg << "]";
    return dbg;
}
