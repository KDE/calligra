/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
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

#ifndef __VRECTANGLETOOL_H__
#define __VRECTANGLETOOL_H__


#include <qgroupbox.h>

#include <klocale.h>

#include "vshapetool.h"

class KarbonPart;
class QLabel;
class KoUnitDoubleSpinBox;


class VRectangleTool : public VShapeTool
{
public:
	VRectangleTool( KarbonView* view );
	virtual ~VRectangleTool();

	virtual QWidget* optionsWidget() { return m_optionsWidget; }
	virtual QString name() { return i18n( "Rectangle tool" ); }
	virtual QString icon() { return "14_rectangle"; }

	virtual VComposite* shape( bool interactive = false ) const;

    void refreshUnit();

private:
	class VRectangleOptionsWidget : public QGroupBox
	{
	public:
		VRectangleOptionsWidget( KarbonPart*part, QWidget* parent = 0L, const char* name = 0L );

		double width() const;
		double height() const;
		void setWidth( double value );
		void setHeight( double value );
		void refreshUnit();

	private:
		KoUnitDoubleSpinBox	*m_width;
		KoUnitDoubleSpinBox	*m_height;
		KarbonPart			*m_part;
		QLabel				*m_heightLabel;
		QLabel				*m_widthLabel;
	};

	VRectangleOptionsWidget* m_optionsWidget;
};

#endif

