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

#ifndef __VCLIP_H__
#define __VCLIP_H__

#include "vgroup.h"
#include "vvisitor.h"
#include <koffice_export.h>
class VPainter;

/**
 * Base class for clipping conglomerates
 * the first child element is used for clipping
 */

class KARBONBASE_EXPORT VClipGroup : public VGroup
{
public:
	VClipGroup( VObject* parent, VState state = normal );
	VClipGroup ( const VClipGroup& group );

	virtual ~VClipGroup();

	virtual void draw( VPainter* painter, const KoRect* rect = 0L ) const;

	virtual VClipGroup* clone() const;

	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );
};


class PathRenderer : public VVisitor
{
public:
	PathRenderer( VPainter *p_painter );

	virtual ~PathRenderer();

protected:
	virtual void visitVSubpath( VSubpath& path );

private:
	VPainter *m_painter;
};

#endif
