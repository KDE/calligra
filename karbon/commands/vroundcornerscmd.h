/* This file is part of the KDE project
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

#ifndef __VROUNDCORNERSCMD_H__
#define __VROUNDCORNERSCMD_H__

#include "vcommand.h"


class VPath;
class VSelection;


class VRoundCornersCmd : public VCommand
{
public:
	VRoundCornersCmd( VDocument *doc, double radius );
	virtual ~VRoundCornersCmd();

	virtual void execute();
	virtual void unexecute();

	virtual void visitVPath( VPath& path );

protected:
	VSelection* m_oldObjects;
	VSelection* m_newObjects;

	double m_radius;
};

#endif

