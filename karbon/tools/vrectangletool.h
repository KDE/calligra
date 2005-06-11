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

#ifndef __VRECTANGLETOOL_H__
#define __VRECTANGLETOOL_H__


#include <klocale.h>
#include <kdialogbase.h>

#include "vshapetool.h"

class KarbonPart;
class QLabel;
class KoBuggyUnitDoubleSpinBox;

class VRectangleTool : public VShapeTool
{
public:
	VRectangleTool( KarbonPart *part );
	virtual ~VRectangleTool();

	virtual QString name() { return i18n( "Rectangle Tool" ); }
	virtual QString icon() { return "14_rectangle"; }

	virtual VPath* shape( bool interactive = false ) const;

	void refreshUnit();

	virtual bool showDialog() const;

private:
	class VRectangleOptionsWidget : public KDialogBase
	{
	public:
		VRectangleOptionsWidget( KarbonPart*part, QWidget* parent = 0L, const char* name = 0L );

		double width() const;
		double height() const;
		void setWidth( double value );
		void setHeight( double value );
		void refreshUnit();

	private:
		KoBuggyUnitDoubleSpinBox	*m_width;
		KoBuggyUnitDoubleSpinBox	*m_height;
		KarbonPart			*m_part;
		QLabel				*m_heightLabel;
		QLabel				*m_widthLabel;
	};

	VRectangleOptionsWidget *m_optionWidget;
};

#endif

