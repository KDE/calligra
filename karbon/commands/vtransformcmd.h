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

#ifndef __VTRANSFORMCMD_H__
#define __VTRANSFORMCMD_H__

#include "vcommand.h"

// Transform object(s) with a specified matrix and allow undo.


class QWMatrix;
class VPath;
class VSelection;


class VTransformCmd : public VCommand
{
public:
	VTransformCmd( VDocument *doc, const QWMatrix& mat );
	virtual ~VTransformCmd();

	virtual void execute();
	virtual void unexecute();

protected:
	VTransformCmd( VDocument *doc, const QString& name );

	VSelection* m_selection;

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

