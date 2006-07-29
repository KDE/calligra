/* This file is part of the KDE project
   Copyright (C) 2002, 2003 The Karbon Developers

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

#ifndef __VZORDERCMD_H__
#define __VZORDERCMD_H__

#include "vcommand.h"

class VSelection;

/**
 * Command that changes the z-order of the selection within a layer
 * to front, back, one step up, or one step down.
 */
class VZOrderCmd : public VCommand
{
public:
	enum VOrder
	{
		bringToFront	= 0,
		up				= 1,
		down			= 2,
		sendToBack		= 3
	};

	VZOrderCmd( VDocument* doc, VOrder );
	VZOrderCmd( VDocument* doc, VObject *obj, VOrder );
	virtual ~VZOrderCmd();

	virtual void execute();
	virtual void unexecute();
	virtual bool isExecuted() { return true; }

protected:
	VSelection *m_selection;
	VOrder		m_state;
};

#endif

