/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VFILL_H__
#define __VFILL_H__

#include "vcolor.h"
#include "vgradient.h"


enum VFillRule{
	fillrule_evenOdd = 0,
	fillrule_winding = 1 };

enum VFillType{
	fill_none     = 0,
	fill_fill     = 1,
	fill_gradient = 2,
	fill_unknown  = 3 };


class QDomElement;

class VFill
{
public:
	VFill();

	const VColor& color() const { return m_color; }
	void setColor( const VColor& color ) { m_color = color; m_type = fill_fill; }

	VGradient& gradient() { return m_gradient; }

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

