/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
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

protected:
	VMCmdTransform( KarbonPart* part, QPtrList<VObject> objects, const QString &name );

	QPtrList<VObject> m_objects;
	QWMatrix m_mat;
};

class VMCmdTranslate : public VMCmdTransform
{
public:
	VMCmdTranslate( KarbonPart* part, QPtrList<VObject> objects, double d1, double d2 );
};

class VMCmdScale : public VMCmdTransform
{
public:
	VMCmdScale( KarbonPart* part, QPtrList<VObject> objects, const QPoint &p, double s1, double s2 );
};

#endif
