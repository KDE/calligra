/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSHAPECMD_H__
#define __VSHAPECMD_H__

#include "vcommand.h"

class VPath;

// Provides a common base class for creation commands since they all have
// a similar execute / unexecute behaviour and all build a VPath.
class VShapeCmd : public VCommand
{
public:
	VShapeCmd( VDocument* doc, const QString& name, VPath* path );
	virtual ~VShapeCmd() {}

	virtual void execute();
	virtual void unexecute();

protected:
	/// Pointer to the created shape.
	VPath* m_path;
};

#endif

