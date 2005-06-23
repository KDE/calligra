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

#ifndef __VTRANSFORMDOCKER_H__
#define __VTRANSFORMDOCKER_H__

#include "vdocker.h"

class KarbonPart;
class KarbonView;
class KoBuggyUnitDoubleSpinBox;

class VTransformDocker : public VDocker
{
	Q_OBJECT

public:
	VTransformDocker( KarbonPart* part, KarbonView* parent = 0L, const char* name = 0L );

public slots:
	void update();

private slots:
	void transform();

private:
	KarbonPart *m_part;
	KarbonView *m_view;
	KoBuggyUnitDoubleSpinBox *m_x;
	KoBuggyUnitDoubleSpinBox *m_y;
	KoBuggyUnitDoubleSpinBox *m_width;
	KoBuggyUnitDoubleSpinBox *m_height;
	QWidget *mainWidget;
};

#endif

