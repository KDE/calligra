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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __VCOLOR_H__
#define __VCOLOR_H__


#include <QColor>
#include <QString>
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
		rgb  = 0,	/**< the RGB colorspace (red, green and blue components) */
		cmyk = 1,	/**< the CMYK colorspace (cyan, magenta, yellow and black components) */
		hsb  = 2,	/**< the HSB colorspace (hue, saturation and brightnes components) */
		gray = 3	/**< the Gray colorspace (gray from black to white) */
	};

	/**
	 * Constructs a new VColor with the specified colorspace.
	 *
	 * @param colorSpace the colorspace of the new color
	 */
	VColor( VColorSpace colorSpace = rgb );

	/**
	 * Constructs a new VColor by copying data from the specified VColor
	 *
	 * @param color the color to copy from 
	 */
	VColor( const VColor& color );

	/**
	 * Constructs a new VColor by copying data from the specified QColor
	 *
	 * @param color the color to copy from 
	 */
	VColor( const QColor& color );

	/**
	 * Cast operator to QColor.
	 */
	operator QColor() const;

	/**
	 * Index operator to access color components.
	 *
	 * @param i the index of the color component to access
	 * @return the requested color component
	 */
	float operator[]( unsigned i ) const
		{ return m_value[i]; }

	/**
	 * Sets the first color component.
	 *
	 * @param v1 the new value of the first color component
	 */
	void set( float v1 )
		{ m_value[0] = v1; }

	/**
	 * Sets the first and second color component.
	 *
	 * @param v1 the new value of the first color component
	 * @param v2 the new value of the second color component
	 */
	void set( float v1, float v2 )
		{ m_value[0] = v1; m_value[1] = v2; }

	/**
	 * Sets the first, second and third color component.
	 *
	 * @param v1 the new value of the first color component
	 * @param v2 the new value of the second color component
	 * @param v3 the new value of the third color component
	 */
	void set( float v1, float v2, float v3 )
		{ m_value[0] = v1; m_value[1] = v2; m_value[2] = v3; }

	/**
	 * Sets the first, second, third and fourth color component.
	 *
	 * @param v1 the new value of the first color component
	 * @param v2 the new value of the second color component
	 * @param v3 the new value of the third color component
	 * @param v4 the new value of the fourth color component
	 */
	void set( float v1, float v2, float v3, float v4 )
		{ m_value[0] = v1; m_value[1] = v2; m_value[2] = v3; m_value[3] = v4; }

	/**
	 * Returns the color opacity.
	 *
	 * Opacity is a value ranging from 0.0 (fully transparent) to 1.0 (opaque).
	 *
	 * @return the color opacity
	 */
	float opacity() const { return m_opacity; }

	/**
	 * Sets the color opacity.
	 *
	 * @param opacity the new color opacity.
	 */
	void setOpacity( float opacity ) { m_opacity = opacity; }

	/**
	 * Returns the color's colorspace.
	 *
	 * @return the color's colorspace
	 */
	VColorSpace colorSpace() const { return m_colorSpace; }

	/**
	 * Sets the color's colorspace.
	 *
	 * The color is converted into the new colorspace by setting convert = true.
	 * 
	 * @param colorSpace the new colorspace
	 * @param convert controls if color is converted into new colorspace
	 */
	void setColorSpace( const VColorSpace colorSpace, bool convert = true );

	/**
	 * Save this color's state to xml.
	 * 
	 * @param element the DOM element to which the attributes are saved
	 */
	void save( QDomElement& element ) const;

	/**
	 * Load this color's state from xml and initialize it accordingly.
	 *
	 * @param element the DOM element from which the attributes are read
	 */
	void load( const QDomElement& element );

private:
	void convertToColorSpace( const VColorSpace colorSpace );

	VColorSpace m_colorSpace;

	float m_value[4];
	float m_opacity;

	QString m_name;
};

#endif
