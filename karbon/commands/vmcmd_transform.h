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
	VMCmdTransform( KarbonPart* part, const VObjectList& objects, const QWMatrix &mat );
	virtual ~VMCmdTransform() {}

	virtual void execute();
	virtual void unexecute();

protected:
	VMCmdTransform( KarbonPart* part, const VObjectList& objects, const QString &name );

	VObjectList m_objects;
	QWMatrix m_mat;
};

class VMCmdTranslate : public VMCmdTransform
{
public:
	VMCmdTranslate( KarbonPart* part, const VObjectList& objects, double d1, double d2 );
};

class VMCmdScale : public VMCmdTransform
{
public:
	VMCmdScale( KarbonPart* part, const VObjectList& objects, const QPoint &p, double s1, double s2 );
};

class VMCmdShear : public VMCmdTransform
{
public:
	VMCmdShear( KarbonPart* part, const VObjectList& objects, const QPoint &p, double s1, double s2 );
};

class VMCmdRotate : public VMCmdTransform
{
public:
	VMCmdRotate( KarbonPart* part, const VObjectList& objects, const QPoint &p, double angle );
};

#endif
