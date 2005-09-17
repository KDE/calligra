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

#ifndef __VDOCKER_H__
#define __VDOCKER_H__

#ifdef Always
#undef Always
#undef Never
#undef KeyPress
#undef KeyRelease
#endif


#include <qdockwindow.h>
#include <koUnit.h>

/* This is the base class for all Karbon14 dockers. Smaller font is used to save screen space
and allow more dockers on screen at the same time */

class QWidget;

class VDocker : public QDockWindow
{
	Q_OBJECT

public:
	VDocker ( QWidget* parent = 0L, const char* name = 0L );
	
public slots:
	/**
	* Sets new unit.
	* @param unit the new unit
	*/
	virtual void setUnit( KoUnit::Unit /*unit*/ ) {}
};

#endif

