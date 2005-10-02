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

#ifndef __VVISITOR_H__
#define __VVISITOR_H__

#include <koffice_export.h>
class VPath;
class VDocument;
class VGroup;
class VLayer;
class VObject;
class VSubpath;
class VSelection;
class VText;
class VImage;

class KARBONBASE_EXPORT VVisitor
{
public:
	VVisitor()
	{
		m_success = false;
	}

	virtual bool visit( VObject& object );
	virtual void visitVObject( VObject& object );
	virtual void visitVPath( VPath& composite );
	virtual void visitVDocument( VDocument& document );
	virtual void visitVGroup( VGroup& group );
	virtual void visitVLayer( VLayer& layer );
	virtual void visitVSubpath( VSubpath& path );
	virtual void visitVSelection( VSelection& selection );
	virtual void visitVText( VText& text );
	virtual void visitVImage( VImage& img );

	bool success() const
	{
		return m_success;
	}

protected:
	/**
	 * Make this class "abstract".
	 */
	virtual ~VVisitor() {}

	void setSuccess( bool success = true )
	{
		m_success = success;
	}

private:
	bool m_success;
};

#endif

