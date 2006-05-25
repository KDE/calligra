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

#ifndef __VSTARTOOL_H__
#define __VSTARTOOL_H__

#include <klocale.h>
#include <kdialogbase.h>

#include "vshapetool.h"
//Added by qt3to4:
#include <QLabel>


class KoUnitDoubleSpinBox;
class KIntSpinBox;
class KComboBox;
class KarbonView;

class VStarOptionsWidget : public KDialogBase
{
Q_OBJECT
public:
	VStarOptionsWidget( KarbonPart *part, QWidget* parent = 0L, const char* name = 0L );

	void refreshUnit();

	int edges() const;
	double innerRadius() const;
	double outerRadius() const;
	double roundness() const;
	uint type() const;
	uint innerAngle() const;
	void setInnerRadius( double );

public slots:
	void typeChanged( int );
	void setEdges( int );
	void setOuterRadius( double );

private:
	KoUnitDoubleSpinBox		*m_innerR;
	KoUnitDoubleSpinBox		*m_outerR;
	KDoubleNumInput			*m_roundness;
	KIntSpinBox			*m_edges;
	KIntSpinBox			*m_innerAngle;
	KComboBox			*m_type;
	KarbonPart 			*m_part;
	QLabel				*m_innerRLabel;
	QLabel				*m_outerRLabel;
};

class VStarTool : public VShapeTool
{
public:
	VStarTool( KarbonView *view );
	virtual ~VStarTool();

	virtual bool showDialog() const;
	virtual void setup(KActionCollection *collection);
	virtual QString uiname() { return i18n( "Star Tool" ); }
	virtual VPath* shape( bool interactive = false ) const;

	void refreshUnit();

	virtual void arrowKeyReleased( Qt::Key );

private:
	VStarOptionsWidget* m_optionsWidget;
};

#endif

