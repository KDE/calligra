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

#include "vlayer.h"
#include "vlayercmd.h"
#include "vdocument.h"

VLayerCmd::VLayerCmd( VDocument* doc, const QString& name, KoLayerShape* layer, VLayerCmdType order )
	: VCommand( doc, name, "14_layers" ), m_layer( layer ), m_cmdType( order )
{
	if( order == addLayer )
	{
		document()->insertLayer( layer );
	}

	m_wasVisible = layer->isVisible();
	m_wasLocked = layer->isLocked();
}

void
VLayerCmd::execute()
{
	/* TODO: porting to flake 
	switch( m_cmdType )
	{
		case addLayer:
			m_layer->setState( VObject::normal );
		break;

		case deleteLayer:
			m_layer->setState( VObject::deleted );
		break;

		case raiseLayer:
			document()->raiseLayer( m_layer );
		break;

		case lowerLayer:
			document()->lowerLayer( m_layer );
		break;
	}

	setSuccess( true );
	*/
}

void
VLayerCmd::unexecute()
{
	/* TODO: porting to flake 
	switch ( m_cmdType )
	{
		case addLayer:
			m_layer->setState( VObject::deleted );
		break;

		case deleteLayer:
			m_layer->setState( m_oldState );
		break;

		case raiseLayer:
			document()->lowerLayer( m_layer );
		break;

		case lowerLayer:
			document()->raiseLayer( m_layer );
		break;
	}

	setSuccess( false );
	*/
}

