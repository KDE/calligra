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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "vlayer.h"
#include "vlayercommand.h"

VLayerCommand::VLayerCommand( VDocument* doc, const QString& name, VLayer* layer, VOrder order )
	: VCommand( doc, name), m_layer( layer ), m_order( order )
{
	if ( order == AddLayer )
	{
		layer->setState( VObject::deleted );
		document()->insertLayer( layer );
	}

	m_oldState = layer->state();
} // VLayerCommand::VLayerCommand

void VLayerCommand::execute()
{
	switch ( m_order )
	{
		case AddLayer:
				m_layer->setState( VObject::normal );
			break;

		case DeleteLayer:
				m_layer->setState( VObject::deleted );
			break;

		case RaiseLayer:
				document()->raiseLayer( m_layer );
			break;

		case LowerLayer:
				document()->lowerLayer( m_layer );
			break;
	}
} // VLayerCommand::execute

void VLayerCommand::unexecute()
{
	switch ( m_order )
	{
		case AddLayer:
				m_layer->setState( VObject::deleted );
			break;

		case DeleteLayer:
				m_layer->setState( m_oldState );
			break;

		case RaiseLayer:
				document()->lowerLayer( m_layer );
			break;

		case LowerLayer:
				document()->raiseLayer( m_layer );
			break;
	}

} // VLayerCommand::execute
