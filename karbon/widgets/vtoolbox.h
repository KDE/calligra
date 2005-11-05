/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __VTOOLBOX_H__
#define __VTOOLBOX_H__
#include <qtoolbutton.h>
#include <qptrvector.h>

#include <ktoolbar.h>
class QWidget;
class KarbonPart;
class VStrokeFillPreview;
class Vtool;
class VTypeButtonBox;


/// This class...

class VToolBox : public KToolBar
{
	Q_OBJECT

public:
	VToolBox( KarbonPart* part, KMainWindow *mainWin, const char* name = 0L );

	VStrokeFillPreview *strokeFillPreview();

	void registerTool( VTool * );
	void setupTools();

signals:
	void activeToolChanged( VTool * );

public slots:
	virtual void setOrientation ( Orientation o );
	void slotButtonPressed( int id );
	void slotPressButton( int id );

private:
	KarbonPart *m_part;

	VStrokeFillPreview  *m_strokeFillPreview;
	VTypeButtonBox      *m_typeButtonBox;

	QBoxLayout          *leftLayout;
	QBoxLayout          *rightLayout;
	QBoxLayout          *columnsLayouter;

	QWidget             *left;
	QWidget             *right;

	QButtonGroup        *buttonGroup;

	bool                 insertLeft;

	QToolButton *addButton( const char* iconName, QString tooltip, int id );
	// tools:
	QPtrVector<VTool> m_misctools;
	QPtrVector<VTool> m_shapetools;
	QPtrVector<VTool> m_manipulationtools;
};

#endif

