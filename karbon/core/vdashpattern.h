/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VDASHPATTERN_H__
#define __VDASHPATTERN_H__

#include <qvaluelist.h>

class QDomElement;

class VDashPattern
{
public:
	VDashPattern( double dashOffset = 0.0 );

	const QValueList<float>& array() const { return m_array; }
	void setArray( const QValueList<float>& array )
		{ m_array = array; }

	// dash offset:
	float offset() const { return m_offset; }
	void setOffset( float offset ) { m_offset = offset; }

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

private:
	QValueList<float> m_array;
	float m_offset;
};

#endif
