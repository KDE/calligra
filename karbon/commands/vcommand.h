/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
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
	VCommand( VDocument *doc, const QString& name )
		: KNamedCommand( name ), m_doc( doc )
	{
		assert( doc );
	}
	virtual ~VCommand() {}

	virtual void execute() = 0;
	virtual void unexecute() {}

protected:
	VDocument *m_doc;
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

