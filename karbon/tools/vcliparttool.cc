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

#include <qlayout.h>
#include <qtoolbutton.h>
#include <qhbuttongroup.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qcursor.h>

#include <kiconloader.h>
#include <kdebug.h>

#include "karbon_factory.h"
#include "karbon_resourceserver.h"
#include "karbon_view.h"
#include "karbon_part.h"
#include "vobject.h"
#include "vkopainter.h"
#include "vpainterfactory.h"
#include "vcliparttool.h"
#include "vdocument.h"
#include "vselection.h"

VClipartIconItem::VClipartIconItem( const VObject* clipart, QString filename )
		: m_filename( filename )
{
	m_clipart = clipart->clone();
	m_clipart->setState( VObject::normal );

	m_pixmap.resize( 64, 64 );
	VKoPainter p( &m_pixmap, 64, 64 );
	QWMatrix mat( 64., 0, 0, 64., 0, 0 );
	m_clipart->transform( mat );
	m_clipart->draw( &p, &m_clipart->boundingBox() );
	m_clipart->transform( mat.invert() );
	p.end();

	m_thumbPixmap.resize( 32, 32 );
	VKoPainter p2( &m_thumbPixmap, 32, 32 );
	mat.setMatrix( 32., 0, 0, 32., 0, 0 );
	m_clipart->transform( mat );
	m_clipart->draw( &p2, &m_clipart->boundingBox() );
	m_clipart->transform( mat.invert() );
	p2.end();

	validPixmap = true;
	validThumb = true;

	m_delete = QFileInfo( filename ).isWritable();
} // VClipartIconItem::VClipartIconItem


VClipartIconItem::VClipartIconItem( const VClipartIconItem& item )
		: KoIconItem( item )
{
	m_clipart     = item.m_clipart->clone();
	m_filename    = item.m_filename;
	m_delete      = item.m_delete;
	m_pixmap      = item.m_pixmap;
	m_thumbPixmap = item.m_thumbPixmap;
	validPixmap   = item.validPixmap;
	validThumb    = item.validThumb;
} // VClipartIconItem::VClipartIconItem

VClipartIconItem::~VClipartIconItem()
{
	delete m_clipart;
} // VClipartIconItem::~VClipartIconItem

VClipartIconItem* VClipartIconItem::clone()
{
	return new VClipartIconItem( *this );
} // VClipartIconItem::clone

VClipartWidget::VClipartWidget( QPtrList<VClipartIconItem>* clipartItems, KarbonView* view, QWidget* parent )
		: QFrame( parent ), m_view( view )
{
	KIconLoader il;

	QVBoxLayout* layout = new QVBoxLayout( this );
	layout->addWidget( m_clipartChooser = new KoIconChooser( QSize( 32, 32 ), this ) );
	layout->addWidget( m_buttonGroup = new QHButtonGroup( this ) );
	QToolButton* m_addClipartButton;
	m_buttonGroup->insert( m_addClipartButton = new QToolButton( m_buttonGroup ) );
	m_buttonGroup->insert( m_importClipartButton = new QToolButton( m_buttonGroup ) );
	m_buttonGroup->insert( m_deleteClipartButton = new QToolButton( m_buttonGroup ) );
	m_clipartChooser->setFixedSize( 180, 120 );
	m_addClipartButton->setIconSet( QPixmap( il.iconPath( "14_clipart_add.png", KIcon::Small ) ) );
	m_addClipartButton->setTextLabel( i18n( "Add" ) );
	m_importClipartButton->setIconSet( QPixmap( il.iconPath( "14_clipart_import.png", KIcon::Small ) ) );
	m_importClipartButton->setTextLabel( i18n( "Import" ) );
	m_deleteClipartButton->setIconSet( QPixmap( il.iconPath( "14_clipart_delete.png", KIcon::Small ) ) );
	m_deleteClipartButton->setTextLabel( i18n( "Delete" ) );

	m_buttonGroup->setInsideMargin( 3 );
	m_importClipartButton->setEnabled( false );
	m_deleteClipartButton->setEnabled( false );

	setFrameStyle( Box | Sunken );
	layout->setMargin( 3 );

	connect( m_buttonGroup, SIGNAL( clicked( int ) ), this, SLOT( slotButtonClicked( int ) ) );
	//connect( m_deleteClipartButton, SIGNAL( clicked() ), this, SLOT( deleteClipart() ) );
	connect( m_clipartChooser, SIGNAL( selected( KoIconItem* ) ), this, SLOT( clipartSelected( KoIconItem* ) ) );
	
	m_clipartChooser->setAutoDelete( false );
	VClipartIconItem* item = 0L;
	for( item = clipartItems->first(); item; item = clipartItems->next() )
		m_clipartChooser->addItem( item );
	m_clipartItem = clipartItems->first()->clone();
} // VClipartWidget::VClipartWidget

VClipartWidget::~VClipartWidget()
{
} // VClipartWidget::~VClipartWidget

VClipartIconItem* VClipartWidget::selectedClipart()
{
	return m_clipartItem;
} // VClipartWidget::selectedItem

void VClipartWidget::clipartSelected( KoIconItem* item )
{
	if ( item )
	{
		delete m_clipartItem;
		VClipartIconItem* clipartItem = (VClipartIconItem*)item;
		m_deleteClipartButton->setEnabled( clipartItem->canDelete() );
		m_selectedItem = clipartItem;
		m_clipartItem  = clipartItem->clone();
	}
} // VClipartWidget::clipartSelected

void VClipartWidget::addClipart()
{
	VObject* clipart = 0L;
	VSelection* selection = m_view->part()->document().selection();
	if ( selection->objects().count() == 1 )
		clipart = selection->objects().getFirst()->clone();
	if ( selection->objects().count() > 1 )
	{
		QPtrVector<VObject> objects;
		selection->objects().toVector( &objects );
		VGroup* group = new VGroup( 0L );
		for( unsigned int i = 0; i < objects.count(); i++ )
			group->append( objects[i]->clone() );
		clipart = group;
	}
	if ( clipart )
	{
		KoRect clipartBox = clipart->boundingBox();
		double scaleFactor = 1. / QMAX( clipartBox.width(), clipartBox.height() );
		QWMatrix trMatrix( scaleFactor, 0, 0, scaleFactor, -clipartBox.x() * scaleFactor, -clipartBox.y() * scaleFactor );
		clipart->transform( trMatrix );
		 // remove Y-mirroring
		trMatrix.reset();
		trMatrix.scale( 1, -1 );
		trMatrix.translate( 0, -1 );
		clipart->transform( trMatrix );
		m_clipartChooser->addItem( KarbonFactory::rServer()->addClipart( clipart ) );
	}
	m_clipartChooser->updateContents();
} // VClipartWidget::addClipart

void VClipartWidget::deleteClipart()
{
	VClipartIconItem* clipartItem = m_clipartItem;
	KarbonFactory::rServer()->removeClipart( clipartItem );
	m_clipartChooser->removeItem( m_selectedItem );
	m_clipartChooser->updateContents();
} // VClipartWidget::deleteClipart

void VClipartWidget::slotButtonClicked( int id )
{
	switch( id )
	{
		case 0: addClipart();
				break;
		case 1: //importClipart();
				break;
		case 2: deleteClipart();
				break;
	} 
} // VClipartWidget::slotButtonClicked

VClipartTool::VClipartTool( KarbonView* view )
		: VTool( view )
{
	m_optionsWidget = new VClipartWidget( KarbonFactory::rServer()->cliparts(), view );
} // VClipartTool::VClipartTool

VClipartTool::~VClipartTool()
{
	delete m_optionsWidget;
} // VClipartTool::~VClipartTool

QString VClipartTool::contextHelp()
{
	QString s = "<qt><b>Clipart tool:</b><br>";
	s += "Choose the clipart in the options docker.<br>";
	s += "<i>Click and drag</i> to place the clipart.<br>";
	s += "<i>Click and drag</i> while pressing <i>SHIFT</i> to place the clipart using the original sizeof the cliaprt.</qt>";
	return s;
} // VClipartTool::contextHelp

void VClipartTool::activate()
{
	view()->statusMessage()->setText( i18n( "Clipart Tool" ) );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::crossCursor ) );
} // VClipartTool::activate

void VClipartTool::draw()
{
	if ( ( m_clipart ) && ( m_last.x() - first().x() != 0 ) && ( m_last.y() - first().y() != 0 ) )
	{
		VPainter* painter = view()->painterFactory()->editpainter();
		view()->canvasWidget()->setYMirroring( true );
		painter->setZoomFactor( view()->zoom() );
		painter->setRasterOp( Qt::NotROP );

		QWMatrix mat( m_last.x() - first().x(), 0, 0, m_last.y() - first().y(), first().x(), first().y() );
		m_clipart->transform( mat );
		m_clipart->draw( painter, &m_clipart->boundingBox() );
		m_clipart->transform( mat.invert() );
	}
} // VClipartTool::draw

void VClipartTool::mouseButtonPress()
{
	m_keepRatio = false;

	VClipartIconItem* clipartItem = m_optionsWidget->selectedClipart();
	m_clipart = clipartItem->clipart()->clone();
	m_clipart->setState( VObject::edit );

	m_last = last();
	
	draw();
} // VClipartTool::mouseButtonPress

void VClipartTool::mouseButtonRelease()
{
	cancel();
} // VClipartTool::mouseButtonRelease

void VClipartTool::mouseDrag()
{
	if ( m_clipart )
	{
		draw();

		if ( m_keepRatio )
		{
			double s = QMAX( last().x() - first().x(), -last().y() + first().y() );
			m_last.setCoords( first().x() + s, first().y() - s );
		}
		else
			m_last = last();

		draw();
	}
} // VClipartTool::mouseDrag

void VClipartTool::mouseDragRelease()
{
	if ( m_clipart )
	{
		//draw();

		if ( m_keepRatio )
		{
			double s = QMAX( last().x() - first().x(), -last().y() + first().y() );
			m_last.setCoords( first().x() + s, first().y() - s );
		}
		else
			m_last = last();

		QWMatrix mat( m_last.x() - first().x(), 0, 0, m_last.y() - first().y(), first().x(), first().y() );
		m_clipart->transform( mat );
		VClipartCmd* cmd = new VClipartCmd(
			&view()->part()->document(),
			name(),
			m_clipart );

		view()->part()->addCommand( cmd, true );
		view()->selectionChanged();

		delete m_clipart;
	}
} // VClipartTool::mouseDragRelease

void VClipartTool::mouseDragShiftPressed()
{
	m_keepRatio = true;
} // VClipartTool::mouseDragShiftPressed

void VClipartTool::mouseDragShiftReleased()
{
	m_keepRatio = false;
} // VClipartTool::mouseDragShiftReleased

void VClipartTool::cancel()
{
	draw();
	
	delete m_clipart;
} // VClipartTool::cancel

VClipartTool::VClipartCmd::VClipartCmd( VDocument* doc, const QString& name, VObject* clipart )
		: VCommand( doc, name ), m_clipart( clipart->clone() ), m_executed( false )
{
} // VClipartTool::VClipartCmd::VClipartCmd

void VClipartTool::VClipartCmd::execute()
{
	if( !m_clipart )
		return;

	if( m_clipart->state() == VObject::deleted )
		m_clipart->setState( VObject::normal );
	else
	{
		m_clipart->setState( VObject::normal );
		document()->append( m_clipart );
		document()->selection()->clear();
		document()->selection()->append( m_clipart );
	}
	m_executed = true;
} // VClipartTool::VClipartCmd::execute

void VClipartTool::VClipartCmd::unexecute()
{
	if( !m_clipart )
		return;

	document()->selection()->take( *m_clipart );
	m_clipart->setState( VObject::deleted );
	
	m_executed = false;
} // VClipartTool::VClipartCmd::unexecute

#include "vcliparttool.moc"
