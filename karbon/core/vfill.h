/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VFILL_H__
#define __VFILL_H__

#include "vcolor.h"

enum VFillRule { evenOdd = 0, winding = 1 };

class QDomElement;

class VFill
{
public:
	// paint server
    enum VPServerMode {
        pserver_none	= 0,
        pserver_fill	= 1,
        pserver_gradient= 2 };
	VFill();

	const VColor& color() const { return m_color; }
	void setColor( const VColor& color ) { m_color = color; m_mode = pserver_fill; }

	float opacity() const { return m_opacity; }
	void setOpacity( float opacity ) { m_opacity = opacity; }

	VPServerMode mode() const { return m_mode; }
	void setMode( VPServerMode mode ) { m_mode = mode; }

	VFillRule fillRule() const { return m_fillRule; }
	void setFillRule( VFillRule fillRule ) { m_fillRule = fillRule; }

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

private:
	VColor m_color;
	float m_opacity;
	VPServerMode m_mode;
	VFillRule m_fillRule;
};

#endif

