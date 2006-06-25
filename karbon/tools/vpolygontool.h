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

#ifndef __VPOLYGONTOOL_H__
#define __VPOLYGONTOOL_H__

#include <kdialog.h>

#include "vshapetool.h"

class KoUnitDoubleSpinBox;
class KIntSpinBox;


class VPolygonTool : public VShapeTool
{
public:
	VPolygonTool( KarbonView *view );
	virtual ~VPolygonTool();

	virtual void setup(KActionCollection *collection);
	virtual bool showDialog() const;
	virtual QString uiname() { return i18n( "Polygon Tool" ); }

	virtual VPath *shape( bool interactive = false ) const;

	void refreshUnit();

	virtual void arrowKeyReleased( Qt::Key );

private:
	class VPolygonOptionsWidget : public KDialog
	{
	public:
		VPolygonOptionsWidget( KarbonView *view, QWidget *parent = 0L, const char *name = 0L );

		double radius() const;
		uint edges() const;
		void setRadius( double value );
		void setEdges( uint value );

		void refreshUnit();

	private:
		KoUnitDoubleSpinBox		*m_radius;
		KIntSpinBox			*m_edges;
		KarbonView		        *m_view;
	};

	VPolygonOptionsWidget* m_optionsWidget;
};

#endif

