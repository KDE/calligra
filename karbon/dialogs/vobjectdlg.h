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

#ifndef __VOBJECTDLG_H__
#define __VOBJECTDLG_H__

#include <qdockwindow.h>

class QGrid;

class KDoubleNumInput;

class KarbonPart;
class TKUFloatSpinBox;
class VStroke;

// Small panel displaying selected object(s) geometry. Can be used for quick numerical modifications of objects
class VObjectDlg : public QDockWindow
{
	Q_OBJECT

public:
	VObjectDlg( KarbonPart* part, KoView* parent = 0L, const char* name = 0L );
	virtual ~VObjectDlg();
	void enable();
	void disable();
	void reset();
	void update( KarbonPart* part = 0L );

private:
	QGrid *mainLayout;
	KarbonPart *m_part;
	KDoubleNumInput *m_X;
	KDoubleNumInput *m_Y;
	KDoubleNumInput *m_Width;
	KDoubleNumInput *m_Height;
	KDoubleNumInput *m_Rotation;
	TKUFloatSpinBox *m_setLineWidth;
	VStroke m_stroke;
	
private slots:
	void xChanged( double x );
	void yChanged( double y );
	void widthChanged( double width );
	void heightChanged( double height );
	void lineWidthChanged( double width );
};

#endif

