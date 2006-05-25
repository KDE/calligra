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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __VSTATEBUTTON_H__
#define __VSTATEBUTTON_H__

#include <QPushButton>
#include <qpixmap.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <QMouseEvent>

class VStateButton : public QPushButton
{
public:
	VStateButton( QWidget* parent = 0L, const char* name = 0L );
	~VStateButton();

	void addState( QPixmap *state ) { m_pixmaps.append( state ); }
	void setState( unsigned int index );
	unsigned int getState() const { return m_index; }

private:
	void mouseReleaseEvent( QMouseEvent * );

	Q3PtrList<QPixmap> m_pixmaps;
	unsigned int m_index;
};

#endif

