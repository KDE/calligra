/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
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

#ifndef __VCOMMAND_H__
#define __VCOMMAND_H__

#include <assert.h>

#include <kcommand.h>

#include "karbon_part.h"


class VDocument;

class VCommand : public KNamedCommand
{
public:
	VCommand( VDocument* doc, const QString& name )
		: KNamedCommand( name ), m_doc( doc )
	{
		assert( doc );
	}
	virtual ~VCommand() {}

	virtual void execute() = 0;
	virtual void unexecute() {}

protected:
	VDocument* m_doc;
};


class VCommandHistory : public KCommandHistory
{
public:
	VCommandHistory( KarbonPart* part )
		: KCommandHistory( part->actionCollection(), false ),
			m_part( part )
	{
	}

	virtual void undo();
	virtual void redo();

protected:
	KarbonPart* m_part;
};

#endif

