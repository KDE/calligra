/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include "vrect.h"

VRect::VRect()
{

}

const QRect&
VRect::getQRect( const double& zoomFactor ) const
{
	m_QRect.moveTopLeft( topLeft().getQPoint( zoomFactor ) );
	m_QRect.moveBottomRight( bottomRight().getQPoint( zoomFactor ) );

	return m_QRect;
}