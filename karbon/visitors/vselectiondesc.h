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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __VSELECTIONDESC_H__
#define __VSELECTIONDESC_H__

#include "vvisitor.h"
#include <QString>
#include <koffice_export.h>
/**
 * This visitors visits structures and tries to capture relevant object type info
 * as text. There are two methods, one creates a large description like "(1 group, containing
 * 2 objects)", and a short description giving object id, or if there is no object id just the
 * object type, like group/path/text etc.
 *
 * These texts are primarily meant for statusbar messages and object trees.
 */
class KARBONBASE_EXPORT VSelectionDescription : public VVisitor
{
public:
	VSelectionDescription() { m_desc = ""; m_shortdesc = ""; }

	virtual void visitVSelection( VSelection& );
	virtual void visitVGroup( VGroup& );
	virtual void visitVPath( VPath& );
	virtual void visitVText( VText& );
	virtual void visitVImage( VImage& );

	QString description() { return m_desc; }
	QString shortDescription() { return m_shortdesc; }

private:
	QString		m_desc;
	QString		m_shortdesc;
};

#endif

