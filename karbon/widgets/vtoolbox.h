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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __VTOOLBOX_H__
#define __VTOOLBOX_H__
#include <qtoolbutton.h>

#include <ktoolbar.h>
class QWidget;
class KarbonPart;
class VStrokeFillPreview;

class VToolBox : public KToolBar
{
	Q_OBJECT

public:
	VToolBox( KarbonPart* part, KMainWindow *mainWin, const char* name = 0L );

	VStrokeFillPreview *strokeFillPreview();

signals:
	void selectToolActivated();
	void rotateToolActivated();
	void selectNodesToolActivated();
	void shearToolActivated();
	void ellipseToolActivated();
	void polygonToolActivated();
	void polylineToolActivated();
	void rectangleToolActivated();
	void roundRectToolActivated();
	void sinusToolActivated();
	void spiralToolActivated();
	void starToolActivated();
	void gradToolActivated();
	void clipartToolActivated();

public slots:
	virtual void setOrientation ( Orientation o );

private:
	KarbonPart			*m_part;
	VStrokeFillPreview	*m_strokeFillPreview;
	QBoxLayout			*leftLayout,*rightLayout,*columnsLayouter;
	QWidget				*left, *right;
	QButtonGroup		*buttonGroup;
	bool				insertLeft;

	QToolButton *addButton(const char* iconName, QString tooltip);
};

#endif

