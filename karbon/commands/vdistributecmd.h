/* This file is part of the KDE project
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

#ifndef __VDISTRIBUTECMD_H__
#define __VDISTRIBUTECMD_H__

#include "vcommand.h"

class VTranslateCmd;

/** A command for distributing objects */

class VDistributeCmd : public VCommand
{
public:
	enum Distribute
	{
		DISTRIBUTE_HORIZONTAL_CENTER,
		DISTRIBUTE_HORIZONTAL_GAP,
		DISTRIBUTE_HORIZONTAL_LEFT,
		DISTRIBUTE_HORIZONTAL_RIGHT,
		DISTRIBUTE_VERTICAL_CENTER,
		DISTRIBUTE_VERTICAL_GAP,
		DISTRIBUTE_VERTICAL_BOTTOM,
		DISTRIBUTE_VERTICAL_TOP
	};
	VDistributeCmd( VDocument *doc, Distribute distribute );
	virtual ~VDistributeCmd();

	virtual void execute();
	virtual void unexecute();
	virtual bool changesSelection() const { return true; }

protected:
	double getAvailableSpace( VObject *first, VObject *last, double extent );

	Distribute				m_distribute;
	QPtrList<VTranslateCmd>	m_trafoCmds;
};

#endif

