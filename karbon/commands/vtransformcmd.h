/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VTRANSFORMCMD_H__
#define __VTRANSFORMCMD_H__

#include "vcommand.h"

// Transform object(s) with a specified matrix and allow undo.

class QWMatrix;
class VPath;

class VTransformCmd : public VCommand
{
public:
	VTransformCmd( VDocument *doc, const QWMatrix& mat );
	virtual ~VTransformCmd() {}

	virtual void execute();
	virtual void unexecute();

protected:
	VTransformCmd( VDocument *doc, const QString& name );

	VSelection m_objects;

	QWMatrix m_mat;
};

class VTranslateCmd : public VTransformCmd
{
public:
	VTranslateCmd( VDocument *doc, double d1, double d2 );
};

class VScaleCmd : public VTransformCmd
{
public:
	VScaleCmd( VDocument *doc, const KoPoint& p, double s1, double s2 );
};

class VShearCmd : public VTransformCmd
{
public:
	VShearCmd( VDocument *doc, const KoPoint& p, double s1, double s2 );
};

class VRotateCmd : public VTransformCmd
{
public:
	VRotateCmd( VDocument *doc, const KoPoint& p, double angle );
};

#endif

