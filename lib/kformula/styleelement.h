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

    virtual void calcSizes( const ContextStyle& context,
						    ContextStyle::TextStyle tstyle,
						    ContextStyle::IndexStyle istyle,
							StyleAttributes& style );

    virtual void draw( QPainter& painter, const LuPixelRect& r,
                       const ContextStyle& context,
                       ContextStyle::TextStyle tstyle,
                       ContextStyle::IndexStyle istyle,
					   StyleAttributes& style,
                       const LuPixelPoint& parentOrigin );

protected:
    virtual bool readAttributesFromMathMLDom( const QDomElement &element );

    void setCharStyle( CharStyle cs );
    CharStyle getCharStyle() { return style; }
	bool customCharStyle() { return custom_style; }

    void setCharFamily( CharFamily cf );
    CharFamily getCharFamily() { return family; }
	bool customCharFamily() { return custom_family; }

    void setColor( const QColor& c ) { color = c; }
    QColor getColor() const { return color; }
	bool customColor() const { return custom_color; }

    void setBackground( const QColor& bg ) { background = bg; }
    QColor& getBackground() { return background; }
	bool customBackground() const { return custom_background; }

private:
	double getSizeFactor( const ContextStyle& context, double factor );
	bool ownSize;
	bool absoluteSize;
    double size;
	double factor;
    CharStyle style;
	bool custom_style;
    CharFamily family;
	bool custom_family;
    QColor color;
	bool custom_color;
    QColor background;
	bool custom_background;
};

KFORMULA_NAMESPACE_END

#endif // STYLEELEMENT_H
