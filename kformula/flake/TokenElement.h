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
#include <QList>
#include <QStringList>

namespace FormulaShape {

class GlyphElement;

/**
 * @short Baseclass for all token elements
 *
 * The MathML specification describes a number of token elements. The classes
 * of these all derive from TokenElement except of mspace. Because of the huge
 * similarity between the token elements loading, saving, painting and layouting
 * code can mostly be shared. This is because token elements hold some text or
 * string that has to be dealt with.
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
    virtual void paint( QPainter& painter, const AttributeManager* am );

    /**
     * Calculate the size of the element and the positions of its children
     * @param am The AttributeManager providing information about attributes values
     */
    virtual void layout( const AttributeManager* am );

protected:
    /// Read contents of the token element. Content should be unicode text strings or mglyphs
    bool readMathMLContent( const KoXmlElement& parent );

    /// Write all content to the KoXmlWriter - reimplemented by the child elements
    virtual void writeMathMLContent( KoXmlWriter* writer ) const;

    /// @return The string to be painted - probably a parsed rawString
    virtual QString stringToRender( const QString& rawString ) const;

private:
    /// The raw string like it is read and written from MathML
    QStringList m_rawStringList;

    /// A list of this pointers for raw strings and pointers to embedded GlyphElements
    QList<BasicElement*> m_content;
};

} // namespace FormulaShape

#endif // TOKENELEMENT_H
