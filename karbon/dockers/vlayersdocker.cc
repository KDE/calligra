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

#include <qpushbutton.h>
#include <qptrvector.h>
#include <qlayout.h> 
#include <qinputdialog.h>
#include <qpixmap.h>

#include <klocale.h>
#include <kiconloader.h>
#include <koMainWindow.h>

#include "vkopainter.h"
#include "karbon_view.h"
#include "karbon_part.h"
#include "vdocument.h"
#include "vlayer.h"
#include "vselection.h"
#include "vlayersdocker.h"
#include "vlayercmd.h"

VLayerListViewItem::VLayerListViewItem( QListView* parent, KarbonView* view, VLayer* layer )
	: QCheckListItem( parent, 0L, CheckBox ), m_view( view ), m_layer( layer )
{
	update();
} // VLayerListViewItem::VLayerListViewItem

void VLayerListViewItem::update()
{
	KIconLoader il;
	QPixmap preview;
	preview.resize( 16, 16 );
	VKoPainter p( &preview, 16, 16, false );
	// Y mirroring
	QWMatrix mat;
	mat.scale( 1, -1 );
	mat.translate( 0,  -16 );
	p.setWorldMatrix( mat );

	// TODO: When the document will support page size, change the following line.
	p.setZoomFactor( 16. / 800. );
	m_layer->draw( &p );
	p.end();

	setOn( m_layer->selected() );
	char buffer[4];
	sprintf( buffer, "%d04", m_view->part()->document().layerPos( m_layer ) );
	setText( 0, buffer );
	setText( 2, m_layer->name() );
	setPixmap( 1, QPixmap( il.iconPath( ( m_layer->state() == VObject::normal || m_layer->state() == VObject::normal_locked ? "14_layer_visible.png" : "14_layer_novisible.png" ), KIcon::Small ) ) );
	setPixmap( 2, preview );
	
	if ( m_layer == m_view->part()->document().activeLayer() )
		setSelected( true );
} // VLayerListViewItem::update

void VLayerListViewItem::stateChange( bool on )
{
	m_layer->setSelected( on );
} // VLayerListViewItem::stateChange

int VLayerListViewItem::pos()
{
	VLayerListViewItem* item;
	if( !( item = (VLayerListViewItem*)itemAbove() ) )
		return 0;
	else 
		return 1 + item->pos();
} // VLayerListViewItem::pos

VLayersDocker::VLayersDocker( KarbonView* view )
	: VDocker( view->shell() ), m_view( view )
{
	KIconLoader il;
	
	setCaption( i18n( "Layers manager" ) );

	QFrame* mainWidget = new QFrame( this );
	mainWidget->setFrameStyle( QFrame::Box | QFrame::Sunken );
	QGridLayout* layout = new QGridLayout( mainWidget, 2, 4 );
	layout->addMultiCellWidget( m_layersListView = new QListView( mainWidget ), 0, 0, 0, 3 );
	layout->addWidget( m_addButton = new QPushButton( QPixmap( il.iconPath( "14_layer_newlayer.png", KIcon::Small ) ), "", mainWidget ), 1, 0, Qt::AlignCenter );
	layout->addWidget( m_raiseButton = new QPushButton( QPixmap( il.iconPath( "14_layer_raiselayer.png", KIcon::Small ) ), "", mainWidget ), 1, 1, Qt::AlignCenter );
	layout->addWidget( m_lowerButton = new QPushButton( QPixmap( il.iconPath( "14_layer_lowerlayer.png", KIcon::Small ) ), "", mainWidget ), 1, 2, Qt::AlignCenter );
	layout->addWidget( m_deleteButton = new QPushButton( QPixmap( il.iconPath( "14_layer_deletelayer.png", KIcon::Small ) ), "", mainWidget ), 1, 3, Qt::AlignCenter);
	layout->setSpacing( 0 );
	layout->setMargin( 3 );
	
	m_layersListView->setAllColumnsShowFocus( true );
	m_layersListView->setFixedSize( 160, 120 );
	m_layersListView->addColumn( i18n( "S" ), 20 );
	m_layersListView->addColumn( i18n( "V" ), 20 );
	m_layersListView->addColumn( i18n( "Layer" ) );
	m_layersListView->setColumnAlignment( 1, Qt::AlignCenter );
	m_layersListView->setColumnWidthMode( 2, QListView::Maximum );
	m_layersListView->setResizeMode( QListView::LastColumn );
	m_layersListView->setSorting( 0, false );
	m_addButton->setFixedSize( 30, 30 );
	m_raiseButton->setFixedSize( 30, 30 );
	m_lowerButton->setFixedSize( 30, 30 );
	m_deleteButton->setFixedSize( 30, 30 );
	
	connect( m_layersListView, SIGNAL( clicked( QListViewItem*, const QPoint&, int ) ), this, SLOT( selectionChanged( QListViewItem*, const QPoint&, int ) ) );
	connect( m_layersListView, SIGNAL( rightButtonClicked( QListViewItem*, const QPoint&, int ) ), this, SLOT( renameLayer( QListViewItem*, const QPoint&, int ) ) );
	connect( m_addButton, SIGNAL( clicked() ), this, SLOT( addLayer() ) );
	connect( m_raiseButton, SIGNAL( clicked() ), this, SLOT( raiseLayer() ) );
	connect( m_lowerButton, SIGNAL( clicked() ), this, SLOT( lowerLayer() ) );
	connect( m_deleteButton, SIGNAL( clicked() ), this, SLOT( deleteLayer() ) );
	
	updateLayers();
	setWidget( mainWidget );
} // VLayerDocker::VLayerDocker

void VLayersDocker::selectionChanged( QListViewItem* item, const QPoint &, int col )
{
	if ( item )
	{
		VLayerListViewItem* layerItem = (VLayerListViewItem*)item;
		m_view->part()->document().setActiveLayer( layerItem->layer() );
		m_view->part()->document().selection()->clear();
		if ( col == 1 )
		{
			layerItem->layer()->setState( layerItem->layer()->state() == VObject::normal || layerItem->layer()->state() == VObject::normal_locked ? VObject::hidden : VObject::normal );
			layerItem->update();
			m_view->part()->repaintAllViews();
		}
	}
} // VLayerDocker::selectionChanged

void VLayersDocker::renameLayer( QListViewItem* item, const QPoint&, int col )
{
	if ( ( item ) && col == 2 ) 
	{
		VLayerListViewItem* layerItem = (VLayerListViewItem*)item;
		bool ok = true;
		QString name = QInputDialog::getText( i18n( "Current layer" ), i18n( "Change the name of the current layer:" ),
																QLineEdit::Normal, layerItem->layer()->name(), &ok, this );
		if (ok)
		{
			layerItem->layer()->setName( name );
			layerItem->update();
		}
	}
} // VLayersDocker::renameLayer

void VLayersDocker::addLayer()
{
	bool ok = true;
	QString name = QInputDialog::getText( i18n( "New layer" ), i18n( "Enter the name of the new layer:" ),
	                             QLineEdit::Normal, i18n( "New layer" ), &ok, this );
	if (ok)
	{
		VLayer* layer = new VLayer( &( m_view->part()->document() ) );
		layer->setName( name );
		VLayerCmd* cmd = new VLayerCmd( &m_view->part()->document(), i18n("Delete layer"), 
				layer, VLayerCmd::addLayer );
		m_view->part()->addCommand( cmd, true );
		updateLayers();
	}
} // VLayersDocker::addLayer

void VLayersDocker::raiseLayer()
{
	VLayerListViewItem* layerItem = (VLayerListViewItem*)m_layersListView->selectedItem();
	if( !layerItem || !layerItem->layer() )
		return;
	VLayerCmd* cmd = new VLayerCmd( &m_view->part()->document(), i18n("Raise layer"), 
			layerItem->layer(), VLayerCmd::raiseLayer );
	m_view->part()->addCommand( cmd, true );
	updatePreviews();
} // VLayersDocker::raiseLayer

void VLayersDocker::lowerLayer()
{
	VLayerListViewItem* layerItem = (VLayerListViewItem*)m_layersListView->selectedItem();
	if( !layerItem || !layerItem->layer() )
		return;
	VLayerCmd* cmd = new VLayerCmd( &m_view->part()->document(), i18n("Lower layer"), 
			layerItem->layer(), VLayerCmd::lowerLayer );
	m_view->part()->addCommand( cmd, true );
	updatePreviews();
} // VLayersDocker::lowerLayer

void VLayersDocker::deleteLayer()
{
	VLayerListViewItem* layerItem = (VLayerListViewItem*)m_layersListView->selectedItem();
	if( !layerItem || !layerItem->layer() )
		return;
	VLayerCmd* cmd = new VLayerCmd( &m_view->part()->document(), i18n("Delete layer"), 
			layerItem->layer(), VLayerCmd::deleteLayer );
	m_view->part()->addCommand( cmd, true );
	updateLayers();
} // VLayersDocker::deleteLayer

void VLayersDocker::updatePreviews()
{
	// TODO: Optimization: call update() on each view item...
	updateLayers();
} // VLayersDocker::updatePreviews

void VLayersDocker::updateLayers()
{
	m_layersListView->clear();
	QPtrVector<VLayer> vector;
	m_view->part()->document().layers().toVector( &vector );
	VLayerListViewItem* item;
	for( int i = vector.count() - 1; i >= 0; i-- )
	{
		if ( vector[i]->state() != VObject::deleted )
			item = new VLayerListViewItem( m_layersListView, m_view, vector[i] );
	}
	m_layersListView->sort();
} // VLayersDocker::updateLayers

#include "vlayersdocker.moc"
