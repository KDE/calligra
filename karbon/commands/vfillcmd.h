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

#ifndef __VFILLCMD_H__
#define __VFILLCMD_H__

#include "vcommand.h"
#include "vfill.h"
#include "vgroup.h"

#include <q3valuevector.h>
#include <koffice_export.h>
class VSelection;

// Fill object(s)

class KARBONCOMMAND_EXPORT VFillCmd : public VCommand
{
public:
	VFillCmd( VDocument *doc, const VFill &, const QString& icon = "14_action" );
	virtual ~VFillCmd();

	virtual void execute();
	virtual void unexecute();

	virtual bool changesSelection() const { return true; }

	virtual void visitVGroup( VGroup& group );
	virtual void visitVPath( VPath& composite );
	virtual void visitVText( VText& text );

	virtual void changeFill( const VFill & );
	virtual VSelection* getSelection() const { return m_selection; }

protected:
	VObjectList m_objects;
	VSelection *m_selection;

	VFill m_fill;

	Q3ValueVector<VFill> m_oldfills;
};

#endif

