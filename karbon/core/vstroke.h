/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSTROKE_H__
#define __VSTROKE_H__

#include <qvaluelist.h>

#include "vcolor.h"
#include "vdashpattern.h"
#include "vgradient.h"


enum VStrokeType
{
	stroke_none     = 0,	/// no stroke at all
	stroke_stroke   = 1,	/// solid stroke
	stroke_gradient = 2,	/// gradient as stroke
	stroke_unknown  = 3
};

enum VLineCap
{
	cap_butt   = 0,
	cap_round  = 1,
	cap_square = 2
};

enum VLineJoin
{
	join_miter = 0,
	join_round = 1,
	join_bevel = 2
};


class QDomElement;
class VObject;

/**
 * Manages stroke properties.
 * Supported are line join/cap styles equivalents to the qpainter ones.
 * Also the line width in pixels and line stroke type (solid / gradient).
 * Finally it also managed the dashing pattern, see VDashPattern.
 *
 * Default is black solid outline of width 1 with miter join, butt cap
 * style and no dashes.
 */

class VStroke
{
public:
	VStroke( VObject* parent = 0L, float width = 1.0, const VLineCap cap = cap_butt,
			 const VLineJoin join = join_miter, float miterLimit = 10.0 );
	VStroke( const VStroke& stroke );

	void setParent( VObject* parent ) { m_parent = parent; }
	VObject* parent() { return m_parent; }

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

	VDashPattern& dashPattern() { return m_dashPattern; }
	const VDashPattern& dashPattern() const { return m_dashPattern; }

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

private:
	VObject* m_parent;

	VStrokeType m_type;

	VColor m_color;
	VGradient m_gradient;
	float m_lineWidth;
	VLineCap m_lineCap;
	VLineJoin m_lineJoin;
	float m_miterLimit;
	VDashPattern m_dashPattern;
};

#endif
