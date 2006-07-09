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

#ifndef __VTYPEBUTTONBOX_H__
#define __VTYPEBUTTONBOX_H__

#include <qhbuttongroup.h>

class KarbonPart;

class VTypeButtonBox : public QHButtonGroup
{
	Q_OBJECT

public:
	enum buttonType {
		none     = 0,
		solid    = 1,
		gradient = 2,
		pattern  = 3
 	};

	VTypeButtonBox( KarbonPart *part, QWidget* parent = 0L, const char* name = 0L );
	bool isStrokeManipulator() { return m_isStrokeManipulator; }

public slots:
	void slotButtonPressed( int id );
	void setFill();
	void setStroke();

private:
	KarbonPart  *m_part;
	bool         m_isStrokeManipulator;

	void manipulateFills( int id );
	void manipulateStrokes( int id );
};

#endif

