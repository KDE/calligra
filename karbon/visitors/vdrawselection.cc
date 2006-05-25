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


#include "vdrawselection.h"
#include "vcomposite.h"
#include "vsegment.h"
#include "vcolor.h"
#include "vstroke.h"
#include "vpainter.h"
#include "vpath.h"

void
VDrawSelection::visitVPath( VPath &composite )
{
	if(
		composite.state() == VObject::deleted ||
		composite.state() == VObject::hidden ||
		composite.state() == VObject::hidden_locked )
	{
		return;
	}


	m_painter->save();
	m_painter->setPen( Qt::SolidLine );

	const bool editnodes = composite.state() == VObject::edit && m_nodeediting;

	VSubpathListIterator itr( composite.paths() );

	if(
		composite.state() == VObject::selected ||
		editnodes )
	{
		// paint fill:
		m_painter->newPath();

		/* TODO: port rasterops to Qt4
		if( editnodes )
			m_painter->setRasterOp( Qt::XorROP );
		*/

		m_painter->setPen( editnodes ? Qt::yellow : Qt::blue );
		m_painter->setBrush( Qt::NoBrush );

		for( itr.toFirst(); itr.current(); ++itr )
		{
			VSubpathIterator jtr( *( itr.current() ) );

			for( ; jtr.current(); ++jtr )
			{
				jtr.current()->draw( m_painter );
			}

			m_painter->strokePath();
		}
	}

	// Draw nodes and control lines.
	if(
		composite.state() == VObject::selected ||
		editnodes )
	{
		itr.toFirst();
		//++itr;		// Skip "begin".

		for( ; itr.current(); ++itr )
		{
			if( (*itr)->isEmpty() )
				continue;
			VSubpathIterator jtr( *( itr.current() ) );
			//++jtr;

			for( ; jtr.current(); ++jtr )
			{
				/* TODO: port rasterops to Qt4
				if( editnodes )
					m_painter->setRasterOp( Qt::XorROP );
				*/

				VColor color;
				color.set( 0.5, 0.5, 1.0 );

				VStroke stroke( color );
				stroke.setLineWidth( 1.0 );

				if( !editnodes )
				{
					m_painter->setPen( stroke );
					m_painter->setPen( Qt::blue );
				}
				else
					m_painter->setPen( Qt::yellow );

				m_painter->setBrush( Qt::NoBrush );

				if( ( editnodes || composite.state() == VObject::selected && m_nodeediting ) &&
						jtr.current()->isCurve() )
				{
					// Draw control lines.
					if(
						jtr.current()->pointIsSelected( 1 ) ||
						jtr.current()->knotIsSelected() ||
						( jtr.current()->next() &&
						  jtr.current()->next()->pointIsSelected( 0 ) &&
						  jtr.current()->isSmooth() ) )
					{
						m_painter->newPath();
						m_painter->moveTo(
							jtr.current()->point( 1 ) );
						m_painter->lineTo(
							jtr.current()->knot() );

						m_painter->strokePath();
						// Draw control node2:
						m_painter->newPath();
						m_painter->setBrush( editnodes ? Qt::yellow : Qt::blue );
						m_painter->drawNode( jtr.current()->point( 1 ), m_nodeSize );
						m_painter->strokePath();
					}

					if(
						jtr.current()->prev() &&
						( ( jtr.current()->prev()->knotIsSelected() ||
						  jtr.current()->pointIsSelected( 0 ) ) ||
						( jtr.current()->prev()->pointIsSelected( 1 ) &&
						  jtr.current()->prev()->isSmooth() ) ) )
					{
						m_painter->newPath();
						m_painter->moveTo(
							jtr.current()->prev()->knot() );
						m_painter->lineTo(
							jtr.current()->point( 0 ) );

						m_painter->strokePath();
						// Draw control node1:
						m_painter->newPath();
						m_painter->setBrush( editnodes ? Qt::yellow : Qt::blue );
						m_painter->drawNode( jtr.current()->point( 0 ), m_nodeSize );
						m_painter->strokePath();
					}
				}

				// Draw knot.
				m_painter->setPen( editnodes ? Qt::yellow : Qt::blue );

				if( !m_nodeediting )
					m_painter->setBrush( Qt::blue );
				else if( jtr.current()->knotIsSelected() )
					m_painter->setBrush( editnodes ? Qt::yellow : Qt::blue );
				else
					m_painter->setBrush( Qt::white );

				m_painter->drawNode( jtr.current()->knot(), m_nodeSize );
			}
		}
	}

	// Draw center node.
	if( composite.drawCenterNode() && composite.state() == VObject::selected && !m_nodeediting )
	{
		m_painter->setPen( Qt::NoPen );
		// TODO: this needs porting:
		// m_painter->setBrush( Qt::blue.light() );
		m_painter->drawNode( composite.boundingBox().center(), m_nodeSize );
	}

	m_painter->restore();

	setSuccess();
}

