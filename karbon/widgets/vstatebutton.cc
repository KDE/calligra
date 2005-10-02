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

#include "vstatebutton.h"

VStateButton::VStateButton( QWidget* parent, const char* name )
		: QPushButton( parent, name )
{
	m_index = 0;
	m_pixmaps.setAutoDelete( true );
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
VStateButton::setState( unsigned int /*index*/ )
{
	if( m_pixmaps.count() > 0 )
		setPixmap( *( m_pixmaps.at( m_index ) ) );
}
