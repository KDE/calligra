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

#ifndef __VSELECTNODES_H__
#define __VSELECTNODES_H__


#include <QRectF>

#include "vvisitor.h"
#include "vsegment.h"
//Added by qt3to4:
#include <Q3PtrList>

class VSelectNodes : public VVisitor
{
public:
	VSelectNodes( bool select = true, bool exclusive = true )
	{
		m_select = select;
		m_exclusive = exclusive;
	}

	VSelectNodes( const QRectF& rect, bool select = true, bool exclusive = true )
	{
		m_select = select;
		m_exclusive = exclusive;
		m_rect = rect;
	}

	virtual void visitVSubpath( VSubpath& path );
	virtual void visitVLayer( VLayer& layer );

private:
	bool m_select;
	bool m_exclusive;
	QRectF m_rect;
};

class VTestNodes : public VVisitor
{
public:
	VTestNodes( const QRectF& rect ) : m_rect( rect ) { m_segments.clear(); }

	virtual void visitVSubpath( VSubpath& path );
	virtual void visitVLayer( VLayer& layer );

	Q3PtrList<VSegment> &result() { return m_segments; }

private:
	QRectF m_rect;
	Q3PtrList<VSegment>	m_segments;
};

#endif


