/* This file is part of the KDE project
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

#ifndef __VTOOLCONTAINER_H__
#define __VTOOLCONTAINER_H__

#include <qptrdict.h>
#include <qtoolbar.h>

#include <ksharedptr.h>

class KarbonPart;
class KoMainWindow;
class KoView;
class QButtonGroup;
class VStrokeFillPreview;

class VToolContainer : public QToolBar, public KShared
{
	Q_OBJECT

public:
	static VToolContainer* instance( KarbonPart* part, KoView* parent = 0L,
		const char* name = 0L );
	~VToolContainer();

	VStrokeFillPreview* strokeFillPreview() { return m_strokeFillPreview; }

signals:
	// shape tools:
	void ellipseToolActivated();
	void polygonToolActivated();
	void rectangleToolActivated();
	void rotateToolActivated();
	void roundRectToolActivated();
	void selectToolActivated();
	void selectNodesToolActivated();
	void shearToolActivated();
	void sinusToolActivated();
	void solidFillActivated();
	void spiralToolActivated();
	void starToolActivated();
	void strokeActivated();
	void strokeChanged( const VStroke & );
	void fillChanged( const VFill& );
	void textToolActivated();
	void gradToolActivated();

private:
	QButtonGroup* btngroup;
	//QButtonGroup* dlggroup;
	VStrokeFillPreview* m_strokeFillPreview;
	enum ButtonChoice
	{
		Select, SelectNodes, Rotate, Shear, Ellipse, Rectangle,
		Roundrect, Polygon, Star, Sinus, Spiral, Text, Grad
	};

	KarbonPart *m_part;

	enum DlgChoice { Outline, SolidFill, Gradient };

	static QPtrDict< VToolContainer > m_containers;

	VToolContainer( KarbonPart *part, KoView* parent = 0L, const char* name = 0L );
};

#endif

