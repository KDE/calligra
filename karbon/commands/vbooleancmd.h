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

#ifndef __VBOOLEANCMD_H__
#define __VBOOLEANCMD_H__


#include <qvaluelist.h>

#include "vcommand.h"

class VPath;
class VSegment;
class VSelection;


class VBooleanCmd : public VCommand
{
public:
	enum VBooleanType
	{
		intersect,
		shape_union,
		shape_xor,
		substract
	};

	VBooleanCmd( VDocument* doc, VBooleanType type = intersect );
	virtual ~VBooleanCmd();

	virtual void execute();
	virtual void unexecute();


	// We can only visit object pairs.
	virtual bool visit( VObject& /*object*/ )
	{
		return false;
	}

	// A pair visit() function.
	bool visit( VObject& object1, VObject& object2 );


	virtual void visitVPath( VPath& path );

protected:
	typedef QValueList<double> VParamList;

	void recursiveSubdivision(
		const VSegment& segment1, double t0_1, double t1_1,
		const VSegment& segment2, double t0_2, double t1_2,
		VParamList& params1, VParamList& params2 );

	void doIt();


	VSelection* m_selection;

	VBooleanType m_type;

	VPath* m_path1;
	VPath* m_path2;
};

#endif

