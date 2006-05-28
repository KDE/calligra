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

#ifndef __VPOLYGON_H__
#define __VPOLYGON_H__

#include "vcomposite.h"

#include <QPointF>

class VPolygon : public VPath
{
public:
	VPolygon( VObject* parent, VState state = edit );
	VPolygon( VObject* parent, const QString &points,
		const QPointF& topLeft, double width, double height );

	virtual QString name() const;

	virtual void save( QDomElement& element ) const;
	virtual void saveOasis( KoStore *store, KoXmlWriter *docWriter, KoGenStyles &mainStyles, int &index ) const;
	virtual void load( const QDomElement& element );
	virtual bool loadOasis( const QDomElement &element, KoOasisLoadingContext &context );

	virtual VPath* clone() const;

protected:
	void init();

private:
	QPointF m_topLeft;
	double m_width;
	double m_height;
	QString m_points;
};

#endif

