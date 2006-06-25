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

#ifndef __VSINUSTOOL_H__
#define __VSINUSTOOL_H__

#include <kdialog.h>
#include "vshapetool.h"
//Added by qt3to4:
#include <QLabel>

class KoUnitDoubleSpinBox;
class KIntSpinBox;
class KarbonView;
class QLabel;

class VSinusTool : public VShapeTool
{
public:
	VSinusTool( KarbonView *view );
	virtual ~VSinusTool();

	virtual void setup(KActionCollection *collection);
	virtual bool showDialog() const;
	virtual QString uiname() { return i18n( "Sinus Tool" ); }

	virtual VPath *shape( bool interactive = false ) const;

	void refreshUnit();

private:
	class VSinusOptionsWidget : public KDialog
	{
	public:
		VSinusOptionsWidget( KarbonPart *part, QWidget *parent = 0L, const char *name = 0L );

		double width() const;
		double height() const;
		uint periods() const;
		void setWidth( double value );
		void setHeight( double value );
		void setPeriods( uint value );
		void refreshUnit();

	private:
		KoUnitDoubleSpinBox		*m_width;
		KoUnitDoubleSpinBox		*m_height;
		KIntSpinBox			*m_periods;
		KarbonPart			*m_part;
		QLabel				*m_heightLabel;
		QLabel				*m_widthLabel;
	};

	VSinusOptionsWidget *m_optionsWidget;
};

#endif

