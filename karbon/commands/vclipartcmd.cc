/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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


#include "vclipartcmd.h"
#include "vdocument.h"
#include "vselection.h"

VClipartCmd::VClipartCmd( VDocument* doc, const QString& name, VObject* clipart )
	: VCommand( doc, name ), m_clipart( clipart->clone() ), m_executed( false )
{
}

void
VClipartCmd::execute()
{
	if( !m_clipart )
		return;

	if( m_clipart->state() == VObject::deleted )
		m_clipart->setState( VObject::normal );
	else
	{
		m_clipart->setState( VObject::normal );
		/* TODO: porting to flake
		document()->append( m_clipart );
		document()->selection()->clear();
		document()->selection()->append( m_clipart );
		*/
	}

	m_executed = true;

	setSuccess( true );
}

void
VClipartCmd::unexecute()
{
	if( !m_clipart )
		return;

	document()->selection()->take( *m_clipart );

	m_clipart->setState( VObject::deleted );

	m_executed = false;

	setSuccess( false );
}

