/* This file is part of the KDE project
   Copyright (C) 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

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

#ifndef TOKENELEMENT_H
#define TOKENELEMENT_H

#include "kformula_export.h"
#include "BasicElement.h"
#include <QPainterPath>
#include <QFont>

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
class KOFORMULA_EXPORT TokenElement : public BasicElement {
public:
    /// The standart constructor
    TokenElement( BasicElement* parent = 0 );

    /**
     * Obtain a list of all child elements of this element
     * @return a QList with pointers to all child elements
     */
    const QList<BasicElement*> childElements();

    /**
     * Render the element to the given QPainter
     * @param painter The QPainter to paint the element to
     * @param am AttributeManager containing style info
     */
    void paint( QPainter& painter, AttributeManager* am );

    /**
     * Calculate the size of the element and the positions of its children
     * @param am The AttributeManager providing information about attributes values
     */
    void layout( const AttributeManager* am );

    /**
     * Insert new content that the user typed at the cursor position
     * @param cursor The cursor holding the position where to inser
     * @param child A BasicElement to insert
     */
    void insertChild( FormulaCursor* cursor, BasicElement* child );

    /**
     * Remove a child element
     * @param cursor The cursor holding the position where to remove
     * @param element The BasicElement to remove
     */ 
    void removeChild( FormulaCursor* cursor, BasicElement* element );

    /**
     * Implement the cursor behaviour for the element
     * @param direction Indicates whether the cursor moves up, down, right or left
     * @return A this pointer if the element accepts if not the element to asked instead
     */
    BasicElement* acceptCursor( const FormulaCursor* cursor );

    /**
     * Obtain the x position of the cursor inside this token element
     * @oaram cursor The FormulaCursor who is supposed to be used for calculation
     * @return The offset from the left origin
     */
    double cursorOffset( const FormulaCursor* cursor ) const;

    /// Process @p raw and render it to @p path
    virtual void renderToPath( const QString& raw, QPainterPath& path ) = 0;

protected:
    /// Read contents of the token element. Content should be unicode text strings or mglyphs
    bool readMathMLContent( const KoXmlElement& parent );

    /// Write all content to the KoXmlWriter - reimplemented by the child elements
    void writeMathMLContent( KoXmlWriter* writer ) const;

    /// @return The font to use
    QFont font() const;

private:
    /// The raw string like it is read and written from MathML
    QString m_rawString;

    /// The cache for the chosen font
    QFont m_font;

    /// A list of pointers to embedded GlyphElements
    QList<GlyphElement*> m_glyphs;

    /// A painter path holding text content for fast painting
    QPainterPath m_contentPath;
};

#endif // TOKENELEMENT_H
