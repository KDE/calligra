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


#include "insertknotsplugin.h"
#include <karbon_view.h>
#include <karbon_part.h>
#include <core/vpath.h>
#include <core/vsegment.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <qgroupbox.h>
#include <QLabel>

#include <knuminput.h>

typedef KGenericFactory<InsertKnotsPlugin, KarbonView> InsertKnotsPluginFactory;
K_EXPORT_COMPONENT_FACTORY( karbon_insertknotsplugin, InsertKnotsPluginFactory( "karboninsertknotsplugin" ) )

InsertKnotsPlugin::InsertKnotsPlugin( KarbonView *parent, const QStringList & ) : Plugin( parent )
{
	KAction *actionInsertKnots = new KAction(KIcon("14_insertknots"), i18n("&Insert Knots..."), actionCollection(), "path_insert_knots");
	connect(actionInsertKnots, SIGNAL(triggered()), this, SLOT(slotInsertKnots()));

	m_insertKnotsDlg = new VInsertKnotsDlg();
}

void
InsertKnotsPlugin::slotInsertKnots()
{
	KarbonPart *part = ((KarbonView *)parent())->part();
	if( part && m_insertKnotsDlg->exec() )
		part->addCommand( new VInsertKnotsCmd( &part->document(), m_insertKnotsDlg->knots() ), true );
}

VInsertKnotsDlg::VInsertKnotsDlg( QWidget* parent, const char* name )
	: KDialog( parent, i18n( "Insert Knots" ), KDialog::Ok | KDialog::Cancel )
{
	setObjectName(name);
	setModal(true);

	// add input fields:
/* TODO needs porting: 
	QGroupBox* group = new QGroupBox( 2, Qt::Horizontal, i18n( "Properties" ), this );

	new QLabel( i18n( "Knots:" ), group );
	m_knots = new KIntSpinBox( group );
	m_knots->setMinValue( 1 );
	group->setMinimumWidth( 300 );

	// signals and slots:
	connect( this, SIGNAL( okClicked() ), this, SLOT( accept() ) );
	connect( this, SIGNAL( cancelClicked() ), this, SLOT( reject() ) );

	setMainWidget( group );
	setFixedSize( baseSize() );*/
}

uint
VInsertKnotsDlg::knots() const
{
	return m_knots->value();
}

void
VInsertKnotsDlg::setKnots( uint value )
{
	m_knots->setValue( value );
}


VInsertKnotsCmd::VInsertKnotsCmd( VDocument* doc, uint knots )
	: VReplacingCmd( doc, i18n( "Insert Knots" ) )
{
	m_knots = knots > 0 ? knots : 1;
}

void
VInsertKnotsCmd::visitVSubpath( VSubpath& path )
{
	path.first();

	double length;

	// Ommit first segment.
	while( path.next() )
	{
		length = path.current()->length();

		for( uint i = m_knots; i > 0; --i )
		{
			path.insert(
				path.current()->splitAt(
					path.current()->lengthParam( length / ( m_knots + 1.0 ) ) ) );

			path.next();
		}

		if( !success() )
			setSuccess();
	}
}

#include "insertknotsplugin.moc"

