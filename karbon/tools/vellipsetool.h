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

#ifndef __VELLIPSETOOL_H__
#define __VELLIPSETOOL_H__

#include <kdialogbase.h>

#include "vshapetool.h"

class KoUnitDoubleSpinBox;
class KComboBox;
class KIntSpinBox;
class KarbonView;
class QLabel;
class QWidget;

class VEllipseOptionsWidget : public KDialogBase
{
Q_OBJECT
public:
	VEllipseOptionsWidget( KarbonPart *part, QWidget *parent = 0L, const char *name = 0L );

	double width() const;
	double height() const;
	uint startAngle() const;
	uint endAngle() const;
	uint type() const;
	void setWidth( double value );
	void setHeight( double value );
	void refreshUnit();

public slots:
	void typeChanged( int );

private:
	KComboBox				*m_type;
	KIntSpinBox				*m_startAngle;
	KIntSpinBox				*m_endAngle;
	KoUnitDoubleSpinBox			*m_width;
	KoUnitDoubleSpinBox			*m_height;
	KarbonPart				*m_part;
	QLabel					*m_heightLabel;
	QLabel					*m_widthLabel;
};

class VEllipseTool : public VShapeTool
{
public:
	VEllipseTool( KarbonView *view );
	virtual ~VEllipseTool();

	virtual void setup(KActionCollection *collection);
	virtual bool showDialog() const;
	virtual QString uiname() { return i18n( "Ellipse Tool" ); }
	virtual VPath *shape( bool interactive = false ) const;
	void refreshUnit();

protected:
	void mouseDragRelease();
	void mouseMove();
	void mouseButtonPress();
	void mouseButtonRelease();
	void cancel();

private:
	enum { normal, startangle, endangle } m_state;
	VEllipseOptionsWidget *m_optionsWidget;
	double m_startAngle;
	double m_endAngle;
	KoPoint m_center;
};

#endif

