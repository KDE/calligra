/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSHAPECMD_H__
#define __VSHAPECMD_H__

#include "vcommand.h"

class VObject;

// Provides a common base class for creation commands since they all have
// a similar execute / unexecute behaviour and all build a VPath.
class VShapeCmd : public VCommand
{
public:
	VShapeCmd( VDocument *doc, const QString& name )
		: VCommand( doc, name ), m_object( 0L ) {}
	virtual ~VShapeCmd() {}

	virtual void execute();
	virtual void unexecute();

	/// Create a temporary object for displaying while shape creation.
	virtual VObject* createPath() { return 0L; }

protected:
	/// Pointer to the created object. Used for undo/redo.
	VObject* m_object;
};

#endif

