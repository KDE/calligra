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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __VSHAPECMD_H__
#define __VSHAPECMD_H__

#include "vcommand.h"
#include <koffice_export.h>
class VPath;

/*
 * Provides a common base class for creation commands since they all have
 * a similar execute / unexecute behaviour and all build a VPath. Upon
 * execution() the shape will be added to the document and selected, upon undoing
 * it will be set to the deleted state.
 */
class KARBONCOMMAND_EXPORT VShapeCmd : public VCommand
{
public:
	VShapeCmd( VDocument* doc, const QString& name, VPath* shape, const QString& icon = "14_polygon" );
	virtual ~VShapeCmd() {}

	virtual void execute();
	virtual void unexecute();

	virtual bool changesSelection() const { return true; }

protected:
	/// Pointer to the created shape.
	VPath *m_shape;
};

#endif

