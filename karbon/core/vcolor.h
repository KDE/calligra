/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCOLOR_H__
#define __VCOLOR_H__

#include <qstring.h>

class VColor
{
public:
	enum VColorSpace{
		rgb,
		cmyk,
		hsb,
		gray };

	VColor();

	void pseudoValues( int& v1, int& v2, int& v3 ) const;
	void values(
		double* v1 = 0L, double* v2 = 0L,
		double* v3 = 0L, double* v4 = 0L ) const;

	VColorSpace colorSpace() const { return m_colorSpace; }
	void setColorSpace( const VColorSpace colorSpace );

private:
	bool convertToColorSpace( const VColorSpace colorSpace,
		double* v1 = 0L, double* v2 = 0L,
		double* v3 = 0L, double* v4 = 0L ) const;

	VColorSpace m_colorSpace;
	double m_value[4];

	QString m_name;
};

#endif
