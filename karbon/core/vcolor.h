/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCOLOR_H__
#define __VCOLOR_H__

#include <qstring.h>

class QDomElement;
class QColor;


class VColor
{
public:
	enum VColorSpace{
		rgb  = 0,
		cmyk = 1,
		hsb  = 2,
		gray = 3 };

	VColor();

	void pseudoValues( int& v1, int& v2, int& v3 ) const;
	void values(
		float* v1 = 0L, float* v2 = 0L,
		float* v3 = 0L, float* v4 = 0L ) const;
	void setValues(
		const float* v1 = 0L, const float* v2 = 0L,
		const float* v3 = 0L, const float* v4 = 0L );

	float opacity() const { return m_opacity; }
	void setOpacity( float opacity ) { m_opacity = opacity; }

	VColorSpace colorSpace() const { return m_colorSpace; }
	void setColorSpace( const VColorSpace colorSpace );

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

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
