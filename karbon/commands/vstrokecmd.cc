/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vselection.h"
#include "vstroke.h"
#include "vgradient.h"
#include "vstrokecmd.h"

VStrokeLineWidthCmd::VStrokeLineWidthCmd( VDocument *doc, double width )
    : VCommand( doc, i18n( "Stroke Objects" ) ), m_width( width )
{
	m_selection = m_doc->selection()->clone();
}

VStrokeLineWidthCmd::~VStrokeLineWidthCmd()
{
	delete( m_selection );
}

void
VStrokeLineWidthCmd::execute()
{
	VObjectListIterator itr( m_selection->objects() );
	for ( ; itr.current() ; ++itr )
	{
		VStroke stroke = *itr.current()->stroke();
		stroke.setParent( itr.current() );
		stroke.setLineWidth( m_width );
		itr.current()->setStroke( stroke );
		m_oldwidths.push_back( itr.current()->stroke()->lineWidth() );
	}
}

void
VStrokeLineWidthCmd::unexecute()
{
	VObjectListIterator itr( m_selection->objects() );
	int i = 0;
	for ( ; itr.current() ; ++itr )
	{
		VStroke stroke = *itr.current()->stroke();
		stroke.setParent( itr.current() );
		stroke.setLineWidth( m_oldwidths[ i++ ] );
		itr.current()->setStroke( stroke );
	}
}

VStrokeCmd::VStrokeCmd( VDocument *doc,  const VStroke *stroke )
    : VCommand( doc, i18n( "Stroke Objects" ) ), m_stroke( stroke )
{
	m_selection = m_doc->selection()->clone();
	m_gradient = 0L;

	if( m_selection->objects().count() == 1 )
		setName( i18n( "Stroke Object" ) );
}

VStrokeCmd::VStrokeCmd( VDocument *doc, VGradient *gradient )
	: VCommand( doc, i18n( "Stroke Objects" ) ), m_gradient( gradient )
{
	m_selection = m_doc->selection()->clone();
	m_stroke = 0L;

	if( m_selection->objects().count() == 1 )
		setName( i18n( "Stroke Object" ) );
}

VStrokeCmd::~VStrokeCmd()
{
	delete( m_selection );
}

void
VStrokeCmd::execute()
{
	VObjectListIterator itr( m_selection->objects() );
	for ( ; itr.current() ; ++itr )
	{
		//if( m_opacity == -1 )
		//	m_color.setOpacity( itr.current()->stroke()->color().opacity() );

		m_oldcolors.push_back( *itr.current()->stroke() );

		VStroke stroke( *itr.current()->stroke() );
		if( m_gradient )
		{
			stroke.gradient() = *m_gradient;
			stroke.setType( VStroke::grad );
		}
		else if( m_stroke )
		{
			stroke.setLineCap( m_stroke->lineCap() );
			stroke.setLineJoin( m_stroke->lineJoin() );
			stroke.setParent( itr.current() );
			stroke.setLineWidth( m_stroke->lineWidth() );
			if( m_stroke->type() == VStroke::none )
			{
				stroke.setType( VStroke::none );
			}
			else if( m_stroke->type() == VStroke::solid )
			{
				stroke.setColor( m_stroke->color() );
				stroke.setType( VStroke::solid );
			}
		}
		itr.current()->setStroke( stroke );
	}
}

void
VStrokeCmd::unexecute()
{
	VObjectListIterator itr( m_selection->objects() );
	int i = 0;
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->setStroke( m_oldcolors[ i++ ] );
	}
}

