/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCCOMMAND_H__
#define __VCCOMMAND_H__

#include <vcommand.h>

class VPath;
// Provides a common base class for creation commands since they all have
// a similar execute / unexecute behaviour and all build a VPath.
class VCCommand : public VCommand
{
public:
	VCCommand( KarbonPart* part, const QString& name )
		: VCommand( part, name ), m_object( 0L ) {}
	virtual ~VCCommand() {}

	virtual void execute();
	virtual void unexecute();

	// for complex shapes. needed to draw while creation (creation tool):
	virtual VPath* createPath() = 0;

protected:
	VPath* m_object;
};

#endif
