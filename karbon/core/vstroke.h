/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSTROKE_H__
#define __VSTROKE_H__

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

	VStroke( const double width = 1.0, const VLineCap cap = cap_butt,
		const VLineJoin join = join_miter, const double miterLimit = 10.0 );

	const VColor& color() const { return m_color; }
	void setColor( const VColor& color ) { m_color = color; }

	double lineWidth() const { return m_lineWidth; }
	void setLineWidth( const double width ) { m_lineWidth = width; }

	VLineCap lineCap() const { return m_lineCap; }
	void setLineCap( const VLineCap cap ) { m_lineCap = cap; }

	VLineJoin lineJoin() const { return m_lineJoin; }
	void setLineJoin( const VLineJoin join ) { m_lineJoin = join; }

	// opacity in the range 0-255
	unsigned short opacity() const { return m_opacity; }
	void setOpacity( unsigned  short opacity ) { m_opacity = opacity; }

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

private:
	VColor m_color;
	double m_lineWidth;
	VLineCap m_lineCap;
	VLineJoin m_lineJoin;
	double m_miterLimit;
	unsigned short m_opacity;
	// TODO: dash
};

#endif
