/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VFILL_H__
#define __VFILL_H__

#include "vcolor.h"
#include "vgradient.h"


enum VFillRule
{
	fillrule_evenOdd = 0,
	fillrule_winding = 1
};

enum VFillType
{
	fill_none     = 0,	/// no fill at all
	fill_fill     = 1,	/// solid fill
	fill_gradient = 2,	/// gradient fill
	fill_unknown  = 3
};

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
	VFill();
	VFill( const VColor & );

	const VColor& color() const { return m_color; }
	void setColor( const VColor& color ) { m_color = color; m_type = fill_fill; }

	VGradient& gradient() { return m_gradient; }
	const VGradient& gradient() const { return m_gradient; }

	VFillType type() const { return m_type; }
	void setType( VFillType type ) { m_type = type; }

	VFillRule fillRule() const { return m_fillRule; }
	void setFillRule( VFillRule fillRule ) { m_fillRule = fillRule; }

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

private:
	VColor m_color;
	VGradient m_gradient;

	VFillType m_type;
	VFillRule m_fillRule;
};

#endif

