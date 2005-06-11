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

#ifndef __VPOLYGONTOOL_H__
#define __VPOLYGONTOOL_H__

#include <kdialogbase.h>

#include "vshapetool.h"

class KoBuggyUnitDoubleSpinBox;
class KIntSpinBox;


class VPolygonTool : public VShapeTool
{
public:
	VPolygonTool( KarbonPart *part );
	virtual ~VPolygonTool();

	virtual QString name() { return i18n( "Polygon Tool" ); }
	virtual bool showDialog() const;
	
	virtual VPath *shape( bool interactive = false ) const;

	void refreshUnit();

	virtual void arrowKeyReleased( Qt::Key );

private:
	class VPolygonOptionsWidget : public KDialogBase
	{
	public:
		VPolygonOptionsWidget( KarbonPart *part, QWidget *parent = 0L, const char *name = 0L );

		double radius() const;
		uint edges() const;
		void setRadius( double value );
		void setEdges( uint value );

		void refreshUnit();

	private:
		KoBuggyUnitDoubleSpinBox	*m_radius;
		KIntSpinBox			*m_edges;
		KarbonPart          *m_part;
	};

	VPolygonOptionsWidget* m_optionsWidget;
};

#endif

