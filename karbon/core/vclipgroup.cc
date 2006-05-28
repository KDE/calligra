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

#include <config-karbon.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qdom.h>
#include <QRectF>
#include <QPointF>

#include <kdebug.h>

#include "vclipgroup.h"
#include "vgroup.h"
#include "vcomposite.h"
#include "vsegment.h"
#include <vpainter.h>
#include "vtext.h"
VClipGroup::VClipGroup( VObject* parent, VState state ) : VGroup( parent, state ) {}
VClipGroup::VClipGroup( const VClipGroup& group ) : VGroup( group ) {}

VClipGroup::~VClipGroup() { }

void VClipGroup::draw( VPainter* painter, const QRectF* rect ) const
{
	return VGroup::draw( painter, rect );
	if(
		state() == deleted ||
		state() == hidden ||
		state() == hidden_locked )
	{
		return;
	}

	VObjectListIterator itr = m_objects;

	painter->save();

	PathRenderer renderer( painter );
	kDebug(38000) << "calling painter setClipPath" << endl;
	painter->setClipPath();

	VObject *obj = itr.current();
	obj->accept( renderer );
	++itr;

	for( ; itr.current(); ++itr )
		itr.current()->draw( painter, rect );

	painter->restore();
}

VClipGroup* VClipGroup::clone() const
{
	return new VClipGroup( *this );
}


void VClipGroup::save( QDomElement& element ) const
{
	QDomElement me = element.ownerDocument().createElement( "CLIP" );
	element.appendChild( me );

	// save objects:
	VObjectListIterator itr = m_objects;

	for( ; itr.current(); ++itr )
		itr.current()->save( me );
}

void VClipGroup::load( const QDomElement& element )
{
	m_objects.setAutoDelete( true );
	m_objects.clear();
	m_objects.setAutoDelete( false );

	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "COMPOSITE" || e.tagName() == "PATH" ) // TODO : remove COMPOSITE later
			{
				VPath* composite = new VPath( this );
				composite->load( e );
				append( composite );
			}
			else if( e.tagName() == "GROUP" )
			{
				VGroup* group = new VGroup( this );
				group->load( e );
				append( group );
			}
			else if( e.tagName() == "CLIP" )
			{
				VClipGroup* clip = new VClipGroup( this );
				clip->load( e );
				append( clip );
			}
			else if( e.tagName() == "TEXT" )
			{
#ifdef HAVE_KARBONTEXT
				VText *text = new VText( this );
				text->load( e );
				append( text );
#endif
			}
		}
	}
}

PathRenderer::PathRenderer( VPainter *p_painter ) : VVisitor()
{
	m_painter = p_painter;
}

PathRenderer::~PathRenderer() {}

void PathRenderer::visitVSubpath( VSubpath& path )
{
	if(!m_painter) return;

	if(path.isEmpty()) return;

	for(path.first(); VSegment *segment = path.current(); path.next() )
	{
		QPointF p1;
		QPointF p2;
		QPointF p3;

		QString buffer;

		if(segment->state() != VSegment::deleted)
		{
			if (segment->isBegin()) {
			    p1 = segment->point( 0 );

			    //kDebug(38000) << "calling painter.moveTo with " << p1 << endl;
			    m_painter->moveTo( p1 );
			} else if (segment->isCurve()) {
			    p1 = segment->point( 0 );
			    p2 = segment->point( 1 );
			    p3 = segment->point( 2 );

			    //kDebug(38000) << "calling painter.curveTo with " << p1 << " " << p2 << " " << p3 << endl;
			    m_painter->curveTo( p1, p2, p3 );

			} else if (segment->isLine()) {
			    p1 = segment->point( 0 );
			    //kDebug(38000) << "calling painter.lineTo with " << p1 << endl;
			    m_painter->lineTo( p1 );
			}
		}
	}

	VVisitor::visitVSubpath(path);

//	if( path.isClosed() ) m_painter->closePath();
}

