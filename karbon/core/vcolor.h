/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCOLOR_H__
#define __VCOLOR_H__

#include <qstring.h>

class VColor
{
public:
	enum VColorSpace{
		gray,
		rgb,
		hsb,
		cmyk };

	VColor();

	void values( VColorSpace colorSpace,
		int* v1 = 0L, int* v2 = 0L, int* v3 = 0L, int* v4 = 0L ) const;

	VColorSpace colorSpace() const { return m_colorSpace; }
	void setColorSpace( VColorSpace colorSpace );

private:
	VColorSpace m_colorSpace;
	double m_value[4];

	QString m_name;
};

#endif
