/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCOLOR_H__
#define __VCOLOR_H__

#include <qstring.h>
#include <qcolor.h>

class QDomElement;

/**
 * This class keeps track of color properties.
 * The actual color values can be represented in
 * rgb and hsv color spaces. Also each color has
 * a related opacity value.
 *
 * Default is opaque, rgb, black color.
 */
class VColor
{
public:
	enum VColorSpace
	{
		rgb  = 0,
		cmyk = 1,
		hsb  = 2,
		gray = 3
	};

	VColor();
	VColor( const VColor& color );
	VColor( const QRgb & );

	/// color values in all color spaces range from 0.0 to 1.0
	void pseudoValues( int& v1, int& v2, int& v3 ) const;

	float value( uint i ) const { return m_value[i]; }
	void values(
		float* v1 = 0L, float* v2 = 0L,
		float* v3 = 0L, float* v4 = 0L ) const;
	void setValues(
		const float* v1 = 0L, const float* v2 = 0L,
		const float* v3 = 0L, const float* v4 = 0L );

	/// opacity is a value ranging from 0.0(fully transparent) to 1.0(opaque)
	float opacity() const { return m_opacity; }
	void setOpacity( float opacity ) { m_opacity = opacity; }

	VColorSpace colorSpace() const { return m_colorSpace; }
	void setColorSpace( const VColorSpace colorSpace );

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

	/// convenience method
	QColor toQColor() const;

private:
	void convertToColorSpace( const VColorSpace colorSpace,
		float* v1 = 0L, float* v2 = 0L,
		float* v3 = 0L, float* v4 = 0L ) const;

	VColorSpace m_colorSpace;
	float m_value[4];
	float m_opacity;

	QString m_name;
};

#endif
