/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VFILL_H__
#define __VFILL_H__

#include "vcolor.h"

class QDomElement;

class VFill
{
public:
	VFill();

	const VColor& color() const { return m_color; }
	void setColor( const VColor& color ) { m_color = color; }

	float opacity() const { return m_opacity; }
	void setOpacity( float opacity ) { m_opacity = opacity; }

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

private:
	VColor m_color;
	float m_opacity;
};

#endif

