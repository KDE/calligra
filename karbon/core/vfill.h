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

#ifndef __VFILL_H__
#define __VFILL_H__

#include "vcolor.h"
#include "vgradient.h"
#include "vpattern.h"
#include <koffice_export.h>

class QDomElement;
class KoGenStyle;
class KoGenStyles;
class KoOasisLoadingContext;


/**
 * Manages the fill of shapes.
 *
 * The fill can be solid or gradient.
 * Also two fill rules are supported that effect how the shape is
 * filled. For explanation see the QPainter documentation.
 *
 * Default is no fill and even-odd filling rule.
 */
class KARBONBASE_EXPORT VFill
{
public:
	enum VFillType
	{
		none     = 0,	/// no fill at all
		solid    = 1,	/// solid fill
		grad     = 2,	/// gradient fill
		patt 	 = 3,	/// pattern fill
		unknown  = 4
	};

	VFill();
	VFill( const VColor & );
	VFill( const VFill & );

	const VColor& color() const { return m_color; }
	void setColor( const VColor& color, bool bsolid = true ) { m_color = color; if( bsolid ) m_type = solid; }

	VGradient& gradient() { return m_gradient; }
	const VGradient& gradient() const { return m_gradient; }

	VPattern& pattern() { return m_pattern; }
	const VPattern& pattern() const { return m_pattern; }

	VFillType type() const { return m_type; }
	void setType( VFillType type ) { m_type = type; }

	void save( QDomElement& element ) const;
	void saveOasis( KoGenStyles &mainStyles, KoGenStyle &style ) const;
	void load( const QDomElement& element );
	void loadOasis( const QDomElement &object, KoOasisLoadingContext &context, VObject* parent = 0L );

	VFill& operator=( const VFill& fill );

	void transform( const QWMatrix& m );

private:
	VColor		m_color;
	VGradient	m_gradient;
	VPattern	m_pattern;

	VFillType	m_type		: 3;
};

#endif

