/* This file is part of the KDE project
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

#ifndef KSPREAD_STYLE
#define KSPREAD_STYLE

#include <QFont>
#include <QSharedDataPointer>
#include <QTextCharFormat>

#include <KoXmlReader.h>

#include "Currency.h"
#include "Format.h"
#include "Global.h"

class KoGenStyle;
class KoGenStyles;
class KoOdfStylesReader;
class KoStyleStack;

namespace KSpread
{
class Conditions;
class CustomStyle;
class SharedSubStyle;
class Style;
class StyleManager;
class StyleManipulator;
class SubStyle;

// used for preloading OASIS auto styles
typedef QHash<QString, Style>       Styles;
// needs to be ordered (QMap) for the style dialog
typedef QMap<QString, CustomStyle*> CustomStyles;


/**
 * \ingroup Style
 * A cell style.
 */
class KSPREAD_EXPORT Style
{
public:
    enum HAlign {
        Left = 1,
        Center = 2,
        Right = 3,
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


    bool loadXML(KoXmlElement& format, Paste::Mode pm = Paste::Normal);
    void saveXML(QDomDocument& doc, QDomElement& format, const StyleManager* styleManager) const;
    void loadOdfStyle(KoOdfStylesReader& stylesReader, const KoXmlElement& element,
                      Conditions& conditions, const StyleManager* styleManager);
    /**
     * Saves an OASIS automatic style.
     * Reimplemented by CustomStyle for OASIS user styles.
     * \return the OASIS style's name
     */
    virtual QString saveOdf(KoGenStyle& style, KoGenStyles& mainStyles,
                            const StyleManager* manager) const;


    void clearAttribute(Key key);
    bool hasAttribute(Key key) const;
    void loadAttributes(const QList<SharedSubStyle>& subStyles);


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
    bool   bold()         const;
    bool   italic()       const;
    bool   underline()    const;
    bool   strikeOut()    const;
    uint   fontFlags()    const;
    int    fontSize()     const;
    int    precision()    const;
    int    angle()        const;
    double indentation()  const;
    bool   shrinkToFit()  const;
    bool   verticalText() const;
    bool   wrapText()     const;
    bool   printText()    const;
    bool   hideAll()      const;
    bool   hideFormula()  const;
    bool   notProtected() const;
    bool   isDefault()    const;
    bool   isEmpty()      const;

protected:
    /**
     * Helper function for saveOdf
     * Does the real work by determining the used attributes.
     */
    void saveOdfStyle(const QSet<Key>& subStyles, KoGenStyle &style,
                      KoGenStyles &mainStyles, const StyleManager* manager) const;

    void loadOdfDataStyle(KoOdfStylesReader& stylesReader, const KoXmlElement& element, Conditions& conditions, const StyleManager* styleManager);
    void loadOdfParagraphProperties(KoOdfStylesReader& stylesReader, const KoStyleStack& element);
    void loadOdfTableCellProperties(KoOdfStylesReader& stylesReader, const KoStyleStack& element);
    void loadOdfTextProperties(KoOdfStylesReader& stylesReader, const KoStyleStack& element);

public:
    void setHAlign(HAlign align);
    void setVAlign(VAlign align);
    void setFont(QFont const & font);
    void setFontFamily(QString const & fam);
    void setFontBold(bool enable);
    void setFontItalic(bool enable);
    void setFontUnderline(bool enable);
    void setFontStrikeOut(bool enable);
    void setFontSize(int size);
    void setFontColor(QColor const & color);
    void setRightBorderPen(QPen const & pen);
    void setBottomBorderPen(QPen const & pen);
    void setLeftBorderPen(QPen const & pen);
    void setTopBorderPen(QPen const & pen);
    void setFallDiagonalPen(QPen const & pen);
    void setGoUpDiagonalPen(QPen const & pen);
    void setAngle(int angle);
    void setIndentation(double indent);
    void setBackgroundBrush(QBrush const & brush);
    void setFloatFormat(FloatFormat format);
    void setFloatColor(FloatColor color);
    void setFormatType(Format::Type format);
    void setCustomFormat(QString const & strFormat);
    void setPrecision(int precision);
    void setPrefix(QString const & prefix);
    void setPostfix(QString const & postfix);
    void setCurrency(Currency const & currency);
    void setWrapText(bool enable);
    void setHideAll(bool enable);
    void setHideFormula(bool enable);
    void setNotProtected(bool enable);
    void setDontPrintText(bool enable);
    void setVerticalText(bool enable);
    void setShrinkToFit(bool enable);
    void setBackgroundColor(QColor const & color);
    void setDefault();
    void clear();


    // static functions
    //
    static Format::Type dateType(const QString&);
    static Format::Type timeType(const QString&);
    static Format::Type fractionType(const QString&);
    static Format::Type numberType(const QString&);
    static Currency numberCurrency(const QString&);

    /**
     * @return the name of the data style (number, currency, percentage, date,
     * boolean, text)
     */
    static QString saveOdfStyleNumeric(KoGenStyle &style, KoGenStyles &mainStyles, Format::Type _style,
                                       const QString &_prefix, const QString &_postfix, int _precision, const QString& symbol);
    static QString saveOdfStyleNumericDate(KoGenStyles &mainStyles, Format::Type _style,
                                           const QString &_prefix, const QString &_suffix);
    static QString saveOdfStyleNumericFraction(KoGenStyles &mainStyles, Format::Type _style,
            const QString &_prefix, const QString &_suffix);
    static QString saveOdfStyleNumericTime(KoGenStyles& mainStyles, Format::Type _style,
                                           const QString &_prefix, const QString &_suffix);
    static QString saveOdfStyleNumericCustom(KoGenStyles&mainStyles, Format::Type _style,
            const QString &_prefix, const QString &_suffix);
    static QString saveOdfStyleNumericScientific(KoGenStyles&mainStyles, Format::Type _style,
            const QString &_prefix, const QString &_suffix, int _precision);
    static QString saveOdfStyleNumericPercentage(KoGenStyles&mainStyles, Format::Type _style, int _precision,
            const QString &_prefix, const QString &_suffix);
    static QString saveOdfStyleNumericMoney(KoGenStyles&mainStyles, Format::Type _style,
                                            const QString& symbol, int _precision,
                                            const QString &_prefix, const QString &_suffix);
    static QString saveOdfStyleNumericText(KoGenStyles&mainStyles, Format::Type _style, int _precision,
                                           const QString &_prefix, const QString &_suffix);
    static QString saveOdfStyleNumericNumber(KoGenStyles&mainStyles, Format::Type _style, int _precision,
            const QString &_prefix, const QString &_suffix);
    static QString saveOdfBackgroundStyle(KoGenStyles &mainStyles, const QBrush &brush);

    /**
     * Returns the name of a color.  This is the same as returned by QColor::name, but an internal cache
     * is used to reduce the overhead when asking for the name of the same color.
     */
    static QString colorName(const QColor& color);

    static bool compare(const SubStyle* one, const SubStyle* two);


    /** Returns true if both styles have the same properties */
    bool operator== (const Style& style) const;
    inline bool operator!=(const Style& other) const {
        return !operator==(other);
    }
    void operator=(const Style& style);
    Style operator-(const Style& style) const;
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
protected:
    QList<SharedSubStyle> subStyles() const;

    SharedSubStyle createSubStyle(Key key, const QVariant& value);
    virtual void insertSubStyle(Key key, const QVariant& value);
    void insertSubStyle(const SharedSubStyle& subStyle);
    bool releaseSubStyle(Key key);

private:
    friend class StyleStorage;
    friend class BorderColorCommand;

    class Private;
    QSharedDataPointer<Private> d;
};



/**
 * \ingroup Style
 * A named cell style.
 */
class CustomStyle : public Style
{
public:
    /**
     * Constructor.
     * Creates a custom style.
     * \param name The name of this style.
     * \param parent The style whose attributes are inherited - the parent style.
     */
    explicit CustomStyle(const QString& name, CustomStyle* parent = 0);
    virtual ~CustomStyle();

    virtual StyleType type() const;
    void setType(StyleType type);

    void setName(QString const & name);
    QString const & name() const;

    bool loadXML(KoXmlElement const & style, QString const & name);
    void save(QDomDocument & doc, QDomElement & styles, const StyleManager* styleManager);

    /**
     * Loads the style properties from @p style .
     * Determines also the parent's name.
     * @param stylesReader map of all styles
     * @param style the DOM element defining the style
     * @param name the style's new name
     */
    void loadOdf(KoOdfStylesReader& stylesReader, const KoXmlElement& style,
                 const QString& name, Conditions& conditions,
                 const StyleManager* styleManager);

    /**
     * @reimp
     * Stores an OASIS user style.
     * @return the OASIS style's name
     */
    virtual QString saveOdf(KoGenStyle& style, KoGenStyles &mainStyles,
                            const StyleManager* manager) const;


    bool operator==(const CustomStyle& other) const;
    inline bool operator!=(const CustomStyle& other) const {
        return !operator==(other);
    }

    /**
     * @return the number of references to this style.
     */
    int usage() const;

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
class SubStyle : public QSharedData
{
public:
    SubStyle() {}
    virtual ~SubStyle() {}
    virtual Style::Key type() const {
        return Style::DefaultStyleKey;
    }
    virtual void dump() const {
        kDebug() << debugData();
    }
    virtual QString debugData(bool withName = true) const {
        QString out; if (withName) out = name(Style::DefaultStyleKey); return out;
    }
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
    inline SharedSubStyle() : d(new SubStyle()) {}
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
};

class NamedStyle : public SubStyle
{
public:
    NamedStyle(const QString& n) : SubStyle(), name(n) {}
    virtual Style::Key type() const {
        return Style::NamedStyleKey;
    }
    virtual void dump() const {
        kDebug() << debugData();
    }
    virtual QString debugData(bool withName = true) const {
        QString out; if (withName) out = SubStyle::name(Style::NamedStyleKey) + ' '; out += name; return out;
    }
    QString name;
};

template<Style::Key key, class Value1>
class SubStyleOne : public SubStyle
{
public:
    SubStyleOne(const Value1& v = Value1()) : SubStyle(), value1(v) {}
    virtual Style::Key type() const {
        return key;
    }
    virtual void dump() const {
        kDebug(36006) << debugData();
    }
    virtual QString debugData(bool withName = true) const {
        QString out; if (withName) out = name(key) + ' '; QDebug qdbg(&out); qdbg << value1; return out;
    }
    Value1 value1;
};

} // namespace KSpread

Q_DECLARE_TYPEINFO(KSpread::Style, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(KSpread::CustomStyle, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(KSpread::SharedSubStyle, Q_MOVABLE_TYPE);

#endif // KSPREAD_STYLE
