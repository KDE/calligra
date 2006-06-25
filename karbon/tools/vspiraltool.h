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

#ifndef __VSPIRALTOOL_H__
#define __VSPIRALTOOL_H__

#include <kdialog.h>
#include <klocale.h>

#include "vshapetool.h"

class KComboBox;
class KoUnitDoubleSpinBox;
class KDoubleNumInput;
class KIntSpinBox;
class KarbonView;

class VSpiralTool : public VShapeTool
{
public:
	VSpiralTool( KarbonView *view );
	virtual ~VSpiralTool();

	virtual void setup(KActionCollection *collection);
	virtual bool showDialog() const;
	virtual QString uiname() { return i18n( "Spiral Tool" ); }

	virtual VPath* shape( bool interactive = false ) const;

	void refreshUnit();

	virtual void arrowKeyReleased( Qt::Key );

private:
	class VSpiralOptionsWidget : public KDialog
	{
	public:
		VSpiralOptionsWidget( KarbonPart *part, QWidget *parent = 0L, const char* name = 0L );

		double radius() const;
		uint segments() const;
		double fade() const;
		uint type() const;
		bool clockwise() const;
		void setRadius( double value );
		void setSegments( uint value );
		void setFade( double value );
		void setClockwise( bool value );

		void refreshUnit();

	private:
		KoUnitDoubleSpinBox		*m_radius;
		KIntSpinBox			*m_segments;
		KDoubleNumInput			*m_fade;
		KComboBox			*m_type;
		KComboBox			*m_clockwise;
		KarbonPart			*m_part;
	};

	VSpiralOptionsWidget *m_optionsWidget;
};

#endif

