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

#include <qhbuttongroup.h>
#include <qinputdialog.h>
#include <qlayout.h>
#include <qptrvector.h>
#include <qtoolbutton.h>

#include <kiconloader.h>
#include <klocale.h>
#include <koMainWindow.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vdocument.h"
#include "vkopainter.h"
#include "vlayer.h"
#include "vlayercmd.h"
#include "vlayersdocker.h"
#include "vselection.h"

VLayerListViewItem::VLayerListViewItem( QListView* parent, KarbonView* view, VLayer* layer )
	: QCheckListItem( parent, 0L, CheckBox ), m_view( view ), m_layer( layer )
{
	update();
} // VLayerListViewItem::VLayerListViewItem

void
VLayerListViewItem::update()
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

void
VLayerListViewItem::stateChange( bool on )
{
	m_layer->setSelected( on );
} // VLayerListViewItem::stateChange

int
VLayerListViewItem::pos()
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

	setCaption( i18n( "Layers Manager" ) );

	QToolButton* button;
	QFrame* mainWidget = new QFrame( this );
	mainWidget->setFrameStyle( QFrame::Box | QFrame::Sunken );
	QVBoxLayout* layout = new QVBoxLayout( mainWidget, 1 );
	layout->addWidget( m_layersListView = new QListView( mainWidget ), 0 );
	m_buttonGroup = new QHButtonGroup( mainWidget );
	m_buttonGroup->setInsideMargin( 3 );
	button = new QToolButton( m_buttonGroup );
	button->setIconSet( SmallIcon( "14_layer_newlayer.png" ) );
	button->setTextLabel( i18n( "New" ) );
	m_buttonGroup->insert( button );
	button = new QToolButton( m_buttonGroup );
	button->setIconSet( SmallIcon( "14_layer_raiselayer.png" ) );
	button->setTextLabel( i18n( "Raise" ) );
	m_buttonGroup->insert( button );
	button = new QToolButton( m_buttonGroup );
	button->setIconSet( SmallIcon( "14_layer_lowerlayer.png" ) );
	button->setTextLabel( i18n( "Lower" ) );
	m_buttonGroup->insert( button );
	button = new QToolButton( m_buttonGroup );
	button->setIconSet( SmallIcon( "14_layer_deletelayer.png" ) );
	button->setTextLabel( i18n( "Delete" ) );
	m_buttonGroup->insert( button );
	layout->addWidget( m_buttonGroup, 1);
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

	connect( m_layersListView, SIGNAL( clicked( QListViewItem*, const QPoint&, int ) ), this, SLOT( selectionChanged( QListViewItem*, const QPoint&, int ) ) );
	connect( m_layersListView, SIGNAL( rightButtonClicked( QListViewItem*, const QPoint&, int ) ), this, SLOT( renameLayer( QListViewItem*, const QPoint&, int ) ) );
	connect( m_buttonGroup, SIGNAL( clicked( int ) ), this, SLOT( slotButtonClicked( int ) ) );

	layout->activate();
	updateLayers();
	setWidget( mainWidget );
} // VLayerDocker::VLayerDocker

void
VLayersDocker::slotButtonClicked( int ID )
{
	switch( ID )
	{
		case 0:
			addLayer(); break;
		case 1:
			raiseLayer(); break;
		case 2:
			lowerLayer(); break;
		case 3:
			deleteLayer(); break;
	}
}

void
VLayersDocker::selectionChanged( QListViewItem* item, const QPoint &, int col )
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

void
VLayersDocker::renameLayer( QListViewItem* item, const QPoint&, int col )
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

void
VLayersDocker::addLayer()
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

void
VLayersDocker::raiseLayer()
{
	VLayerListViewItem* layerItem = (VLayerListViewItem*)m_layersListView->selectedItem();
	if( !layerItem || !layerItem->layer() )
		return;
        if ( m_view->part()->document().canRaiseLayer( layerItem->layer()))
        {
            VLayerCmd* cmd = new VLayerCmd( &m_view->part()->document(), i18n( "Raise layer" ),
                                            layerItem->layer(), VLayerCmd::raiseLayer );
            m_view->part()->addCommand( cmd, true );
            updatePreviews();
        }
} // VLayersDocker::raiseLayer

void
VLayersDocker::lowerLayer()
{
	VLayerListViewItem* layerItem = (VLayerListViewItem*)m_layersListView->selectedItem();
	if( !layerItem || !layerItem->layer() )
		return;
	VLayer *layer = layerItem->layer();
        if ( m_view->part()->document().canLowerLayer( layer))
        {
            VLayerCmd* cmd = new VLayerCmd( &m_view->part()->document(), i18n( "Lower layer" ), layer, VLayerCmd::lowerLayer );
            m_view->part()->addCommand( cmd, true );
            updatePreviews();
        }
} // VLayersDocker::lowerLayer

void
VLayersDocker::deleteLayer()
{
	VLayerListViewItem* layerItem = (VLayerListViewItem*)m_layersListView->selectedItem();
	if( !layerItem || !layerItem->layer() )
		return;
	VLayer *layer = layerItem->layer();
	VLayerCmd* cmd = new VLayerCmd( &m_view->part()->document(), i18n( "Delete layer" ), layer, VLayerCmd::deleteLayer );
	m_view->part()->addCommand( cmd, true );
	updateLayers();
} // VLayersDocker::deleteLayer

void
VLayersDocker::updatePreviews()
{
	// TODO: Optimization: call update() on each view item...
	updateLayers();
} // VLayersDocker::updatePreviews

void
VLayersDocker::updateLayers()
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
