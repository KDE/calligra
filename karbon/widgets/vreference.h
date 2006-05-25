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

/* This is the color slider widget that is used to select color or color components */

/* vreference.h */
#ifndef VREFERENCE_H
#define VREFERENCE_H

#include <q3frame.h>

class Q3ButtonGroup;

enum Choice
{
	TopLeft,
	Left,
	BottomLeft,
	Top,
	Center,
	Bottom,
	TopRight,
	Right,
	BottomRight
};

class VReference : public Q3Frame
{
	Q_OBJECT
public:
	VReference( QWidget *parent = 0L, const char *name = 0L );
	int referencePoint();
public slots:
	virtual void setReferencePoint ( int );

private:
	int m_referencePoint;
	Q3ButtonGroup* mButtonGroup;
signals:
	void referencePointChanged( int referencePoint );
};

#endif

