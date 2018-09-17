/* This file is part of the Calligra project
 * Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>
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

#ifndef SCRIPTING_STYLE_H
#define SCRIPTING_STYLE_H

#include <QObject>
#include <KoParagraphStyle.h>
#include <KoCharacterStyle.h>
#include <KoListStyle.h>

namespace Scripting
{

/**
* The CharacterStyle class provides access to styles for characters.
*/
class CharacterStyle : public QObject
{
    Q_OBJECT
public:
    CharacterStyle(QObject* parent, KoCharacterStyle* style) : QObject(parent), m_style(style) {}
    virtual ~CharacterStyle() {}
    KoCharacterStyle* style() const {
        return m_style;
    }
public Q_SLOTS:

    /** Return the user-visible name the character-style has. */
    QString name() const {
        return m_style->name();
    }
    /** Set the user-visible name the character-style has. */
    void setName(const QString& name) {
        m_style->setName(name);
    }

    /***** Font *****/

    /** Return the font-family name. */
    QString family() const {
        return m_style->fontFamily();
    }
    /**
    * Set the font-family name.
    *
    * The following python sets the font for the two character-styles
    * mycharstyle1 and mycharstyle2;
    * \code
    * mycharstyle1.setFamily("Times New Roman")
    * mycharstyle2.setFamily("Arial")
    * \endcode
    */
    void setFamily(const QString &family) {
        m_style->setFontFamily(family);
    }

    /** Return the size of the font. */
    qreal size() const {
        return m_style->fontPointSize();
    }
    /**
    * Set the size of the font.
    *
    * Python sample that sets the font size;
    * \code
    * mycharstyle.setSize(12.0)
    * \endcode
    */
    void setSize(qreal size) {
        m_style->setFontPointSize(size);
    }

    /** Return the weight of the font. */
    int weight() const {
        return m_style->fontWeight();
    }
    /**
    * Set the weight of the font.
    *
    * Python sample that sets the font weight;
    * \code
    * if style == "normal":
    *     mycharstyle.setWeight(50)
    * elif style == "bold":
    *     mycharstyle.setWeight(75)
    * else:
    *     raise "Invalid style %s" % style
    * \endcode
    */
    void setWeight(int weight) {
        m_style->setFontWeight(weight);
    }

    /** Return true if the font is italic. */
    bool italic() const {
        return m_style->fontItalic();
    }
    /**
    * Set if the font should be italic or not.
    *
    * Python sample that sets the font italic for the both
    * character-styles mycharstyle1 and mycharstyle2;
    * \code
    * mycharstyle1.setItalic(True)
    * mycharstyle2.setItalic(False)
    * \endcode
    */
    void setItalic(bool italic) {
        m_style->setFontItalic(italic);
    }

    /** Return true if the font is bold. */
    bool bold() const {
        return m_style->fontWeight() >= 75;
    }
    /**
    * Set if the font should be bold or not.
    *
    * Python sample that sets the font bold for the both
    * character-styles mycharstyle1 and mycharstyle2;
    * \code
    * mycharstyle1.setBold(True)
    * mycharstyle2.setBold(False)
    * \endcode
    */
    void setBold(bool bold) {
        m_style->setFontWeight(bold ? 75 : 50);
    }

    /** Return true if there is an underline. */
    bool underline() const {
        return m_style->underlineStyle() != KoCharacterStyle::NoLineStyle;
    }
    /** Set the underline. */
    void setUnderline(bool underline) {
        m_style->setUnderlineStyle(underline ? KoCharacterStyle::SolidLine : KoCharacterStyle::NoLineStyle);
    }

    /***** Foreground *****/

    /** Return the font-color. */
    QColor color() const {
        return m_style->foreground().color();
    }
    /**
    * Set the font-color.
    *
    * Python sample that sets the font text-color to red aka
    * RGB-value FF0000 ;
    * \code
    * mycharstyle.setColor("#ff0000")
    * \endcode
    */
    void setColor(const QColor& color) {
        QBrush brush = m_style->foreground();
        brush.setColor(color);
        m_style->setForeground(brush);
    }

    /***** Background *****/

    /** Return the background-color. */
    QColor backgroundColor() const {
        return m_style->background().color();
    }
    /**
    * Set the background-color.
    *
    * Python sample that sets the text- and the background-color
    * of the character-style mycharstyle;
    * \code
    * mycharstyle.setColor("#ffffff")
    * mycharstyle.setBackgroundColor("#0000ff")
    * \endcode
    */
    void setBackgroundColor(const QColor &color) {
        QBrush brush = m_style->background();
        brush.setColor(color);
        m_style->setBackground(brush);
    }

private:
    KoCharacterStyle* m_style;
};

/**
* The ParagraphStyle class provides access to styles for paragraphs.
*/
class ParagraphStyle : public QObject
{
    Q_OBJECT
    Q_ENUMS(Alignment)
    Q_ENUMS(ListStyle)
public:
    ParagraphStyle(QObject* parent, KoParagraphStyle* style) : QObject(parent), m_style(style) {}
    virtual ~ParagraphStyle() {}
    KoParagraphStyle* style() const {
        return m_style;
    }

    enum Alignment {
        AlignLeft = Qt::AlignLeft,
        AlignRight = Qt::AlignRight,
        AlignHCenter = Qt::AlignHCenter,
        AlignJustify = Qt::AlignJustify,
        AlignTop = Qt::AlignTop,
        AlignBottom = Qt::AlignBottom,
        AlignVCenter = Qt::AlignVCenter,
        AlignCenter = Qt::AlignCenter
    };

    enum ListStyle {
        None = KoListStyle::None,
        SquareItem = KoListStyle::SquareItem,
        DiscItem = KoListStyle::Bullet,
        Bullet = KoListStyle::Bullet,
        CircleItem = KoListStyle::CircleItem,
        DecimalItem = KoListStyle::DecimalItem,
        AlphaLowerItem = KoListStyle::AlphaLowerItem,
        AlphaUpperItem = KoListStyle::UpperAlphaItem,
        RomanLowerItem = KoListStyle::RomanLowerItem,
        RomanUpperItem = KoListStyle::UpperRomanItem,
        BoxItem = KoListStyle::BoxItem
    };

public Q_SLOTS:

    /***** Name *****/

    /** Return the user-visible name the paragraph-style has. */
    QString name() const {
        return m_style->name();
    }

    /** Set the user-visible name the paragraph-style has. */
    void setName(const QString& name) {
        m_style->setName(name);
    }

    /***** Alignment *****/

    /**
    * Return the alignment the paragraph-style has.
    *
    * Valid values are;
    * \li AlignLeft, AlignRight, AlignHCenter, AlignJustify
    * \li AlignTop, AlignBottom, AlignVCenter, AlignCenter
    *
    * The following python sample demonstrates the usage;
    * \code
    * alignment = MyParagraphStyle.alignment()
    * if alignment == MyParagraphStyle.AlignLeft:
    *     print "Align Left"
    * elif alignment == MyParagraphStyle.AlignRight:
    *     print "Align Right"
    * elif alignment == MyParagraphStyle.AlignHCenter:
    *     print "Align Center"
    * elif alignment == MyParagraphStyle.AlignJustify:
    *     print "Align Justify"
    * \endcode
    */
    int alignment() const {
        return m_style->alignment();
    }

    /** Set the alignment the paragraph-style has. */
    void setAlignment(int alignment) {
        m_style->setAlignment((Qt::Alignment) alignment);
    }

    /***** Padding *****/

    /** Return the distance between text and border. */
    QRectF padding() const {
        return QRectF(m_style->leftPadding(), m_style->topPadding(), m_style->rightPadding(), m_style->bottomPadding());
    }

    /** Set the distance between text and border. */
    void setPadding(const QRectF& r) {
        m_style->setLeftPadding(r.x());
        m_style->setTopPadding(r.y());
        m_style->setRightPadding(r.width());
        m_style->setBottomPadding(r.height());
    }

    /***** Margin *****/

    /** Return the margin between text and border. */
    QRectF margin() const {
        return QRectF(m_style->leftMargin(), m_style->topMargin(), m_style->rightMargin(), m_style->bottomMargin());
    }

    /** Set the margin between text and border. */
    void setMargin(const QRectF& r) {
        m_style->setLeftMargin(r.x());
        m_style->setTopMargin(r.y());
        m_style->setRightMargin(r.width());
        m_style->setBottomMargin(r.height());
    }

#if 0
    /***** Border *****/

    /*TODO simplify border options even more. Probably just deal with a QVariantMap using QMetaEnum's, e.g.
    QVariantMap border() {
        QVariantMap map;
        for(int i = KoParagraphStyle::HasLeftBorder; i <= KoParagraphStyle::BottomBorderColor; i++)
            map.insert("", m_style->property(""));
        return map;
    }
    */

    QRect borderStyle() {
        return QRect(m_style->leftBorderStyle(), m_style->topBorderStyle(), m_style->rightBorderStyle(), m_style->bottomBorderStyle());
    }

    void setBorderStyle(const QRect& rect) {
        m_style->setLeftBorderStyle((KoParagraphStyle::BorderStyle) r.x());
        m_style->setTopBorderStyle((KoParagraphStyle::BorderStyle) r.y());
        m_style->setRightBorderStyle((KoParagraphStyle::BorderStyle) r.width());
        m_style->setBottomBorderStyle((KoParagraphStyle::BorderStyle) r.height());
    }

    QRect borderSpacing() {
        return QRect(m_style->leftBorderSpacing(), m_style->topBorderSpacing(), m_style->rightBorderSpacing(), m_style->bottomBorderSpacing());
    }

    void setBorderSpacing(const QRect& rect) {
        m_style->setLeftBorderSpacing(r.x());
        m_style->setTopBorderSpacing(r.y());
        m_style->setRightBorderSpacing(r.width());
        m_style->setBottomBorderSpacing(r.height());
    }

    QRect borderWidth() {
        return QRect(m_style->leftBorderWidth(), m_style->topBorderWidth(), m_style->rightBorderWidth(), m_style->bottomBorderWidth());
    }

    void setBorderWidth(const QRect& rect) {
        m_style->setLeftBorderWidth(r.x());
        m_style->setTopBorderWidth(r.y());
        m_style->setRightBorderWidth(r.width());
        m_style->setBottomBorderWidth(r.height());
    }

    QRect innerBorderWidth() {
        return QRect(m_style->leftInnerBorderWidth(), m_style->topInnerBorderWidth(), m_style->rightInnerBorderWidth(), m_style->bottomInnerBorderWidth());
    }

    void setInnerBorderWidth(const QRect& rect) {
        m_style->setLeftInnerBorderWidth(r.x());
        m_style->setTopInnerBorderWidth(r.y());
        m_style->setRightInnerBorderWidth(r.width());
        m_style->setBottomInnerBorderWidth(r.height());
    }

    QRect borderColor() {
        return QRect(m_style->leftBorderColor(), m_style->topBorderColor(), m_style->rightBorderColor(), m_style->bottomBorderColor());
    }

    void setBorderColor(const QRect& rect) {
        m_style->setLeftBorderColor(r.x());
        m_style->setTopBorderColor(r.y());
        m_style->setRightBorderColor(r.width());
        m_style->setBottomBorderColor(r.height());
    }
#endif

    /***** List *****/

    /** Return true if this item is a list item else false is returned. */
    bool isList() const {
        return m_style->listStyle() != 0;
    }

#if 0
    /** Return the style of listitems. */
    int listStyle() const {
        //return m_style->listStyle() ? m_style->listStyle()->style() : 0;
        KoListStyle liststyle = m_style->listStyle();
        return liststyle.isValid() ? new ListStyle(this, liststyle) : 0;
    }

    /** Set the style of listitems. */
    void setListStyle(int liststyle) {
        if (m_style->listStyle())
            m_style->listStyle()->setStyle((KoListStyle::Style)liststyle);
        else {
            KoListStyle s;
            s.setStyle((KoListStyle::Style) liststyle);
            m_style->setListStyle(s);
        }
    }
#endif

    /** Return the character-style for this paragraph-style. */
    QObject* characterStyle() {
        KoCharacterStyle* charstyle = m_style->characterStyle();
        return charstyle ? new CharacterStyle(this, charstyle) : 0;
    }
    /** Set the character-style for this paragraph-style. */
    void setCharacterStyle(QObject *style) {
        CharacterStyle* charstyle = dynamic_cast<CharacterStyle*>(style);
        KoCharacterStyle* s = charstyle ? charstyle->style() : 0;
        if (s) m_style->setCharacterStyle(s);
    }

private:
    KoParagraphStyle* m_style;
};

}

#endif
