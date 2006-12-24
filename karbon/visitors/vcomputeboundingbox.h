/* This file is part of the KDE project
   Copyright (C) 2006 Jan Hambrecht <jaham@gmx.net>

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

#ifndef __VCOMPUTEBOUNDINGBOX_H__
#define __VCOMPUTEBOUNDINGBOX_H__

#include "KoRect.h"
#include "vvisitor.h"

class VDocument;
class VLayer;
class VGroup;
class VPath;
class VText;
class VImage;

/**
 * This visitor visits objects and calculates the combined bounding box of the
 * objects and their child objects.
 */
class VComputeBoundingBox : public VVisitor
{
public:
	VComputeBoundingBox( bool omitHidden = false );

	virtual void visitVDocument( VDocument& document );
	virtual void visitVLayer( VLayer& layer );
	virtual void visitVGroup( VGroup& group );
	virtual void visitVPath( VPath& composite );
	virtual void visitVText( VText& text );
	virtual void visitVImage( VImage& img );

	const KoRect& boundingRect() const;
private:
	bool isVisible( const VObject* object ) const;

	KoRect m_boundingBox;
	bool m_omitHidden;
};

#endif // __VCOMPUTEBOUNDINGBOX_H__

