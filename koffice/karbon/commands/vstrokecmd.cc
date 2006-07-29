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

#include <klocale.h>

#include "vselection.h"
#include "vstroke.h"
#include "vgradient.h"
#include "vstrokecmd.h"
#include "vdocument.h"

VStrokeCmd::VStrokeCmd( VDocument *doc, const VStroke *stroke, const QString& icon )
    : VCommand( doc, i18n( "Stroke Objects" ), icon ), m_stroke( *stroke )
{
	m_selection = document()->selection()->clone();
	m_state = Stroke;

	if( m_selection->objects().count() == 1 )
		setName( i18n( "Stroke Object" ) );
}

VStrokeCmd::VStrokeCmd( VDocument *doc, VGradient *gradient )
	: VCommand( doc, i18n( "Stroke Objects" ), "14_gradient" )
{
	m_selection = document()->selection()->clone();
	m_state = Gradient;
	m_stroke.gradient() = *gradient;

	if( m_selection->objects().count() == 1 )
		setName( i18n( "Stroke Object" ) );
}

VStrokeCmd::VStrokeCmd( VDocument *doc, VPattern *pattern )
	: VCommand( doc, i18n( "Stroke Objects" ), "14_pattern" )
{
	m_selection = document()->selection()->clone();
	m_state = Pattern;
	m_stroke.pattern() = *pattern;

	if( m_selection->objects().count() == 1 )
		setName( i18n( "Stroke Object" ) );
}

VStrokeCmd::VStrokeCmd( VDocument *doc, double width )
	: VCommand( doc, i18n( "Stroke Width" ), "linewidth" )
{
	m_selection = document()->selection()->clone();
	m_state = LineWidth;
	m_stroke.setLineWidth( width );
}

VStrokeCmd::VStrokeCmd( VDocument *doc, const VColor &color )
	: VCommand( doc, i18n( "Stroke Color" ), "linewidth" )
{
	m_selection = document()->selection()->clone();
	m_state = Color;
	m_stroke.setColor( color );
}

VStrokeCmd::VStrokeCmd( VDocument *doc, const QValueList<float>& array )
	: VCommand( doc, i18n( "Dash Pattern" ), "linewidth" )
{
	m_selection = document()->selection()->clone();
	m_state = Dash;
	m_stroke.dashPattern().setArray( array );
}

VStrokeCmd::~VStrokeCmd()
{
	delete( m_selection );
}

void
VStrokeCmd::changeStroke( const VColor &color )
{
	m_state = Color;
	m_stroke.setColor( color );

	VObjectListIterator itr( m_selection->objects() );
	for ( ; itr.current() ; ++itr )
	{
		//if( m_opacity == -1 )
		//	m_color.setOpacity( itr.current()->stroke()->color().opacity() );

		m_oldstrokes.push_back( *itr.current()->stroke() );

		VStroke stroke( *itr.current()->stroke() );
		stroke.setParent( itr.current() );
		
		stroke.setColor( m_stroke.color() );
		stroke.setType( VStroke::solid );
		
		itr.current()->setStroke( stroke );
	}

	setSuccess( true );
}

void
VStrokeCmd::execute()
{
	VObjectListIterator itr( m_selection->objects() );
	for ( ; itr.current() ; ++itr )
	{
		//if( m_opacity == -1 )
		//	m_color.setOpacity( itr.current()->stroke()->color().opacity() );

		m_oldstrokes.push_back( *itr.current()->stroke() );

		VStroke stroke( *itr.current()->stroke() );
		stroke.setParent( itr.current() );
		if( m_state == LineWidth )
			stroke.setLineWidth( m_stroke.lineWidth() );
		else if( m_state == Color )
		{
			stroke.setColor( m_stroke.color() );
			stroke.setType( VStroke::solid );
		}
		else if( m_state == Gradient )
		{
			stroke.gradient() = m_stroke.gradient();
			stroke.setType( VStroke::grad );
		}
		else if( m_state == Pattern )
		{
			stroke.pattern() = m_stroke.pattern();
			stroke.setType( VStroke::patt );
		}
		else if( m_state == Stroke )
		{
			stroke.setLineCap( m_stroke.lineCap() );
			stroke.setLineJoin( m_stroke.lineJoin() );
			stroke.setLineWidth( m_stroke.lineWidth() );
			if( m_stroke.type() == VStroke::none )
			{
				stroke.setType( VStroke::none );
			}
			else if( m_stroke.type() == VStroke::solid )
			{
				stroke.setColor( m_stroke.color() );
				stroke.setType( VStroke::solid );
			}
			else if( m_stroke.type() == VStroke::grad )
			{
				stroke.gradient() = m_stroke.gradient();
				stroke.setType( VStroke::grad );
			}
			else if( m_stroke.type() == VStroke::patt )
			{
				stroke.pattern() = m_stroke.pattern();
				stroke.setType( VStroke::patt );
			}
		}
		else if( m_state == Dash )
		{
			stroke.dashPattern() = m_stroke.dashPattern();
		}
		itr.current()->setStroke( stroke );
	}

	setSuccess( true );
}

void
VStrokeCmd::unexecute()
{
	VObjectListIterator itr( m_selection->objects() );
	int i = 0;
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->setStroke( m_oldstrokes[ i++ ] );
	}

	setSuccess( false );
}

