/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VMCMDTRANSFORM_H__
#define __VMCMDTRANSFORM_H__

#include "vcommand.h"

// Transform object(s) with a specified matrix and allow undo.

class VPath;
class QWMatrix;

class VMCmdTransform : public VCommand
{
public:
	VMCmdTransform( KarbonPart* part, QPtrList<VObject> objects, const QWMatrix &mat );
	virtual ~VMCmdTransform() {}

	virtual void execute();
	virtual void unexecute();

private:
	QPtrList<VObject> m_objects;
	QWMatrix m_mat;
};

#endif
