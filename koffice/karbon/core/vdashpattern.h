/* This file is part of the KDE project
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

#ifndef __VDASHPATTERN_H__
#define __VDASHPATTERN_H__

#include <qvaluelist.h>
#include <koffice_export.h>

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

class KARBONBASE_EXPORT VDashPattern
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

