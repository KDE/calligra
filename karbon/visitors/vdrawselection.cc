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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include "vdrawselection.h"
#include "vsegment.h"
#include "vcolor.h"
#include "vstroke.h"
#include "vpainter.h"

#include <qcolor.h>

void
VDrawSelection::visitVComposite( VComposite &composite )
{
	if( composite.state() == VObject::deleted || composite.state() == VObject::hidden || composite.state() == VObject::hidden_locked )
		return;

	//if( rect && !rect->intersects( composite.boundingBox() ) )
	//	return;

	m_painter->save();

	VPathListIterator itr( composite.paths() );

	if( composite.state() == VObject::selected )
    {
		// paint fill:
		m_painter->newPath();

		for( itr.toFirst(); itr.current(); ++itr )
		{
			VPathIterator jtr( *( itr.current() ) );
			for( ; jtr.current(); ++jtr )
				jtr.current()->draw( m_painter );
		}
		//m_painter->setRasterOp( Qt::CopyROP );
		m_painter->setPen( Qt::blue );
		m_painter->setBrush( Qt::NoBrush );
		m_painter->strokePath();
	}

	// draw simplistic contour:
	if( composite.state() == VObject::edit )
	{
		// Draw center node:
		//if( m_drawCenterNode )
		//{
			const KoPoint center = composite.boundingBox().center();

			m_painter->newPath();
			//m_painter->setRasterOp( Qt::XorROP );
			m_painter->setPen( Qt::NoPen );
			m_painter->setBrush( Qt::yellow );

			m_painter->drawNode( center, 2 );

			m_painter->fillPath();
		//}
	}

	// Draw nodes and control lines:
	if( composite.state() == VObject::selected )
	{
		itr.toFirst();
		//++itr;		// Skip "begin".

		for( ; itr.current(); ++itr )
		{
			VPathIterator jtr( *( itr.current() ) );
			++jtr;
			for( ; jtr.current(); ++jtr )
			{
				m_painter->newPath();
				//m_painter->setRasterOp( Qt::NotROP );

				VColor color;
				color.set( 0.5, 0.5, 1.0 );
				VStroke stroke( color );
				stroke.setLineWidth( 1.0 );
				m_painter->setPen( stroke );
				m_painter->setBrush( Qt::NoBrush );

				if( jtr.current()->type() == VSegment::curve )
				{
					m_painter->newPath();

					// Draw control lines:
					if(
						jtr.current()->prev() &&
						( jtr.current()->ctrlPoint1Selected() ||
						  jtr.current()->prev()->knotSelected() ) )
					{
						m_painter->moveTo(
							jtr.current()->prev()->knot() );
						m_painter->lineTo(
							jtr.current()->point( 0 ) );

						m_painter->strokePath();
					}

					if(
						jtr.current()->ctrlPoint2Selected() ||
						jtr.current()->knotSelected() )
					{
						m_painter->moveTo(
							jtr.current()->point( 1 ) );
						m_painter->lineTo(
							jtr.current()->knot() );

						m_painter->strokePath();
					}

					// Draw control node1:
					m_painter->newPath();

					if(
						jtr.current()->prev() &&
						( jtr.current()->ctrlPoint1Selected() ||
						  jtr.current()->prev()->knotSelected() ) )
					{
						m_painter->setBrush( Qt::blue.light() );
						m_painter->drawNode( jtr.current()->point( 0 ), 2 );
					}
					else
						m_painter->setBrush( Qt::NoBrush );

					m_painter->strokePath();


					// Draw control node2:
					m_painter->newPath();

					if(
						jtr.current()->ctrlPoint2Selected() ||
						jtr.current()->knotSelected() )
					{
						m_painter->setBrush( Qt::blue.light() );
						m_painter->drawNode( jtr.current()->point( 1 ), 2 );
					}
					else
						m_painter->setBrush( Qt::NoBrush );

					m_painter->strokePath();
				}

				// Draw knot:
				m_painter->setPen( Qt::NoPen );

				if( jtr.current()->knotSelected() )
					m_painter->setBrush( Qt::blue );
				else
					m_painter->setBrush( Qt::blue.light() );

				m_painter->drawNode( jtr.current()->knot(), composite.stroke()->lineWidth() > 5.0 ? 3 : 2 );
			}
		}
	}

	m_painter->restore();

	setSuccess();
}

