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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __VRECTANGLE_H__
#define __VRECTANGLE_H__

#include "vcomposite.h"
#include <koffice_export.h>

class KARBONBASE_EXPORT VRectangle : public VPath
{
public:
	VRectangle( VObject* parent, VState state = edit );
	VRectangle( VObject* parent,
		const KoPoint& topLeft, double width, double height, double rx = 0.0, double ry = 0.0 );

	virtual QString name() const;

	virtual void save( QDomElement& element ) const;
	virtual void saveOasis( KoStore *store, KoXmlWriter *docWriter, KoGenStyles &mainStyles ) const;
	virtual void load( const QDomElement& element );
	virtual bool loadOasis( const QDomElement &element, KoOasisContext &context );

protected:
	void init();

private:
	KoPoint m_topLeft;
	double m_width;
	double m_height;
	double m_rx;
	double m_ry;
};

#endif

