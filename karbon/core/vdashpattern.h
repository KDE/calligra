/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VDASHPATTERN_H__
#define __VDASHPATTERN_H__

#include <qvaluelist.h>


class QDomElement;


/**
 * The dash pattern consistes of a sequence of on/off values.
 * For example 10 5 5 10 would result in a dash of 10 pixels,
 * next 5 pixels no dash, then 5 pixels of dashing, finally 10 pixels
 * of no dash. This sequence is repeated until the whole outline is dashed.
 *
 * Also it supports an offset value for when to start the dashing.
 *
 * Default is no dashes.
 */

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

