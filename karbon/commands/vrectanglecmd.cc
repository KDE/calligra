/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vpath.h"
#include "vrectanglecmd.h"

VRectangleCmd::VRectangleCmd( VDocument *doc,
		const double tlX, const double tlY,
		const double brX, const double brY )
	: VShapeCmd( doc, i18n( "Insert Rectangle" ) ),
	  m_tlX( tlX ), m_tlY( tlY ), m_brX( brX ), m_brY( brY )
{
}

VShape*
VRectangleCmd::createPath()
{
	VPath* path = new VPath();

	path->moveTo( KoPoint( m_tlX, m_tlY ) );
	path->lineTo( KoPoint( m_brX, m_tlY ) );
	path->lineTo( KoPoint( m_brX, m_brY ) );
	path->lineTo( KoPoint( m_tlX, m_brY ) );
	path->close();

	return path;
}

