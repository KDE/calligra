/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCOMMAND_H__
#define __VCOMMAND_H__

#include <assert.h>
#include <kcommand.h>

#include "karbon_part.h"

class VCommand : public KCommand
{
public:
	VCommand( KarbonPart* part, const QString& name )
		: KCommand( name ), m_part( part ) { assert( part ); }
	virtual ~VCommand() {}

	virtual void execute() = 0;
	virtual void unexecute() {}

protected:
	KarbonPart* m_part;
};

class VCommandHistory : public KCommandHistory
{
public:
	VCommandHistory( KarbonPart* part )
		: KCommandHistory( part->actionCollection(), false ), m_part( part ) {}

//	virtual void undo() {}
//	virtual void redo() {}

protected:
	KarbonPart* m_part;
};

#endif
