/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.t-com.hr)
   Copyright (C) 2005, The Karbon Developers

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

/* This is a small widget used on the statusbar, to display fill/stroke colors etc. */

/* vsmallpreview.h */
#ifndef VSMALLPREVIEW_H
#define VSMALLPREVIEW_H

#include <QWidget>

class QFrame;
class QLabel;
class VFill;
class VStroke;

class VSmallPreview : public QWidget
{
	Q_OBJECT
public:
	VSmallPreview( QWidget* parent = 0L, const char* name = 0L );
	~VSmallPreview();

	void update( const VStroke &, const VFill & );

protected:
	virtual void paintEvent( QPaintEvent* event );

private:
	void drawFill( const VFill & );
	void drawStroke( const VStroke & );
	QFrame *m_fillFrame;
	QFrame *m_strokeFrame;
	QLabel *m_fillLabel;
	QLabel *m_strokeLabel;
	VFill m_fill;
	VStroke m_stroke;
};

#endif

