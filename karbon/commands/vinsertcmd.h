/* This file is part of the KDE project
   Copyright (C) 2004, Inge Wallin

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

#ifndef __VINSERTCMD_H__
#define __VINSERTCMD_H__

#include "vcommand.h"
#include "vgroup.h"




// Insert object(s)

class VInsert;

class VInsertCmd : public VCommand
{
public:
	VInsertCmd( VDocument *doc, const QString& name,
				VObjectList *objects, double offset );
	virtual ~VInsertCmd();

	virtual void execute();
	virtual void unexecute();

protected:
	VObjectList  m_objects;
	double       m_offset;
};

#endif

