/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>
                 2009 Jeremias Epperlein <jeeree@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TOKENELEMENT_H
#define TOKENELEMENT_H

#include "BasicElement.h"
#include "koformula_export.h"
#include <QFont>
#include <QPainterPath>

class GlyphElement;
class FormulaCursor;

/**
 * @short Baseclass for all token elements
 *
 * The MathML specification describes a number of token elements. The classes
 * of these all derive from TokenElement except of mspace. Because of the huge
 * similarity between the token elements loading, saving, painting and layouting
 * code can mostly be shared. This is because token elements hold some text or
 * string that has to be dealt with.
 * The handling of embedded glyphs is also implemented in TokenEa list of the embedded GlyphElements. The TokenElement's QString
 * m_rawString is the string that holds the raw data for the TokenElement. This
 * string contains also QChar that are set to the QChar::ObjectReplacementCharacter
 * category. For rendering these QChar's are replaced with the content of the glyph
 * elements they represent in the raw string.
 */
class KOFORMULA_EXPORT TokenElement : public BasicElement
{
public:
    /// The standard constructor
    explicit TokenElement(BasicElement *parent = nullptr);

    /**
     * Obtain a list of all child elements of this element
     * @return a QList with pointers to all child elements
     */
    const QList<BasicElement *> childElements() const override;

    /**
     * Render the element to the given QPainter
     * @param painter The QPainter to paint the element to
     * @param am AttributeManager containing style info
     */
    void paint(QPainter &painter, AttributeManager *am) override;

    /**
     * Calculate the size of the element and the positions of its children
     * @param am The AttributeManager providing information about attributes values
     */
    void layout(const AttributeManager *am) override;

    /**
     * Insert @p text at @p position
     * @return true, if the insert was successful
     */
    virtual bool insertText(int position, const QString &text);

    /// inherited from BasicElement
    virtual bool insertChild(int position, BasicElement *child);

    /// insert a list of glyphs without changing rawString, position points to m_glyphs list
    void insertGlyphs(int position, QList<GlyphElement *> glyphs);

    QList<GlyphElement *> glyphList(int position, int length);

    /// remove the letter after @p position and return the starting position
    ///  of the removed glyphs in the glyphlist
    virtual int removeText(int position, int length = 1);

    /**
     * Implement the cursor behaviour for the element
     * @param cursor The cursor
     * @return A this pointer if the element accepts if not the element to asked instead
     */
    bool acceptCursor(const FormulaCursor &cursor) override;

    /// inherited from BasicElement
    bool moveCursor(FormulaCursor &newcursor, FormulaCursor &oldcursor) override;

    /**
     * Obtain the x position of the cursor inside this token element
     * @param position The cursor position in the element
     * @return The offset from the left origin
     */
    qreal cursorOffset(const int position) const;

    /// Process @p raw and render it to @p path
    virtual QRectF renderToPath(const QString &raw, QPainterPath &path) const = 0;

    /// inherited from BasicElement
    int endPosition() const override;

    /// inherited from BasicElement
    QLineF cursorLine(int position) const override;

    /// inherited from BasicElement
    bool setCursorTo(FormulaCursor &cursor, QPointF point) override;

    /// set m_rawString to @p text and empty the glyph list
    void setText(const QString &text);

    /// @return the raw string
    const QString &text();

    const QString writeElementContent() const override;

protected:
    /// Read contents of the token element. Content should be unicode text strings or mglyphs
    bool readMathMLContent(const KoXmlElement &parent) override;

    /// Write all content to the KoXmlWriter - reimplemented by the child elements
    void writeMathMLContent(KoXmlWriter *writer, const QString &ns) const override;

    /// @return The font to use
    QFont font() const;

    /// Whether the element should be stretched horizontally (e.g. arrows)
    bool m_stretchHorizontally;
    /// Whether the element should be stretched vertically (e.g. brackets)
    bool m_stretchVertically;

    /// Size to stretch from
    QSizeF m_originalSize;

    /// The raw string like it is read and written from MathML
    QString m_rawString;

private:
    /// The cache for the chosen font
    QFont m_font;

    /// A list of pointers to embedded GlyphElements
    QList<GlyphElement *> m_glyphs;

    /// A list of offsets of the letters
    /// They represent the position of the cursor right of
    /// the index, starting with 0.0
    QList<qreal> m_offsets;

    /// A painter path holding text content for fast painting
    QPainterPath m_contentPath;
    /// x offset for painting the path
    qreal m_xoffset;
};

#endif // TOKENELEMENT_H
