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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __VCOLOR_H__
#define __VCOLOR_H__


#include <qcolor.h>
#include <qstring.h>
#include <koffice_export.h>
class QDomElement;


/**
 * This class keeps track of color properties.
 * The actual color values can be represented in
 * rgb and hsv color spaces. Also each color has
 * a related opacity value.
 *
 * Default is opaque, rgb, black color.
 */

class KARBONBASE_EXPORT VColor
{
public:
	enum VColorSpace
	{
		rgb  = 0,
		cmyk = 1,
		hsb  = 2,
		gray = 3
	};

	VColor( VColorSpace colorSpace = rgb );
	VColor( const VColor& color );
	VColor( const QColor& color );

	/**
	 * Cast operator to QColor.
	 */
	operator QColor() const;

	float operator[]( unsigned i ) const
		{ return m_value[i]; }

	void set( float v1 )
		{ m_value[0] = v1; }
	void set( float v1, float v2 )
		{ m_value[0] = v1; m_value[1] = v2; }
	void set( float v1, float v2, float v3 )
		{ m_value[0] = v1; m_value[1] = v2; m_value[2] = v3; }
	void set( float v1, float v2, float v3, float v4 )
		{ m_value[0] = v1; m_value[1] = v2; m_value[2] = v3; m_value[3] = v4; }

	/**
	 * Opacity is a value ranging from 0.0 (fully transparent) to 1.0 (opaque).
	 */
	float opacity() const { return m_opacity; }
	void setOpacity( float opacity ) { m_opacity = opacity; }

	VColorSpace colorSpace() const { return m_colorSpace; }
	void setColorSpace( const VColorSpace colorSpace, bool convert = true );

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

private:
	void convertToColorSpace( const VColorSpace colorSpace );

	VColorSpace m_colorSpace;

	float m_value[4];
	float m_opacity;

	QString m_name;
};

#endif
