/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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

#ifndef __VSHADOWDECORATOR_H__
#define __VSHADOWDECORATOR_H__


#include "vobject.h"
#include <koffice_export.h>

/**
 */
class KARBONBASE_EXPORT VShadowDecorator : public VObject
{
public:
	VShadowDecorator( VObject* object, VObject* parent, int distance = 2, int angle = 0, float opacity = 1.0 );
	VShadowDecorator( const VShadowDecorator& obj );

	virtual ~VShadowDecorator();

	virtual void draw( VPainter* /*painter*/, const KoRect* /*rect*/ = 0L ) const;

	virtual const KoRect& boundingBox() const { return m_object->boundingBox(); }
	VStroke* stroke() const { return m_object->stroke(); }
	virtual void setStroke( const VStroke& stroke );
	VFill* fill() const { return m_object->fill(); }
	virtual void setFill( const VFill& fill );

	virtual void accept( VVisitor& /*visitor*/ );

	virtual void save( QDomElement& ) const;
	virtual void load( const QDomElement& ) {}

	virtual VObject* clone() const;
	
	VState state() const { return m_state; }
	virtual void setState( const VState state );
	
	void setShadow( int distance = 2, int angle = 0, float opacity = 1.0 );
protected:
	VObject *m_object;
	int		m_distance;
	int		m_angle;
	float	m_opacity;
};

#endif

