/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSTROKE_H__
#define __VSTROKE_H__

#include "vcolor.h"

class VStroke
{
public:
	enum VLineCap{
		cap_butt,
		cap_round,
		cap_square };

	enum VLineJoin{
		join_miter,
		join_round,
		join_bevel };

	VStroke( const double width = 1.0, const VLineCap cap = cap_butt,
		const VLineJoin join = join_miter );

	const VColor& color() const { return m_color; }
	void setColor( const VColor& color ) { m_color = color; }

	double lineWidth() const { return m_lineWidth; }
	void setLineWidth( const double width ) { m_lineWidth = width; }

	VLineCap lineCap() const { return m_lineCap; }
	void setLineCap( const VLineCap cap ) { m_lineCap = cap; }

	VLineJoin lineJoin() const { return m_lineJoin; }
	void setLineJoin( const VLineJoin join ) { m_lineJoin = join; }

private:
	VColor m_color;
	double m_lineWidth;
	VLineCap m_lineCap;
	VLineJoin m_lineJoin;
};

#endif
