/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vpath.h"
#include "vroundrectcmd.h"


VRoundRectCmd::VRoundRectCmd( VDocument *doc,
		const double tlX, const double tlY,
		const double brX, const double brY, const double edgeR )
	: VShapeCmd( doc, i18n( "Insert Round Rectangle" ) )
{
	m_edgeR = edgeR < 0.0 ? 0.0 : edgeR;

	// make sure that tl is really top-left and br is bottom-right:
	if ( tlX < brX )
	{
		m_tlX = tlX;
		m_brX = brX;
	}
	else
	{
		m_tlX = brX;
		m_brX = tlX;
	}
	if ( tlY > brY )
	{
		m_tlY = tlY;
		m_brY = brY;
	}
	else
	{
		m_tlY = brY;
		m_brY = tlY;
	}

	// catch case, when radius is larger than width or height:
	double minimum;
	if ( m_edgeR  > ( minimum =
						QMIN( ( m_brX - m_tlX ), ( m_tlY - m_brY ) ) * 0.5 ) )
	{
 		m_edgeR = minimum;
	}
}

VObject*
VRoundRectCmd::createPath()
{
	VPath* path = new VPath( 0L );

	path->moveTo( KoPoint( m_tlX, m_tlY - m_edgeR ) );
	path->arcTo(
		KoPoint( m_tlX, m_tlY ),
		KoPoint( m_tlX + m_edgeR, m_tlY ), m_edgeR );
	path->arcTo(
		KoPoint( m_brX, m_tlY ),
		KoPoint( m_brX, m_tlY - m_edgeR ), m_edgeR );
	path->arcTo(
		KoPoint( m_brX, m_brY ),
		KoPoint( m_brX - m_edgeR, m_brY ), m_edgeR );
	path->arcTo(
		KoPoint( m_tlX, m_brY ),
		KoPoint( m_tlX, m_brY + m_edgeR ), m_edgeR );
	path->close();

	return path;
}

