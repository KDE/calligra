/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

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

#ifndef __VSTROKE_H__
#define __VSTROKE_H__

#include <q3valuelist.h>

#include "vcolor.h"
#include "vdashpattern.h"
#include "vgradient.h"
#include "vpattern.h"
#include <koffice_export.h>

class QDomElement;
class VObject;
class KoGenStyle;
class KoStyleStack;


/**
 * Manages stroke properties.
 * Supported are line join/cap styles equivalents to the qpainter ones.
 * Also the line width in pixels and line stroke type (solid / gradient).
 * Finally it also managed the dashing pattern, see VDashPattern.
 *
 * Default is black solid outline of width 1 with miter join, butt cap
 * style and no dashes.
 */

class KARBONBASE_EXPORT VStroke
{
public:
	enum VStrokeType
	{
		none     = 0,	/// no stroke at all
		solid    = 1,	/// solid stroke
		grad     = 2,	/// gradient as stroke
		patt     = 3,	/// pattern as stroke
		unknown  = 4
	};

	enum VLineCap
	{
		capButt   = 0,
		capRound  = 1,
		capSquare = 2
	};

	enum VLineJoin
	{
		joinMiter = 0,
		joinRound = 1,
		joinBevel = 2
	};


	VStroke( VObject* parent = 0L, float width = 1.0, const VLineCap cap = capButt,
			 const VLineJoin join = joinMiter, float miterLimit = 10.0 );
	VStroke( const VColor &c, VObject* parent = 0L, float width = 1.0, const VLineCap cap = capButt,
			 const VLineJoin join = joinMiter, float miterLimit = 10.0 );
	VStroke( const VStroke& stroke );

	void setParent( VObject* parent ) { m_parent = parent; }
	VObject* parent()const { return m_parent; }

	VStrokeType type() const { return m_type; }
	void setType( VStrokeType type ) { m_type = type; }

	const VColor& color() const { return m_color; }
	void setColor( const VColor& color ) { m_color = color; }

	float lineWidth() const { return m_lineWidth; }
	void setLineWidth( float width );

	VLineCap lineCap() const { return m_lineCap; }
	void setLineCap( VLineCap cap ) { m_lineCap = cap; }

	VLineJoin lineJoin() const { return m_lineJoin; }
	void setLineJoin( VLineJoin join ) { m_lineJoin = join; }

	float miterLimit() const { return m_miterLimit; }
	void setMiterLimit( float limit ) { m_miterLimit = limit; }

	VGradient& gradient() { return m_gradient; }
	const VGradient& gradient() const { return m_gradient; }

	VPattern& pattern() { return m_pattern; }
	const VPattern& pattern() const { return m_pattern; }

	VDashPattern& dashPattern() { return m_dashPattern; }
	const VDashPattern& dashPattern() const { return m_dashPattern; }

	void save( QDomElement& element ) const;
	void saveOasis( KoGenStyle &style ) const;
	void load( const QDomElement& element );
	void loadOasis( const KoStyleStack &stack );


	VStroke& operator=( const VStroke& stroke );

	void transform( const QMatrix& m );

private:
	VObject			*m_parent;

	VColor			m_color;
	VGradient		m_gradient;
	VPattern		m_pattern;
	float			m_lineWidth;
	float			m_miterLimit;
	VLineCap		m_lineCap	: 2;
	VLineJoin		m_lineJoin	: 2;
	VStrokeType		m_type		: 3;
	VDashPattern	m_dashPattern;
};

#endif
