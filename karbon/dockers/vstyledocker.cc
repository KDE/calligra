/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
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

#include <qlabel.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qsize.h>

#include <klocale.h>
#include <koIconChooser.h>
#include <koMainWindow.h>
#include <kseparator.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "karbon_factory.h"
#include "karbon_resourceserver.h"
#include <karbon_drag.h>
#include "vselection.h"
#include "vfill.h"
#include "vfillcmd.h"
#include <commands/vtransformcmd.h>

#include "vstyledocker.h"

ClipartChooser::ClipartChooser( QSize iconSize, QWidget *parent, const char *name )
	: KoIconChooser( iconSize, parent, name )
{
	setDragEnabled( true );
}

void
ClipartChooser::startDrag()
{
	KoIconChooser::startDrag();
	KarbonDrag* kd = new KarbonDrag( this );
	VObjectList objects;
	VClipartIconItem *selectedClipart = (VClipartIconItem *)currentItem();
	double s = 128;//kMax( selectedClipart->originalWidth(), selectedClipart->originalHeight() );
	VObject *clipart = selectedClipart->clipart()->clone();

	QWMatrix mat( s, 0, 0, -s, -( s / 2 ), ( s / 2 ) );

	VTransformCmd trafo( 0L, mat );
	trafo.visit( *clipart );

	objects.append( clipart );
	kd->setObjectList( objects );
	kd->dragCopy();
}

VStyleDocker::VStyleDocker( KarbonPart* part, KarbonView* parent, const char* /*name*/ )
	: VDocker( parent->shell() ), m_part ( part ), m_view( parent )
{
	setCaption( i18n( "Style settings" ) );

	mainWidget = new QWidget( this );
	mTabWidget = new QTabWidget( mainWidget );

	//Pattern
	KoPatternChooser *pPatternChooser = new KoPatternChooser( KarbonFactory::rServer()->patterns(), mTabWidget );
	pPatternChooser->setCaption( i18n( "Patterns" ) );

	connect( pPatternChooser, SIGNAL(selected( KoIconItem * ) ), this, SLOT( slotItemSelected( KoIconItem * )));
	mTabWidget->addTab( pPatternChooser, i18n( "Patterns" ) );

	//Clipart
	ClipartChooser *pClipartChooser = new ClipartChooser( QSize( 32,  32 ), mTabWidget );
	mTabWidget->addTab( pClipartChooser, i18n( "Clipart" ) );
	pClipartChooser->setAutoDelete( false );
	VClipartIconItem* item = 0L;
	QPtrList<VClipartIconItem>* clipartItems = KarbonFactory::rServer()->cliparts();
	for( item = clipartItems->first(); item; item = clipartItems->next() )
		pClipartChooser->addItem( item );

	QVBoxLayout *mainWidgetLayout = new QVBoxLayout( mainWidget, 2 );
	mainWidgetLayout->addWidget( mTabWidget );
	mainWidgetLayout->activate();
	mainWidget->setMinimumHeight( 174 );
	mainWidget->setMinimumWidth( 194 );

	setWidget( mainWidget );
}

VStyleDocker::~VStyleDocker()
{
}

void VStyleDocker::slotItemSelected( KoIconItem *item )
{
	VPattern *pattern = (VPattern *)item;
	if( !pattern ) return;
	kdDebug() << "loading pattern : " << pattern->tilename().latin1() << endl;
	if( m_part && m_part->document().selection() )
	{
		VFill fill;
		fill.pattern() = *pattern;//.load( pattern->tilename() );
		//fill.setColor( *m_color );
		fill.setType( VFill::patt );
		m_part->addCommand( new VFillCmd( &m_part->document(), fill ), true );
	}
}

void
VStyleDocker::mouseReleaseEvent( QMouseEvent * )
{
}

#include "vstyledocker.moc"

