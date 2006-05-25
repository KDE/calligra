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

#ifndef __VROUNDRECTTOOL_H__
#define __VROUNDRECTTOOL_H__

#include <klocale.h>
#include <kdialogbase.h>
#include <knuminput.h>


#include "vshapetool.h"
//Added by qt3to4:
#include <QLabel>

class KarbonView;
class KarbonPart;
class QLabel;
class KoUnitDoubleSpinBox;


class VRoundRectTool : public VShapeTool
{
public:
	VRoundRectTool( KarbonView *view );
	virtual ~VRoundRectTool();

	virtual bool showDialog() const;
	virtual void setup(KActionCollection *collection);
	virtual QString uiname() { return i18n( "Round Rectangle Tool" ); }

	virtual VPath* shape( bool interactive = false ) const;

	void refreshUnit();

private:
	class VRoundRectOptionsWidget : public KDialogBase
	{
	public:
		VRoundRectOptionsWidget( KarbonPart *part, QWidget *parent = 0L, const char *name = 0L );

		double width() const;
		double height() const;
		double roundx() const;
		double roundy() const;
		void setWidth( double value );
		void setHeight( double value );
		void setRoundX( double value );
		void setRoundY( double value );
		void refreshUnit ();

	private:

		KoUnitDoubleSpinBox* m_width;
		KoUnitDoubleSpinBox* m_height;
		KoUnitDoubleSpinBox* m_roundx;
		KoUnitDoubleSpinBox* m_roundy;

		KarbonPart* m_part;

	};

	VRoundRectOptionsWidget* m_optionsWidget;
};

#endif

