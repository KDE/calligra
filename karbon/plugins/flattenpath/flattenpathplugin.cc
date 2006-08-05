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

#include "flattenpathplugin.h"
#include "klocale.h"
#include <karbon_view.h>
#include <karbon_part.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <qgroupbox.h>
#include <QLabel>
#include <QHBoxLayout>
#include <kicon.h>

#include <knuminput.h>
#include <commands/vflattencmd.h>


typedef KGenericFactory<FlattenPathPlugin, KarbonView> FlattenPathPluginFactory;
K_EXPORT_COMPONENT_FACTORY( karbon_flattenpathplugin, FlattenPathPluginFactory( "karbonflattenpathplugin" ) )

FlattenPathPlugin::FlattenPathPlugin( KarbonView *parent, const QStringList & )
: Plugin( parent/*, name*/ )
{
	KAction *actionFlattenPath = new KAction(KIcon("14_flatten"), i18n("&Flatten Path..."), actionCollection(), "path_flatten");
	connect(actionFlattenPath, SIGNAL(triggered()), this, SLOT(slotFlattenPath()));

	m_flattenPathDlg = new VFlattenDlg();
	m_flattenPathDlg->setFlatness( 0.2 );
}

void
FlattenPathPlugin::slotFlattenPath()
{
	KarbonPart *part = ((KarbonView *)parent())->part();
	if( part && m_flattenPathDlg->exec() )
		part->addCommand( new VFlattenCmd( &part->document(), m_flattenPathDlg->flatness() ), true );
}

VFlattenDlg::VFlattenDlg( QWidget* parent, const char* name )
	: KDialog( parent )
{
	setObjectName(name);
	setModal(true);
	setCaption( i18n( "Flatten Path" ) );
	setButtons( Ok | Cancel );

	// add input fields on the left:
	QGroupBox* group = new QGroupBox( i18n( "Properties" ), this );

	QHBoxLayout* layout = new QHBoxLayout;

	layout->addWidget(new QLabel( i18n( "Flatness:" )));
	m_flatness = new KDoubleNumInput(group);
	layout->addWidget(m_flatness);

	group->setLayout(layout);
	group->setMinimumWidth( 300 );

	// signals and slots:
	connect( this, SIGNAL( okClicked() ), this, SLOT( accept() ) );
	connect( this, SIGNAL( cancelClicked() ), this, SLOT( reject() ) );

	setMainWidget( group );
	setFixedSize( baseSize() );
}

double
VFlattenDlg::flatness() const
{
	return m_flatness->value();
}

void
VFlattenDlg::setFlatness( double value )
{
	m_flatness->setValue( value);
}

#include "flattenpathplugin.moc"

