/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include "vstatebutton.h"

VStateButton::VStateButton( QWidget* parent, const char* name )
		: QPushButton( parent, name )
{
	m_index = 0;
}

VStateButton::~VStateButton()
{
}

void
VStateButton::mouseReleaseEvent( QMouseEvent *e )
{
	QPushButton::mouseReleaseEvent( e );
	if( m_pixmaps.count() > 0 )
	{
		m_index = ++m_index % m_pixmaps.count();
		setPixmap( *( m_pixmaps.at( m_index ) ) );
	}
}

void
VStateButton::setState( unsigned int index )
{
	if( m_pixmaps.count() > 0 )
		setPixmap( *( m_pixmaps.at( m_index ) ) );
}
