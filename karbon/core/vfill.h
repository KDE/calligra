/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VFILL_H__
#define __VFILL_H__

#include "vcolor.h"
#include "vgradient.h"
#include "vpattern.h"


class QDomElement;


/**
 * Manages the fill of shapes.
 *
 * The fill can be solid or gradient.
 * Also two fill rules are supported that effect how the shape is
 * filled. For explanation see the QPainter documentation.
 *
 * Default is no fill and even-odd filling rule.
 */
class VFill
{
public:
	enum VFillRule
	{
		evenOdd = 0,
		winding = 1
	};

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
	void setColor( const VColor& color ) { m_color = color; m_type = solid; }

	VGradient& gradient() { return m_gradient; }
	const VGradient& gradient() const { return m_gradient; }

	VPattern& pattern() { return m_pattern; }
	const VPattern& pattern() const { return m_pattern; }

	VFillType type() const { return m_type; }
	void setType( VFillType type ) { m_type = type; }

	VFillRule fillRule() const { return m_fillRule; }
	void setFillRule( VFillRule fillRule ) { m_fillRule = fillRule; }

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

	VFill& operator=( const VFill& fill );

private:
	VColor m_color;
	VGradient m_gradient;
	VPattern m_pattern;

	VFillType m_type;
	VFillRule m_fillRule;
};

#endif

