/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSTROKE_H__
#define __VSTROKE_H__

#include <qvaluelist.h>

#include "vcolor.h"

class QDomElement;


class VStroke
{
public:
	enum VLineCap{
		cap_butt   = 0,
		cap_round  = 1,
		cap_square = 2 };

	enum VLineJoin{
		join_miter = 0,
		join_round = 1,
		join_bevel = 2 };

	VStroke( float width = 1.0, const VLineCap cap = cap_butt,
		const VLineJoin join = join_miter, float miterLimit = 10.0 );

	const VColor& color() const { return m_color; }
	void setColor( const VColor& color ) { m_color = color; }

	double lineWidth() const { return m_lineWidth; }
	void setLineWidth( const double width ) { m_lineWidth = width; }

	VLineCap lineCap() const { return m_lineCap; }
	void setLineCap( const VLineCap cap ) { m_lineCap = cap; }

	VLineJoin lineJoin() const { return m_lineJoin; }
	void setLineJoin( const VLineJoin join ) { m_lineJoin = join; }

	float miterLimit() const { return m_miterLimit; }
	void setMiterLimit( float limit ) { m_miterLimit = limit; }

	// opacity:
	float opacity() const { return m_opacity; }
	void setOpacity( float opacity ) { m_opacity = opacity; }

	// dashes:
	const QValueList<float>& dashArray() const { return m_dashArray; }
	void setDashArray( const QValueList<float>& array )
		{ m_dashArray = array; }

	// dash offset:
	float dashOffset() const { return m_dashOffset; }
	void setDashOffset( float offset ) { m_dashOffset = offset; }

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

private:
	VColor m_color;
	float m_lineWidth;
	VLineCap m_lineCap;
	VLineJoin m_lineJoin;
	float m_miterLimit;
	float m_opacity;
	QValueList<float> m_dashArray;
	float m_dashOffset;
};

#endif
