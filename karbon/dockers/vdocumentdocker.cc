/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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

#include <Q3HButtonGroup>
#include <qinputdialog.h>
#include <QLayout>
#include <QCheckBox>
#include <q3listview.h>
#include <q3ptrvector.h>
#include <qtoolbutton.h>
#include <qpainter.h>
#include <qtabwidget.h>
#include <QLabel>
#include <qcursor.h>
#include <QGridLayout>
#include <Q3PtrList>
#include <QPixmap>
#include <Q3Frame>
#include <QMouseEvent>
#include <QEvent>
#include <QVBoxLayout>
#include <QPaintEvent>
#include <QRectF>
#include <QPalette>
#include <QBrush>
#include <QPainter>

#include <klocale.h>
#include <kglobal.h>
#include <KoMainWindow.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kinputdialog.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "karbon_factory.h"
#include "karbon_resourceserver.h"
#include "vdocument.h"
#include "vqpainter.h"
#include "vlayer.h"
#include "vlayercmd.h"
#include "vzordercmd.h"
#include "vselection.h"
#include "vstroke.h"
#include "vcanvas.h"
#include "vdocumentdocker.h"
#include <visitors/vselectiondesc.h>

static long g_lastKey = 0;

/*************************************************************************
 *  Document tab                                                         *
 *************************************************************************/

VDocumentPreview::VDocumentPreview( KarbonView* view, QWidget* parent )
		: QWidget( parent ), m_document( &view->part()->document() ), m_view( view )
{
	setObjectName("DocumentPreview");

	update();
	installEventFilter( this );
	QBrush b(Qt::NoBrush);
	QPalette p = palette();
	p.setBrush(QPalette::Window, b);
	setMouseTracking( true );
	m_dragging = false;
	m_docpixmap = 0L;
} // VDocumentPreview::VDocumentPreview

VDocumentPreview::~VDocumentPreview()
{
	delete m_docpixmap;
} // VDocumentPreview::~VDocumentPreview

void
VDocumentPreview::reset()
{
	delete m_docpixmap;
	m_docpixmap = 0L;
}

bool
VDocumentPreview::eventFilter( QObject* object, QEvent* event )
{
	double scaleFactor;
	double xoffset = 0.;
	double yoffset = 0.;
	if ( ( height() - 4 ) / m_document->height() > ( width() - 4 ) / m_document->width() )
	{
		scaleFactor = ( width() - 4 ) / m_document->width();
		yoffset = ( ( height() - 4 ) / scaleFactor - m_document->height() ) / 2;
	}
	else
	{
		scaleFactor = ( height() - 4 ) / m_document->height();
		xoffset = ( ( width() - 4 ) / scaleFactor - m_document->width() ) / 2;
	}
	// TODO: needs porting
	// QRectF rect = m_view->canvasWidget()->boundingBox();
QRectF rect = QRectF();

	QMouseEvent* mouseEvent = static_cast<QMouseEvent*>( event );
	if( event->type() == QEvent::MouseButtonPress )
	{
		m_firstPoint.setX( mouseEvent->pos().x() );
		m_firstPoint.setY( mouseEvent->pos().y() );
		m_lastPoint = m_firstPoint;
		QPointF p3( m_firstPoint.x() / scaleFactor - xoffset,
					( height() - m_firstPoint.y() ) / scaleFactor - yoffset );
		m_dragging = rect.contains( p3 );
	}
	else if( event->type() == QEvent::MouseButtonRelease )
	{
		if( m_dragging )
		{
			m_lastPoint.setX( mouseEvent->pos().x() );
			m_lastPoint.setY( mouseEvent->pos().y() );
			double dx = m_lastPoint.x() - m_firstPoint.x();
			double dy = m_lastPoint.y() - m_firstPoint.y();
			scaleFactor /= m_view->zoom();
	// TODO: needs porting
			// m_view->canvasWidget()->scrollContentsBy( int( dx / scaleFactor ), int( dy / scaleFactor ) );
			m_firstPoint = m_lastPoint;
			m_dragging = false;
			update();
		}
	}
	else if( event->type() == QEvent::MouseMove )
	{
		if( m_dragging )
		{
			m_lastPoint.setX( mouseEvent->pos().x() );
			m_lastPoint.setY( mouseEvent->pos().y() );
			update();
			/*double dx = m_lastPoint.x() - m_firstPoint.x();
			double dy = m_lastPoint.y() - m_firstPoint.y();
			scaleFactor /= m_view->zoom();
			m_view->canvasWidget()->scrollBy( int( dx / scaleFactor ), int( dy / scaleFactor ) );
			m_firstPoint = m_lastPoint;*/
		}
		else
		{
			QPointF p3( mouseEvent->pos().x() / scaleFactor - xoffset,
						( height() - mouseEvent->pos().y() ) / scaleFactor - yoffset );
			setCursor( rect.contains( p3 ) ? Qt::SizeAllCursor : QCursor( Qt::ArrowCursor ) );
		}
	}

	return QWidget::eventFilter( object, event );
}

void
VDocumentPreview::paintEvent( QPaintEvent* )
{
	// TODO : use NotROP, otherwise too slow
	QPixmap pixmap( width(), height() );
	double xoffset = 0.;
	double yoffset = 0.;
	double scaleFactor;
	if ( ( height() - 4 ) / m_document->height() > ( width() - 4 ) / m_document->width() )
	{
		scaleFactor = ( width() - 4 ) / m_document->width();
		yoffset = ( ( height() - 4 ) / scaleFactor - m_document->height() ) / 2;
	}
	else
	{
		scaleFactor = ( height() - 4 ) / m_document->height();
		xoffset = ( ( width() - 4 ) / scaleFactor - m_document->width() ) / 2;
	}
	xoffset += 2 / scaleFactor;
	yoffset += 2 / scaleFactor;
	if( !m_docpixmap || m_docpixmap->width() != width() || m_docpixmap->height() != height() )
	{
		delete m_docpixmap;
		m_docpixmap = new QPixmap( width(), height() );
		VQPainter p( m_docpixmap, width(), height() );
		p.clear( QColor( 195, 194, 193 ) );
		p.setMatrix( QMatrix( 1, 0, 0, -1, xoffset * scaleFactor, height() - yoffset * scaleFactor ) );
		p.setZoomFactor( scaleFactor );
		QRectF rect( -xoffset, -yoffset, m_document->width() + xoffset, m_document->height() + yoffset );
		// draw doc outline
		VColor c( Qt::black );
		VStroke stroke( c, 0L, 1.0 / scaleFactor );
		p.setPen( stroke );
		p.setBrush( Qt::white );
		p.drawRect( QRectF( 2, 2, m_document->width() - 2, m_document->height() - 2 ) );
		m_document->draw( &p, &rect );
		p.end();
	}
	QPainter pmp(&pixmap);
	pmp.drawPixmap( QPoint(0, 0), *m_docpixmap, QRect(0, 0, width(), height()) );

	// draw viewport rect
	{
		QPainter p( &pixmap );
		p.setMatrix( QMatrix( scaleFactor, 0, 0, -scaleFactor, xoffset * scaleFactor, height() - yoffset * scaleFactor ) );
		p.setPen( QColor( "red" ) );
		double dx = ( m_lastPoint.x() - m_firstPoint.x() ) * m_view->zoom();
		double dy = ( m_lastPoint.y() - m_firstPoint.y() ) * m_view->zoom();
		QPointF p1( dx / scaleFactor, dy / scaleFactor );
	// TODO: needs porting
		// p1 = m_view->canvasWidget()->toContents( p1 );
		QPointF p2( dx / scaleFactor + m_view->canvasWidget()->width(), dy / scaleFactor + m_view->canvasWidget()->height() );
	// TODO: needs porting
		// p2 = m_view->canvasWidget()->toContents( p2 );
		p.drawRect( int( p1.x() ), int( p1.y() ), int( p2.x() - p1.x() ), int( p2.y() - p1.y() ) );
	}

	QPainter pw( &pixmap );
	pw.setPen( palette().light().color() );
	pw.drawLine( 1, 1, 1, height() - 2 );
	pw.drawLine( 1, 1, width() - 2, 1 );
	pw.drawLine( width() - 1, height() - 1, 0, height() - 1 );
	pw.drawLine( width() - 1, height() - 1, width() - 1, 0 );
	pw.setPen( palette().dark().color() );
	pw.drawLine( 0, 0, width() - 1, 0 );
	pw.drawLine( 0, 0, 0, height() - 1 );
	pw.drawLine( width() - 2, height() - 2, width() - 2, 1 );
	pw.drawLine( width() - 2, height() - 2, 1, height() - 2 );
	pw.end();
	QPainter pmp2(this);
	pmp2.drawPixmap( QPoint(0, 0), pixmap, QRect(0, 0, width(), height()) );
} // VDocumentPreview::paintEvent

VDocumentTab::VDocumentTab( KarbonView* view, QWidget* parent )
		: QWidget( parent ), m_view( view )
{
	setObjectName("DocumentTab");

	Q3Frame* frame;
	QGridLayout* layout = new QGridLayout;
	layout->setMargin( 3 );
	layout->setSpacing( 2 );
	layout->addWidget( m_documentPreview = new VDocumentPreview( m_view, this ), 0, 7, 2, 2 );
	layout->addWidget( new QLabel( i18n( "Width:" ), this ), 0, 0 );
	layout->addWidget( new QLabel( i18n( "Height:" ), this ), 1, 0 );
	layout->addWidget( frame = new Q3Frame( this ), 2, 2, 0, 1 );
	frame->setFrameShape( Q3Frame::HLine );
	layout->addWidget( new QLabel( i18n( "Layers:" ), this ), 3, 0 );
	layout->addWidget( new QLabel( i18n( "Format:" ), this ), 4, 0 );
	layout->addWidget( frame = new Q3Frame( this ), 5, 5, 0, 1 );
	frame->setFrameShape( Q3Frame::HLine );
	//layout->addMultiCellWidget( new QLabel( i18n( "Zoom factor:" ), this ), 6, 6, 0, 1 );
	layout->addWidget( m_width = new QLabel( this ), 0, 1 );
	layout->addWidget( m_height = new QLabel( this ), 1, 1 );
	layout->addWidget( m_layers = new QLabel( this ), 3, 1 );
	layout->addWidget( m_format = new QLabel( this ), 4, 1 );
	layout->setRowStretch( 7, 1 );
	layout->setColumnStretch( 0, 0 );
	layout->setColumnStretch( 1, 0 );
	layout->setColumnStretch( 2, 2 );
	//layout->addWidget(

	m_width->setAlignment( Qt::AlignRight );
	m_height->setAlignment( Qt::AlignRight );
	m_layers->setAlignment( Qt::AlignRight );
	m_format->setAlignment( Qt::AlignRight );

	/*
	connect( view->part()->commandHistory(), SIGNAL( commandAdded( VCommand* ) ), this, SLOT( slotCommandAdded( VCommand* ) ) );
	connect( view->part()->commandHistory(), SIGNAL( commandExecuted() ), this, SLOT( slotCommandExecuted() ) );
	*/
	connect( view, SIGNAL( pageLayoutChanged() ), this, SLOT( slotCommandExecuted() ) );
	connect( view->canvasWidget(), SIGNAL( viewportChanged() ), this, SLOT( slotViewportChanged() ) );
	setLayout(layout);

	updateDocumentInfo();
} // VDocumentTab::VDocumentTab

VDocumentTab::~VDocumentTab()
{
} // VDocumentTab::~VDocumentTab

void
VDocumentTab::updateDocumentInfo()
{
	m_width->setText( KoUnit::toUserStringValue( m_view->part()->document().width(), m_view->part()->unit() ) + m_view->part()->unitName() );
	m_height->setText( KoUnit::toUserStringValue( m_view->part()->document().height(), m_view->part()->unit() ) + m_view->part()->unitName() );
	m_layers->setText( QString::number( m_view->part()->document().layers().count() ) );
} // VDocumentTab::updateDocumentInfo

void
VDocumentTab::slotCommandAdded( VCommand * )
{
	m_documentPreview->reset();
	m_documentPreview->update();
}

void
VDocumentTab::slotZoomChanged( double )
{
	m_documentPreview->update();
}

void
VDocumentTab::slotViewportChanged()
{
	m_documentPreview->update();
	updateDocumentInfo();
}

void
VDocumentTab::slotCommandExecuted()
{
	m_documentPreview->reset();
	m_documentPreview->update();
}

/*************************************************************************
 *  Layers tab                                                           *
 *************************************************************************/

VObjectListViewItem::VObjectListViewItem( Q3ListViewItem* parent, KoShape* object, VDocument *doc, uint key, Q3PtrDict<VObjectListViewItem> *map )
	: Q3ListViewItem( parent, 0L ), m_object( object ), m_document( doc ), m_key( key ), m_map( map )
{
	update();
	// add itself to object list item map
	m_map->insert( object, this );
}

VObjectListViewItem::~VObjectListViewItem() 
{
	// remove itself from object list item map
	m_map->take( m_object );
}

QString
VObjectListViewItem::key( int, bool ) const
{
	return QString( "%1" ).arg( m_key );
}

void
VObjectListViewItem::update()
{
	// text description
	/* TODO: porting to flake
	VSelectionDescription selectionDesc;
	selectionDesc.visit( *m_object );
	setText( 0, QString( "%1" ).arg( selectionDesc.shortDescription() ) );
	*/

	// draw thumb preview (16x16)
	QPixmap preview( 16, 16 );
	VQPainter p( &preview, 16, 16, false );
	// Y mirroring
	QMatrix mat;
	mat.scale( 1, -1 );
	QRectF bbox = m_object->boundingRect();
	mat.translate( 0, -16 );
	double factor = 16. / qMax( bbox.width(), bbox.height() );
	mat.translate( -bbox.x() * factor, -bbox.y() * factor );
	p.setMatrix( mat );

	// TODO: When the document will support page size, change the following line.
	p.setZoomFactor( factor );
	// TODO: porting to flake
	//m_object->draw( &p );
	p.setZoomFactor( 1 );
	p.setMatrix( QMatrix() );
	p.setPen( QColor( "black" ) );
	p.setBrush( Qt::NoBrush );
	p.drawRect( QRectF( 0, 0, 16, 16 ) );
	p.end();

	// set thumb preview, lock and visible pixmaps
	setPixmap( 0, preview );
	QString s = m_object->isLocked() ? "locked" : "unlocked";
	setPixmap( 1, *KarbonFactory::rServer()->cachePixmap( s, K3Icon::Small ) );
	s = m_object->isVisible() ? "14_layer_visible" : "14_layer_novisible";
	setPixmap( 2, *KarbonFactory::rServer()->cachePixmap( s, K3Icon::Small ) );
}

int 
VObjectListViewItem::compare( Q3ListViewItem *i, int /*col*/, bool /*ascending*/ ) const
{
	VObjectListViewItem *objectItem = dynamic_cast<VObjectListViewItem*>(i);
	if( ! objectItem ) return 0;
	return m_key < objectItem->m_key ? -1 : 1;
}

VLayerListViewItem::VLayerListViewItem( Q3ListView* parent, KoLayerShape* layer, VDocument *doc, Q3PtrDict<VLayerListViewItem> *map )
	: Q3CheckListItem( parent, 0L, CheckBox ), m_layer( layer ), m_document( doc), m_map( map )
{
	update();
	// add itself to layer list item map
	m_map->insert( layer, this );
} // VLayerListViewItem::VLayerListViewItem

VLayerListViewItem::~VLayerListViewItem()
{
	// remove itself from layer list item map
	m_map->take( m_layer );
}

void
VLayerListViewItem::update()
{
	// draw thumb preview (16x16)
	QPixmap preview( 16, 16 );
	VQPainter p( &preview, 16, 16, false );
	// Y mirroring
	QMatrix mat;
	mat.scale( 1, -1 );
	mat.translate( 0,  -16 );
	p.setMatrix( mat );

	// TODO: When the document will support page size, change the following line.
	p.setZoomFactor( 16. / 800. );
	//m_layer->draw( &p );
	p.setZoomFactor( 1 );
	p.setMatrix( QMatrix() );
	p.setPen( QColor( "black" ) );
	p.setBrush( Qt::NoBrush );
	p.drawRect( QRectF( 0, 0, 16, 16 ) );
	p.end();

	// TODO: porting
	// text description
	//setOn( m_layer->selected() );
	//setText( 0, m_layer->name() );

	// set thumb preview, lock and visible pixmaps
	setPixmap( 0, preview );
	QString s = m_layer->isLocked() ? "locked" : "unlocked";
	setPixmap( 1, *KarbonFactory::rServer()->cachePixmap( s, K3Icon::Small ) );
	s = m_layer->isVisible() ? "14_layer_visible" : "14_layer_novisible";
	setPixmap( 2, *KarbonFactory::rServer()->cachePixmap( s, K3Icon::Small ) );
} // VLayerListViewItem::update

void
VLayerListViewItem::stateChange( bool on )
{
	//m_layer->setSelected( on );
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

QString
VLayerListViewItem::key( int, bool ) const
{
	return QString( "%1" ).arg( m_key );
}

int 
VLayerListViewItem::compare( Q3ListViewItem *i, int /*col*/, bool /*ascending*/ ) const
{
	VLayerListViewItem *layerItem = dynamic_cast<VLayerListViewItem*>(i);
	if( ! layerItem ) return 0;
	return m_key < layerItem->m_key ? -1 : 1;
}

VLayersTab::VLayersTab( KarbonView* view, QWidget* parent )
		: QWidget( parent ), m_view( view ), m_document( &view->part()->document() )
{
	setObjectName("LayersTab");

	QToolButton* button;
	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget( m_layersListView = new Q3ListView( this ), 1 );
	m_buttonGroup = new Q3HButtonGroup( this );
	m_buttonGroup->setInsideMargin( 3 );
	button = new QToolButton( m_buttonGroup );
	button->setIcon( SmallIcon( "14_layer_newlayer" ) );
	button->setText( i18n( "New" ) );
	m_buttonGroup->insert( button );
	button = new QToolButton( m_buttonGroup );
	button->setIcon( SmallIcon( "14_layer_raiselayer" ) );
	button->setText( i18n( "Raise" ) );
	m_buttonGroup->insert( button );
	button = new QToolButton( m_buttonGroup );
	button->setIcon( SmallIcon( "14_layer_lowerlayer" ) );
	button->setText( i18n( "Lower" ) );
	m_buttonGroup->insert( button );
	button = new QToolButton( m_buttonGroup );
	button->setIcon( SmallIcon( "14_layer_deletelayer" ) );
	button->setText( i18n( "Delete" ) );
	m_buttonGroup->insert( button );
	layout->addWidget( m_buttonGroup, 0);
	layout->setSpacing( 0 );
	layout->setMargin( 3 );

	m_layersListView->setAllColumnsShowFocus( true );
	m_layersListView->addColumn( i18n( "Item" ), 120 );
	m_layersListView->addColumn( i18n( "L" ), 20 );
	m_layersListView->addColumn( i18n( "V" ), 20 );
	m_layersListView->setColumnWidthMode( 0, Q3ListView::Maximum );
	m_layersListView->setColumnAlignment( 1, Qt::AlignCenter );
	m_layersListView->setColumnAlignment( 2, Qt::AlignCenter );
	m_layersListView->setResizeMode( Q3ListView::NoColumn );
	m_layersListView->setSorting( 0, false );
	m_layersListView->setRootIsDecorated( true );
	m_layersListView->setSelectionMode( Q3ListView::Extended );

	connect( m_layersListView, SIGNAL( clicked( Q3ListViewItem*, const QPoint&, int ) ), this, SLOT( itemClicked( Q3ListViewItem*, const QPoint&, int ) ) );
	connect( m_layersListView, SIGNAL( rightButtonClicked( Q3ListViewItem*, const QPoint&, int ) ), this, SLOT( renameItem( Q3ListViewItem*, const QPoint&, int ) ) );
	connect( m_layersListView, SIGNAL( selectionChanged() ), this, SLOT( selectionChangedFromList() ) );
	connect( m_view, SIGNAL( selectionChange() ), this, SLOT( selectionChangedFromTool() ) );
	connect( m_buttonGroup, SIGNAL( clicked( int ) ), this, SLOT( slotButtonClicked( int ) ) );
	//connect( view->part()->commandHistory(), SIGNAL( commandExecuted( VCommand*) ), this, SLOT( slotCommandExecuted( VCommand* ) ) );

	layout->activate();
	setLayout(layout);

	updateLayers();
} // VLayersTab::VLayersTab

VLayersTab::~VLayersTab()
{
} // VLayersTab::~VLayersTab

void
VLayersTab::slotButtonClicked( int ID )
{
	switch( ID )
	{
		case 0:
			addLayer(); break;
		case 1:
			raiseItem(); break;
		case 2:
			lowerItem(); break;
		case 3:
			deleteItem(); break;
	}
} // VLayersTab::slotButtonClicked

void 
VLayersTab::resetSelection()
{
	Q3ListViewItemIterator it( m_layersListView );

	// iterates over all list items and deselects them manually
	// to avoid the list views selectionChanged signal
	for(; it.current(); ++it )
	{
		it.current()->setSelected( false );
		it.current()->repaint();
	}
}

void 
VLayersTab::selectActiveLayer()
{
	/* TODO: porting to flake
	if( ! m_layers[ m_document->activeLayer() ] )
	{
		Q3PtrVector<VLayer> vector;
		m_document->layers().toVector( &vector );
		// find another layer to set active
		for( int i = vector.count() - 1; i >= 0; i-- )
			if ( vector[i]->state() != VObject::deleted )
			{
				m_document->setActiveLayer( vector[i] );
				break;
			}
	}

	// deselect all other layers
	Q3PtrDictIterator<VLayerListViewItem> it( m_layers );
	for(; it.current(); ++it )
	{
		it.current()->setSelected( false );
		it.current()->repaint();
	}

	VLayerListViewItem *layerItem = m_layers[ m_document->activeLayer() ];
	if( layerItem )
	{
		layerItem->setSelected( true );
		layerItem->repaint();
		kDebug(38000) << "selecting active layer: " << layerItem->text() << endl;
	}
	*/
}

void
VLayersTab::selectionChangedFromTool()
{
	/* TODO: porting to flake
	resetSelection();
	removeDeletedObjectsFromList();

	// TODO : use some kind of mapping...
	VObjectListIterator itr = m_document->selection()->objects();
	for( ; itr.current(); ++itr )
		if( itr.current()->state() != VObject::deleted )
		{
			VObject *obj = itr.current();

			VObjectListViewItem *item = m_objects[ obj ];
			if( ! item ) 
			{
				VLayerListViewItem *layerItem = m_layers[ obj->parent() ];
				if( layerItem )
					updateObjects( layerItem->layer(), layerItem );
				else
				{
					VObjectListViewItem *objectItem = m_objects[ obj->parent() ];
					if( objectItem )
						updateObjects( objectItem->object(), objectItem );
					else
						continue;
				}
				item = m_objects[ obj ];
			}
			if( ! item ) continue;

			item->setSelected( true );
			item->update();
		}
	selectActiveLayer();	
	*/
}

void 
VLayersTab::updateChildItems( Q3ListViewItem *item )
{
	Q3ListViewItemIterator it( item );

	// iterator points to item, so make the next item current first
	for( ++it; it.current(); ++it )
	{
		VObjectListViewItem *objectItem = dynamic_cast<VObjectListViewItem *>( it.current() );
		if( ! objectItem ) continue;
		
		if( dynamic_cast<KoLayerShape*>( objectItem->object() ) )
			updateChildItems( objectItem );

		objectItem->update();
		objectItem->repaint();
	}
}

void 
VLayersTab::toggleState( KoShape *obj, int col )
{
	switch( col )
	{
		case 1: // toggle locking
			obj->setLocked( ! obj->isLocked() );
		break;
		case 2: // toggle visibility	
			obj->setVisible( ! obj->isVisible() );
		break;
		default: return;
	}

	/* TODO: porting to flake 
	if( obj->state() < VObject::selected )
		m_document->selection()->take( *obj );
	else
		m_document->selection()->append( obj );
	*/
}

void
VLayersTab::itemClicked( Q3ListViewItem* item, const QPoint &, int col )
{
	if( item )
	{
		VLayerListViewItem *layerItem = dynamic_cast<VLayerListViewItem *>( item );
		if( layerItem )
		{
			if( col == 0 )
			{
				m_document->setActiveLayer( layerItem->layer() );
				selectActiveLayer();
			}
			if( col > 0 )
			{
				toggleState( layerItem->layer(), col );

				layerItem->update();
				layerItem->repaint();

				updateChildItems( layerItem );

				m_view->part()->repaintAllViews();
			}
		}
		else
		{
			VObjectListViewItem *objectItem = static_cast< VObjectListViewItem *>( item );

			if( col == 0 )
			{
				KoShape *obj = objectItem->object();
				/*TODO: porting to flake
				if( obj->isVisible() && ! obj->Locked() )
					obj->setState( VObject::selected );
				*/
			}
			if( col > 0 ) 
			{
				toggleState( objectItem->object(), col );
				
				/* TODO: porting to flake
				if( objectItem->object()->state() == VObject::selected )
					objectItem->setSelected( true );
				else
					objectItem->setSelected( false );
				*/
				objectItem->update();
				objectItem->repaint();

				if( dynamic_cast<KoLayerShape*>( objectItem->object() ) )
					updateChildItems( objectItem );

				m_view->part()->repaintAllViews();
			}
		}
	}
} // VLayersTab::itemClicked

void
VLayersTab::selectionChangedFromList()
{
	m_document->selection()->clear();

	Q3ListViewItemIterator it( m_layersListView );

	// iterate over all list items and add their corresponding object 
	// to the documents selection if the list item is selected and not hidden or locked or both
	for(; it.current(); ++it )
	{
		VObjectListViewItem *objectItem = dynamic_cast<VObjectListViewItem *>( it.current() );
		if( ! objectItem ) continue;

		/* TODO: porting to flake
		VObject::VState state = objectItem->object()->state();
		
		if( state == VObject::deleted ) 
		{
			delete objectItem;
			continue;
		}

		if( objectItem->isSelected() && (state != VObject::hidden) && (state != VObject::normal_locked ) 
		&& (state != VObject::hidden_locked) )
		{
			m_document->selection()->append( objectItem->object() );
			objectItem->repaint();
		}
		*/
	}
	
	m_view->selectionChanged();
	m_view->part()->repaintAllViews();
}

void
VLayersTab::renameItem( Q3ListViewItem* item, const QPoint&, int col )
{
	/* TODO: porting to flake
	if ( ( item ) && col == 0 )
	{
		bool ok = true;
		VLayerListViewItem* layerItem = dynamic_cast<VLayerListViewItem *>( item );

		VObjectListViewItem *objectItem = dynamic_cast< VObjectListViewItem *>( item );
		if( !layerItem  && objectItem )
		{
			KoShape *obj = objectItem->object();
			QString name = KInputDialog::getText( i18n( "Current Object" ), i18n( "Change the name of the object:" ),
												  obj->name(), &ok, this );
			if( ok )
			{
				m_document->setObjectName( obj, name );
				objectItem->update();
			}
		}
		else
		{
			QString name = KInputDialog::getText( i18n( "Rename Layer" ), i18n( "Change the name of the current layer:" ),
												  layerItem->layer()->name(), &ok, this );
			if( ok )
			{
				layerItem->layer()->setName( name );
				layerItem->update();
			}
		}
	}
	*/
} // VLayersTab::renameItem

void
VLayersTab::addLayer()
{
	bool ok = true;
	QString name = KInputDialog::getText( i18n( "New Layer" ), i18n( "Enter the name of the new layer:" ),
										  i18n( "New layer" ), &ok, this );
	if( ok )
	{
		KoLayerShape* layer = new KoLayerShape();
		/*TODO: porting to flake
		layer->setName( name );
		VLayerCmd* cmd = new VLayerCmd( m_document, i18n( "Add Layer" ),
				layer, VLayerCmd::addLayer );
		m_view->part()->addCommand( cmd, true );
		updateLayers();
		*/
		m_document->insertLayer( layer );
	}
} // VLayersTab::addLayer

void
VLayersTab::raiseItem()
{
	/* TODO: porting to flake
	VCommand *cmd = 0L;
	//QListViewItem *newselection = 0L;
	Q3ListViewItemIterator it( m_layersListView );

	for(; it.current(); ++it )
	{
		if( ! it.current()->isSelected() ) continue;

		VLayerListViewItem* layerItem = dynamic_cast<VLayerListViewItem *>( it.current() );
		if( layerItem )
		{
			VLayer *layer = layerItem->layer();
			if( layer && m_document->canRaiseLayer( layer ) )
			{
				cmd = new VLayerCmd( m_document, i18n( "Raise Layer" ),
								layerItem->layer(), VLayerCmd::raiseLayer );
				//newselection = layerItem;
			}
		}
		else
		{
			VObjectListViewItem* item = dynamic_cast< VObjectListViewItem *>( it.current() );
			if( item )
			{
				cmd = new VZOrderCmd( m_document, item->object(), VZOrderCmd::up );
				//newselection = item;
			}
		}
		if( cmd )
		{
			m_view->part()->addCommand( cmd, true );
			//if( newselection )
			//	m_layersListView->setSelected( newselection, true );
		}
	}
	if( cmd ) updatePreviews();
	*/
} // VLayersTab::raiseItem

void
VLayersTab::lowerItem()
{
	/* TODO: porting to flake
	VCommand *cmd = 0L;
	Q3ListViewItemIterator it( m_layersListView );

	for(; it.current(); ++it )
	{
		if( ! it.current()->isSelected() ) continue;
		
		VLayerListViewItem* layerItem = dynamic_cast<VLayerListViewItem *>( it.current() );
		if( layerItem )
		{
			VLayer *layer = layerItem->layer();
			if( layer && m_document->canLowerLayer( layer ) )
				cmd = new VLayerCmd( m_document, i18n( "Lower Layer" ), layer, VLayerCmd::lowerLayer );
		}
		else
		{
			VObjectListViewItem* item = dynamic_cast< VObjectListViewItem *>( it.current() );
			if( item )
				cmd = new VZOrderCmd( m_document, item->object(), VZOrderCmd::down );
		}
		if( cmd )
		{
			m_view->part()->addCommand( cmd, true );
		}
	}
	if( cmd ) updatePreviews();
	*/
} // VLayersTab::lowerItem

void
VLayersTab::deleteItem()
{
	/* TODO: porting to flake
	VCommand *cmd = 0L;
	Q3ListViewItemIterator it( m_layersListView );

	Q3PtrList<Q3ListViewItem> deleteItems;
	deleteItems.setAutoDelete( false );

	// collect all selected items because they get deselected
	// when the first item is removed
	for(; it.current(); ++it )
	{
		if( ! it.current()->isSelected() ) continue;
			deleteItems.append( it.current() );
	}
	
	for( ;deleteItems.first(); )
	{
		VLayerListViewItem* layerItem = dynamic_cast< VLayerListViewItem *>( deleteItems.current() );
		if( layerItem )
		{
			VLayer *layer = layerItem->layer();
			if( layer && m_layers.count() > 1 )
			{
				cmd = new VLayerCmd( m_document, i18n( "Delete Layer" ), layer, VLayerCmd::deleteLayer );

				VObjectListIterator itr = layer->objects();
				// iterate over this layers child objects and remove them from the internal 
				// object list and the list of the to be deleted items
				for( ; itr.current(); ++itr )
				{
					VObjectListViewItem *objectItem = m_objects.take( itr.current() );
					deleteItems.remove( objectItem );
				}
				
				delete layerItem;

				m_view->part()->addCommand( cmd );
				
				selectActiveLayer();
			}
		}
		else
		{
			VObjectListViewItem* item = dynamic_cast< VObjectListViewItem *>( deleteItems.current() );
			if( item )
			{
				cmd = new VDeleteCmd( m_document, item->object() );
				
				delete item;

				m_view->part()->addCommand( cmd );
			}
		}
		// remove first item, next item becomes current
		deleteItems.removeFirst();
	}
	if( cmd ) 
	{
		updatePreviews();
		m_view->part()->repaintAllViews();
	}
	*/
} // VLayersTab::deleteItem

void
VLayersTab::updatePreviews()
{
	// TODO: Optimization: call update() on each view item...
	updateLayers();
} // VLayersTab::updatePreviews

void
VLayersTab::updateLayers()
{
	removeDeletedObjectsFromList();	

	VLayerListViewItem* item = 0L;
	
	int layerCnt = m_document->layers().size();

	m_layersListView->clear();
	
	const VLayerList &l = m_document->layers();

	for( int i = layerCnt-1; i >= 0; --i )
	{
		KoLayerShape *layer = (KoLayerShape*)l.at( i );
		item = new VLayerListViewItem( m_layersListView, layer, m_document, &m_layers );
		item->setOpen( true );
		item->setKey(i);

		updateObjects( layer, item );
	}
	/*
	Q3PtrVector<VLayer> vector;
	m_document->layers().toVector( &vector );
	VLayerListViewItem* item = 0L;
	for( int i = vector.count() - 1; i >= 0; i-- )
	{
		if ( vector[i]->state() != VObject::deleted )
		{
			if( !m_layers[ vector[i] ] )
			{
				item = new VLayerListViewItem( m_layersListView, vector[i], m_document, &m_layers );
				item->setOpen( true );
			}
			else 
				item = m_layers[ vector[i] ];

			item->setKey(i);

			updateObjects( vector[i], item );
		}
	}
	*/
	selectActiveLayer();
	m_layersListView->sort();
} // VLayersTab::updateLayers

void
VLayersTab::updateObjects( KoShapeContainer *object, Q3ListViewItem *item )
{
	uint key = 0;
	foreach( KoShape* shape, object->iterator() )
	{
		VObjectListViewItem *objectItem = new VObjectListViewItem( item, shape, m_document, key++, &m_objects );
		objectItem->setKey( key );
	}
	item->sort();

	/* TODO: porting to flake
	VObjectListIterator itr = dynamic_cast<VGroup *>( object )->objects();

	for( uint objcount = 1; itr.current(); ++itr, objcount++ )
		if( itr.current()->state() != VObject::deleted )
		{
			VObjectListViewItem *objectItem = m_objects[ itr.current() ];
			if( ! objectItem )
			{
				// object not found -> insert
				objectItem = new VObjectListViewItem( item, itr.current(), m_document, objcount, &m_objects );
				objectItem->update();
			}
			else if( objectItem->parent() != item )
			{
				// object found, but has false parent -> reparent
				objectItem->parent()->takeItem( objectItem );
				item->insertItem( objectItem );
			}

			objectItem->setKey( objcount );

			if( dynamic_cast<VGroup *>( itr.current() ) )
				updateObjects( itr.current(), objectItem );
		}

	item->sort();
	*/
}

void
VLayersTab::removeDeletedObjectsFromList()
{
	/* TODO: porting to flake
	Q3PtrDictIterator<VObjectListViewItem> it( m_objects );

	// iterate over all object items and delete the following items:
	// - items representing deleted objects
	// - items with objects objects that changed parents
	// BEWARE: when deleting an item, the iterator is automatically incremented
	for(; it.current(); )
	{
		VLayerListViewItem *layerItem = dynamic_cast<VLayerListViewItem*>( it.current()->parent() );
		if( layerItem )
		{
			VGroup *group = dynamic_cast<VGroup*>( layerItem->layer() );
			// check if object of item is still child of object of parent item
			if( group && ! group->objects().contains( it.current()->object() ) )
			{
				layerItem->takeItem( it.current() );
				delete it.current();
				continue;
			}
		}
		else
		{
			VObjectListViewItem *objectItem = dynamic_cast<VObjectListViewItem*>( it.current()->parent() );
			if( objectItem ) 
			{
				VGroup *group = dynamic_cast<VGroup*>( objectItem->object() );
				// check if object of item is still child of object of parent item
				if( group && ! group->objects().contains( it.current()->object() ) )
				{
					objectItem->takeItem( it.current() );
					delete it.current();
					continue;
				}
			}
			else
			{
				delete it.current();
				continue;
			}
		}

		if( it.current()->object()->state() == VObject::deleted )
		{
			delete it.current();
			continue;
		}

		++it;
	}

	Q3PtrDictIterator<VLayerListViewItem> itr( m_layers );

	// iterate over all layer items and delete the following items:
	// - items representing deleted layers
	// BEWARE: when deleting an item, the iterator is automatically incremented
	for(; itr.current(); )
	{
		if( itr.current()->layer()->state() == VObject::deleted )
		{
			m_layersListView->takeItem( itr.current() );
			delete itr.current();
			continue;
		}
		++itr;
	}
	*/
}


void 
VLayersTab::slotCommandExecuted( VCommand* command )
{
	// sync listview on changing layers or deleting/undeleting or grouping/ungrouping objects
/* TODO Port to flake
	if( dynamic_cast<VLayerCmd*>( command ) 
	|| dynamic_cast<VDeleteCmd*>( command ) 
	|| dynamic_cast<VZOrderCmd*>( command ) )
		updateLayers();*/
}
#include "vdocumentdocker.moc"
