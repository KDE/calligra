/* This file is part of the KDE project
   Copyright (C) 2002, 2003 The Karbon Developers

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

#include "shadoweffectplugin.h"
#include "klocale.h"
#include <karbon_view.h>
#include <karbon_part.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <qgroupbox.h>
#include <QLabel>
#include <QGridLayout>

#include <knuminput.h>
#include <core/vgroup.h>
#include <core/vpath.h>
#include <core/vsegment.h>
#include <core/vselection.h>
#include <core/vdocument.h>
#include "vshadowdecorator.h"

typedef KGenericFactory<ShadowEffectPlugin, KarbonView> ShadowEffectPluginFactory;
K_EXPORT_COMPONENT_FACTORY( karbon_shadoweffectplugin, ShadowEffectPluginFactory( "karbonshadoweffectplugin" ) )

ShadowEffectPlugin::ShadowEffectPlugin( KarbonView *parent, const QStringList & )
: Plugin( parent )
{
	new KAction(
		i18n( "&Shadow Effect..." ), "shadowRB", 0, this,
		SLOT( slotShadowEffect() ), actionCollection(), "object_shadow" );

	m_shadowEffectDlg = new VShadowEffectDlg();
	m_shadowEffectDlg->setDistance( 2 );
	m_shadowEffectDlg->setAngle( 0 );
}

void
ShadowEffectPlugin::slotShadowEffect()
{
	KarbonPart *part = ((KarbonView *)parent())->part();
	if( part && m_shadowEffectDlg->exec() )
		part->addCommand( new VCreateShadowCmd( &part->document(), m_shadowEffectDlg->distance(), m_shadowEffectDlg->angle(), double( m_shadowEffectDlg->opacity() ) / 255.0 ), true );
}

VShadowEffectDlg::VShadowEffectDlg( QWidget* parent, const char* name )
	: KDialogBase( parent, name, true,  i18n( "Create Shadow Effect" ), Ok | Cancel )
{
	// add input fields on the left:
	QGroupBox* group = new QGroupBox( i18n( "Properties" ), this );

	QGridLayout* layout = new QGridLayout;

	layout->addWidget(new QLabel( i18n( "Distance:" )), 0, 0);
	m_distance = new KIntNumInput( group );
	m_distance->setRange( -1000, 1000, 1, true );
	m_distance->setValue( 2 );
	layout->addWidget(m_distance, 0, 1);

	layout->addWidget(new QLabel( i18n( "Angle:" )), 1, 0);
	m_angle = new KIntNumInput( group );
	m_angle->setRange( 0, 360, 10, true );
	m_angle->setValue( 0 );
	layout->addWidget(m_angle, 1, 1);

	layout->addWidget(new QLabel( i18n( "Opacity:" )), 2, 0);
	m_opacity = new KIntNumInput( group );
	m_opacity->setRange( 0, 100, 1, true );
	m_opacity->setValue( 100 );
	layout->addWidget(m_opacity, 2, 1);

	group->setLayout(layout);
	group->setMinimumWidth( 300 );
	m_opacity->setSuffix(i18n("%"));

	// signals and slots:
	connect( this, SIGNAL( okClicked() ), this, SLOT( accept() ) );
	connect( this, SIGNAL( cancelClicked() ), this, SLOT( reject() ) );

	setMainWidget( group );
}

void
VShadowEffectDlg::setDistance( int d )
{
	m_distance->setValue( d );
}

void
VShadowEffectDlg::setAngle( int a )
{
	m_angle->setValue( a );
}

void
VShadowEffectDlg::setOpacity( int o )
{
	m_angle->setValue( o );
}

int
VShadowEffectDlg::distance() const
{
	return m_distance->value();
}

int
VShadowEffectDlg::angle() const
{
	return m_angle->value();
}

int
VShadowEffectDlg::opacity() const
{
	return m_opacity->value();
}

VCreateShadowCmd::VCreateShadowCmd( VDocument* doc, int distance, int angle, float opacity )
	: VCommand( doc, i18n( "Create Shadow" ) ), m_distance( distance ), m_angle( angle ), m_opacity( opacity )
{
	// Set members.
	m_oldObjects = document()->selection()->clone();
	m_newObjects = 0L;
}

VCreateShadowCmd::~VCreateShadowCmd()
{
	delete( m_oldObjects );
	delete( m_newObjects );
}

void
VCreateShadowCmd::execute()
{
	// Did we have at least once a success? Otherwise we don't get inserted
	// into the command history.
	bool successful = false;


	// Create new shapes if they don't exist yet.
	if( !m_newObjects )
	{
		m_newObjects = new VSelection();

		// Pointer to temporary object.
		VObject* newObject;

		VObjectListIterator itr( m_oldObjects->objects() );

		for( ; itr.current(); ++itr )
		{
			// Clone object and visit the clone.
			VShadowDecorator *shadow = dynamic_cast<VShadowDecorator *>( itr.current() );
			if( shadow )
			{
				//kDebug() <<  "Its a decorator!!!" << endl;
				shadow->setShadow( m_distance, m_angle, m_opacity );
				newObject = 0L;
			}
			else
				newObject = new VShadowDecorator( itr.current()->clone(), 0L, m_distance, m_angle, m_opacity );

			successful = true;

			if(newObject)
			{
				// Insert new shape right before old shape.
				itr.current()->parent()->insertInfrontOf( 
					newObject, itr.current() );

				// Add new shape to list of new objects.
				m_newObjects->append( newObject );
			}
		}
	}

	// Nothing to do.
	if( m_newObjects->objects().count() == 0 )
		return;	
	
	VObjectListIterator itr( m_oldObjects->objects() );

	// Hide old objects.
	for( ; itr.current(); ++itr )
	{
		document()->selection()->take( *itr.current() );
		itr.current()->setState( VObject::deleted );
	}

	// Show new objects.
	for( itr = m_newObjects->objects(); itr.current(); ++itr )
	{
		itr.current()->setState( VObject::normal );
		document()->selection()->append( itr.current() );
	}

	successful = true;

	// Tell command history wether we had success at least once.
	setSuccess( successful );
}

void
VCreateShadowCmd::unexecute()
{
	// Nothing to do.
	if( m_newObjects->objects().count() == 0 )
		return;


	VObjectListIterator itr( m_oldObjects->objects() );

	// Show old objects.
	for( ; itr.current(); ++itr )
	{
		itr.current()->setState( VObject::normal );
		document()->selection()->append( itr.current() );
	}

	// Hide new objects.
	for( itr = m_newObjects->objects(); itr.current(); ++itr )
	{
		document()->selection()->take( *itr.current() );
		itr.current()->setState( VObject::deleted );
	}

	// Reset success for command history.
	setSuccess( false );
}

#include "shadoweffectplugin.moc"

