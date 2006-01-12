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

#ifndef __VDRAWSELECTION_H__
#define __VDRAWSELECTION_H__

#include "vgroup.h"
#include "vvisitor.h"
#include <koffice_export.h>
/**
 *  Helper class to draw the outline of a composite path, including (?)
 *  optionally its bezier helper lines, depending on the state.
 */
class KARBONBASE_EXPORT VDrawSelection : public VVisitor
{
public:
	VDrawSelection( const VObjectList& selection, VPainter *painter, bool nodeediting = false, uint nodeSize = 2 )
		: m_selection( selection ), m_painter( painter ), m_nodeediting( nodeediting ), m_nodeSize( nodeSize ) {}

	virtual void visitVPath( VPath& composite );

private:
	VObjectList		m_selection;
	VPainter		*m_painter;
	bool			m_nodeediting;
	uint			m_nodeSize;
};

#endif

