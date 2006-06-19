/* This file is part of the KDE project
   Copyright (C) 2006 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef STYLEELEMENT_H
#define STYLEELEMENT_H

#include "kformuladefs.h"
#include "sequenceelement.h"

KFORMULA_NAMESPACE_BEGIN

class StyleElement : public SequenceElement {
    typedef SequenceElement inherited;
public:
    StyleElement( BasicElement* parent = 0 );

    void setAbsoluteSize( double s );
	void setRelativeSize( double s );
    double getAbsoluteSize() const { return size; }
	double getRelativeSize() const { return factor; }

    void setColor( const QColor& c ) { color = c; }
    QColor getColor() const { return color; }

    void setBackground( const QColor& bg ) { background = bg; }
    QColor getBackground() const {return background; }

    virtual void calcSizes( const ContextStyle& style,
						    ContextStyle::TextStyle tstyle,
						    ContextStyle::IndexStyle istyle,
							double factor );

    virtual void draw( QPainter& painter, const LuPixelRect& r,
                       const ContextStyle& context,
                       ContextStyle::TextStyle tstyle,
                       ContextStyle::IndexStyle istyle,
					   double factor,
                       const LuPixelPoint& parentOrigin );

protected:
    virtual bool readAttributesFromMathMLDom( const QDomElement &element );
    void setCharStyle( CharStyle cs );
    void setCharFamily( CharFamily cf );
    CharStyle getCharStyle() { return style; }
    CharFamily getCharFamily() { return family; }

private:
	double getProportion( const ContextStyle& context, double factor );
	bool ownSize;
	bool absoluteSize;
    double size;
	double factor;
    CharStyle style;
    CharFamily family;
    QColor color;
    QColor background;
};

KFORMULA_NAMESPACE_END

#endif // STYLEELEMENT_H
