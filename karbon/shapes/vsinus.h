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

#ifndef __VSINUS_H__
#define __VSINUS_H__

#include "vcomposite.h"
#include <koffice_export.h>

class KARBONBASE_EXPORT VSinus : public VPath
{
public:
	VSinus( VObject* parent, VState state = edit );
	VSinus( VObject* parent,
		const KoPoint& topLeft, double width, double height, uint periods );

	virtual QString name() const;

	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );

	virtual VPath* clone() const;

protected:
	void init();

private:
	KoPoint m_topLeft;
	double m_width;
	double m_height;
	uint m_periods;
};

#endif

