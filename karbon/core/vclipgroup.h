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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __VCLIP_H__
#define __VCLIP_H__

#include <qdom.h>

#include "vgroup.h"
#include "vobject.h"
#include "vvisitor.h"
#include "vpath.h"

class VPainter;

/**
 * Base class for clipping conglomerates
 * the first child elemt is used for clipping
 */

class VClipGroup : public VGroup
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
	PathRenderer( VPainter *p_painter);

	virtual ~PathRenderer();
private:
        VPainter *m_painter;

protected:
	virtual void visitVPath( VPath& path );

};

#endif
