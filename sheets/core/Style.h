/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_STYLE
#define CALLIGRA_SHEETS_STYLE

#include <QBrush>
#include <QColor>
#include <QPen>
#include <QSharedDataPointer>

#include "engine/SheetsDebug.h"

#include "sheets_core_export.h"
#include "engine/Format.h"
#include "Currency.h"


static inline uint qHash(const QColor& color)
{ return uint(color.rgb()); }

static inline uint qHash(const QPen& pen)
{ return qHash(pen.color()) ^ 37 * uint(pen.style()); }

static inline uint qHash(const QBrush& brush)
{ return qHash(brush.color()) ^ 91 * uint(brush.style()); }

class QFont;
class QTextCharFormat;

namespace Calligra
{
namespace Sheets
{
class Conditions;
class CustomStyle;
class SharedSubStyle;
class Style;
class StyleManager;
class SubStyle;
class ValueParser;

// used for preloading OASIS auto styles
typedef QMap<QString, Style>       Styles;

CALLIGRA_SHEETS_CORE_EXPORT uint qHash(const Style& style);

/**
 * \ingroup Style
 * A cell style.
 */
class CALLIGRA_SHEETS_CORE_EXPORT Style
{
public:
    enum HAlign {
        Left = 1,
        Center = 2,
        Right = 3,
        Justified = 4,
        HAlignUndefined = 0
    };

    enum VAlign {
        Top = 1,
        Middle = 2,
        Bottom = 3,
        VDistributed = 4,
        VJustified = 5,
        VAlignUndefined = 0
    };

    enum FloatFormat {
        DefaultFloatFormat = 0,
        AlwaysSigned = 1,
        AlwaysUnsigned = 2,
        OnlyNegSigned = DefaultFloatFormat
    };

    enum FloatColor {
        DefaultFloatColor = 0,
        NegRed = 1,
        AllBlack = DefaultFloatColor,
        NegBrackets = 3,
        NegRedBrackets = 4
    };

    /// The style type
    enum StyleType {
        BUILTIN,   ///< built-in style (the default style)
        CUSTOM,    ///< custom style (defined in the StyleManager dialog)
        AUTO,      ///< automatically generated on cell format changes
        TENTATIVE  ///< @internal temporary state
    };

    enum Key {
        // special cases
        DefaultStyleKey,
        /**
         * The NamedStyleKey has two functions:
         * \li In Style: On lookup the Style is composed (\see StyleStorage::composeStyle()).
         *               For named styles just the style's name is stored. On lookup, these styles
         *               are looked up in the StyleManager and their attributes, including all
         *               attributes of the parents, are filled in the composed style. Additionally,
         *               the name of the named style is stored in the NamedStyleKey attribute.
         *               This is especially useful while saving the styles.
         * \li In CustomStyle: It simply refers to the parent style.
         */
        NamedStyleKey,
        // borders
        LeftPen,
        RightPen,
        TopPen,
        BottomPen,
        FallDiagonalPen,
        GoUpDiagonalPen,
        // layout
        HorizontalAlignment,
        VerticalAlignment,
        MultiRow,
        VerticalText,
        Angle,
        ShrinkToFit,
        Indentation,
        // content format
        Prefix,
        Postfix,
        Precision,
        ThousandsSep,
        FormatTypeKey,
        FloatFormatKey,
        FloatColorKey,
        CurrencyFormat,
        CustomFormat,
        // background
        BackgroundBrush,
        BackgroundColor,
        // font
        FontColor,
        FontFamily,
        FontSize,
        FontBold,
        FontItalic,
        FontStrike,
        FontUnderline,
        //misc
        DontPrintText,
        NotProtected,
        HideAll,
        HideFormula
    };

    Style();
    Style(const Style& style);
    virtual ~Style();

    virtual StyleType type() const;

    QString parentName() const;
    void setParentName(const QString& name);

    void clearAttribute(Key key);
    bool hasAttribute(Key key) const;


    uint bottomPenValue() const;
    uint rightPenValue() const;
    uint leftPenValue() const;
    uint topPenValue() const;

    QColor  fontColor()       const;
    QColor  backgroundColor() const;
    QPen    rightBorderPen()  const;
    QPen    bottomBorderPen() const;
    QPen    leftBorderPen()   const;
    QPen    topBorderPen()    const;
    QPen    fallDiagonalPen() const;
    QPen    goUpDiagonalPen() const;
    QBrush  backgroundBrush() const;
    QString customFormat()    const;
    QString prefix()          const;
    QString postfix()         const;
    QString fontFamily()      const;

    HAlign      halign()      const;
    VAlign      valign()      const;
    FloatFormat floatFormat() const;
    FloatColor  floatColor()  const;
    Format::Type  formatType()  const;

    Currency currency() const;

    QFont  font()         const;

    bool bold() const { return getBoolValue(FontBold); }
    bool italic() const { return getBoolValue(FontItalic); }
    bool underline() const { return getBoolValue(FontUnderline); }
    bool strikeOut() const { return getBoolValue(FontStrike); }

    uint   fontFlags()    const;
    int    fontSize()     const;
    int    precision()    const;
    bool   thousandsSep() const { return getBoolValue(ThousandsSep); }
    int    angle()        const;
    double indentation()  const;
    bool   shrinkToFit()  const { return getBoolValue(ShrinkToFit); }
    bool   verticalText() const { return getBoolValue(VerticalText); }
    bool   wrapText()     const { return getBoolValue(MultiRow); }
    bool   printText()    const { return !getBoolValue(DontPrintText); }
    bool   hideAll()      const { return getBoolValue(HideAll); }
    bool   hideFormula()  const { return getBoolValue(HideFormula); }
    bool   notProtected() const { return getBoolValue(NotProtected); }

    bool   isDefault()    const;
    bool   isEmpty()      const;

    bool getBoolValue(Key key) const;

public:
    void setFont(QFont const & font);
    void setFontFamily(QString const & fam);
    void setCurrency(Currency const & currency);

    void setHAlign(HAlign align) { insertSubStyle(HorizontalAlignment, align); }
    void setVAlign(VAlign align) { insertSubStyle(VerticalAlignment, align); }
    void setFontBold(bool enable) { insertSubStyle(FontBold, enable); }
    void setFontItalic(bool enable) { insertSubStyle(FontItalic, enable); }
    void setFontUnderline(bool enable) { insertSubStyle(FontUnderline, enable); }
    void setFontStrikeOut(bool enable) { insertSubStyle(FontStrike, enable); }
    void setFontSize(int size) { insertSubStyle(FontSize, size); }
    void setFontColor(QColor const & color) { insertSubStyle(FontColor, color); }
    void setRightBorderPen(QPen const & pen) { insertSubStyle(RightPen, pen); }
    void setBottomBorderPen(QPen const & pen) { insertSubStyle(BottomPen, pen); }
    void setLeftBorderPen(QPen const & pen) { insertSubStyle(LeftPen, pen); }
    void setTopBorderPen(QPen const & pen) { insertSubStyle(TopPen, pen); }
    void setFallDiagonalPen(QPen const & pen) { insertSubStyle(FallDiagonalPen, pen); }
    void setGoUpDiagonalPen(QPen const & pen) { insertSubStyle(GoUpDiagonalPen, pen); }
    void setAngle(int angle) { insertSubStyle(Angle, angle); }
    void setIndentation(double indent) { insertSubStyle(Indentation, indent); }
    void setBackgroundBrush(QBrush const & brush) { insertSubStyle(BackgroundBrush, brush); }
    void setFloatFormat(FloatFormat format) { insertSubStyle(FloatFormatKey, format); }
    void setFloatColor(FloatColor color) { insertSubStyle(FloatColorKey, color); }
    void setFormatType(Format::Type format) { insertSubStyle(FormatTypeKey, format); }
    void setCustomFormat(QString const & strFormat) { insertSubStyle(CustomFormat, strFormat); }
    void setPrecision(int precision) { insertSubStyle(Precision, precision); }
    void setThousandsSep(bool thousandsSep) { insertSubStyle(ThousandsSep, thousandsSep); }
    void setPrefix(QString const & prefix) { insertSubStyle(Prefix, prefix); }
    void setPostfix(QString const & postfix) { insertSubStyle(Postfix, postfix); }
    void setWrapText(bool enable) { insertSubStyle(MultiRow, enable); }
    void setHideAll(bool enable) { insertSubStyle(HideAll, enable); }
    void setHideFormula(bool enable) { insertSubStyle(HideFormula, enable); }
    void setNotProtected(bool enable) { insertSubStyle(NotProtected, enable); }
    void setDontPrintText(bool enable) { insertSubStyle(DontPrintText, enable); }
    void setVerticalText(bool enable) { insertSubStyle(VerticalText, enable); }
    void setShrinkToFit(bool enable) {  insertSubStyle(ShrinkToFit, enable); }
    void setBackgroundColor(QColor const & color) { insertSubStyle(BackgroundColor, color); }

    void setDefault();
    void clear();

    static bool compare(const SubStyle* one, const SubStyle* two);


    /** Returns true if both styles have the same properties */
    bool operator== (const Style& style) const;
    inline bool operator!=(const Style& other) const {
        return !operator==(other);
    }
    friend CALLIGRA_SHEETS_CORE_EXPORT uint qHash(const Style& style);
    void operator=(const Style& style);
    Style operator-(const Style& style) const;
    /// Insert and replace substyles from style
    void merge(const Style& style);

    /**
     * The keys, that are contained in this style, but not in \p other and
     * the keys, that are contained in both but differ in value.
     * \return a set of keys, in which this style and \p other differ.
     */
    QSet<Key> difference(const Style& other) const;

    void dump() const;

    /**
     * Return the properties of this style that can be represented as a QTextCharFormat
     */
    QTextCharFormat asCharFormat() const;

    /** Defined style elements - used when saving the style */
    virtual QSet<Style::Key> definedKeys(const StyleManager *) const;

    virtual void insertSubStyle(Key key, const QVariant& value);

protected:
    QList<SharedSubStyle> subStyles() const;

    SharedSubStyle createSubStyle(Key key, const QVariant& value);
    void insertSubStyle(const SharedSubStyle& subStyle);
    bool releaseSubStyle(Key key);

private:
    friend class StyleStorage;
    friend class StyleStorageLoaderJob;

    class Private;
    QSharedDataPointer<Private> d;
};



/**
 * \ingroup Style
 * A named cell style.
 */
class CALLIGRA_SHEETS_CORE_EXPORT CustomStyle : public Style
{
public:
    /**
     * Constructor.
     * Creates a custom style.
     * \param name The name of this style.
     * \param parent The style whose attributes are inherited - the parent style.
     */
    explicit CustomStyle(const QString& name, CustomStyle* parent = 0);
	CustomStyle(const CustomStyle& style);
    ~CustomStyle() override;
    CustomStyle& operator=(const CustomStyle& other);

    StyleType type() const override;
    void setType(StyleType type);

    void setName(QString const & name);
    QString const & name() const;

    //bool operator==(const CustomStyle& other) const;
    //inline bool operator!=(const CustomStyle& other) const {
    //    return !operator==(other);
    //}

    /**
     * @return the number of references to this style.
     */
    int usage() const;

    QSet<Style::Key> definedKeys(const StyleManager *) const override;
private:
    friend class StyleManager;

    /**
     * Constructor.
     * Constructs the default cell style.
     */
    CustomStyle();

    class Private;
    QSharedDataPointer<Private> d;
};


/**
 * \ingroup Style
 * A single style attribute.
 */
class CALLIGRA_SHEETS_CORE_TEST_EXPORT SubStyle : public QSharedData
{
public:
    SubStyle() {}
    virtual ~SubStyle() {}
    virtual Style::Key type() const {
        return Style::DefaultStyleKey;
    }
    virtual void dump() const {
        debugSheetsStyle << debugData();
    }
    virtual QString debugData(bool withName = true) const {
        QString out; if (withName) out = name(Style::DefaultStyleKey); return out;
    }
    virtual uint koHash() const { return uint(type()); }
    static QString name(Style::Key key);
};

// Provides a default SubStyle for the tree.
// Otherwise, we would have QSharedDataPointer<SubStyle>() as default,
// which has a null pointer and crashes.
// Also, this makes the code more readable:
// QSharedDataPointer<SubStyle> vs. SharedSubStyle
class SharedSubStyle
{
public:
    inline SharedSubStyle() : d(s_defaultStyle.d){}
    inline SharedSubStyle(SubStyle* subStyle) : d(subStyle) {}
    inline const SubStyle *operator->() const {
        return d.data();
    }
    inline const SubStyle *data() const {
        return d.data();
    }
    inline bool operator<(const SharedSubStyle& o) const {
        return d.data() < o.d.data();
    }
    inline bool operator==(const SharedSubStyle& o) const {
        return d.data() == o.d.data();
    }
    inline bool operator!() const {
        return !d;
    }

private:
    QSharedDataPointer<SubStyle> d;
    static SharedSubStyle s_defaultStyle;
};

class NamedStyle : public SubStyle
{
public:
    NamedStyle(const QString& n) : SubStyle(), name(n) {}
    Style::Key type() const override {
        return Style::NamedStyleKey;
    }
    void dump() const override {
        debugSheetsStyle << debugData();
    }
    QString debugData(bool withName = true) const override {
        QString out; if (withName) out = SubStyle::name(Style::NamedStyleKey) + ' '; out += name; return out;
    }
    uint koHash() const override { return uint(type()) ^ qHash(name); }
    QString name;
};

template<class Value1>
class SubStyleOne : public SubStyle
{
public:
    SubStyleOne(Style::Key type, const Value1& v = Value1()) : SubStyle(), value1(v), m_type(type) {}
    Style::Key type() const override {
        return m_type;
    }
    void dump() const override {
        debugSheetsStyle << debugData();
    }
    QString debugData(bool withName = true) const override {
        QString out; if (withName) out = name(m_type) + ' '; QDebug qdbg(&out); qdbg << value1; return out;
    }
    uint koHash() const override { return uint(type()) ^ ::qHash(value1); }
    Value1 value1;
private:
    Style::Key m_type;
};

} // namespace Sheets
} // namespace Calligra

Q_DECLARE_TYPEINFO(Calligra::Sheets::Style, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(Calligra::Sheets::CustomStyle, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(Calligra::Sheets::SharedSubStyle, Q_MOVABLE_TYPE);

CALLIGRA_SHEETS_CORE_TEST_EXPORT QDebug operator<<( QDebug dbg, const Calligra::Sheets::Style *style );
CALLIGRA_SHEETS_CORE_TEST_EXPORT QDebug operator<<( QDebug dbg, const Calligra::Sheets::Style &style );

#endif // CALLIGRA_SHEETS_STYLE
