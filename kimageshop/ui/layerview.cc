/*
 *  layerview.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
 *                1999 Michael Koch    <koch@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

// only for debug
#include <iostream.h>

#include <qpainter.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qpopupmenu.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qstring.h>

#include <kstddirs.h>
#include <klocale.h>
#include <knuminput.h>

#include "kis_util.h"
#include "kis_doc.h"
#include "kis_factory.h"
#include "layerview.h"
#include "kis_dlg_layer.h"

LayerView::LayerView( QWidget* _parent, const char* _name )
  : QTableView( _parent, _name )
{
  init( 0 );
}

LayerView::LayerView( KisDoc* doc, QWidget* _parent, const char* _name )
  : QTableView( _parent, _name )
{
  init( doc );
}

void LayerView::init( KisDoc* doc )
{
  setTableFlags( Tbl_autoHScrollBar | Tbl_autoVScrollBar );

  m_doc = doc;

  setBackgroundColor( white );
  updateTable();

  setCellWidth( CELLWIDTH );
  setCellHeight( CELLHEIGHT );
  m_selected = m_doc->layerList().count() - 1;

  QPopupMenu *submenu = new QPopupMenu();

  submenu->insertItem( i18n( "Upper" ), UPPERLAYER );
  submenu->insertItem( i18n( "Lower" ), LOWERLAYER );
  submenu->insertItem( i18n( "Most front" ), FRONTLAYER );
  submenu->insertItem( i18n( "Most back" ), BACKLAYER );

  m_contextmenu = new QPopupMenu();

  m_contextmenu->setCheckable(TRUE);

  m_contextmenu->insertItem( i18n( "Visible" ), VISIBLE );
  m_contextmenu->insertItem( i18n( "Selection"), SELECTION );
  m_contextmenu->insertItem( i18n( "Level" ), submenu );
  m_contextmenu->insertItem( i18n( "Linked"), LINKING );
  m_contextmenu->insertItem( i18n( "Properties"), PROPERTIES );

  m_contextmenu->insertSeparator();

  m_contextmenu->insertItem( i18n( "Add Layer" ), ADDLAYER );
  m_contextmenu->insertItem( i18n( "Remove Layer"), REMOVELAYER );
  m_contextmenu->insertItem( i18n( "Add Mask" ), ADDMASK );
  m_contextmenu->insertItem( i18n( "Remove Mask"), REMOVEMASK );

  connect( m_contextmenu, SIGNAL( activated( int ) ), SLOT( slotMenuAction( int ) ) );
  connect( submenu, SIGNAL( activated( int ) ), SLOT( slotMenuAction( int ) ) );

  connect( doc, SIGNAL( layersUpdated()), this, SLOT( slotDocUpdated () ) );
}

void LayerView::slotDocUpdated()
{
  updateTable();
  updateAllCells();
}

void LayerView::paintCell( QPainter* _painter, int _row, int )
{
  if( _row == m_selected )
  {
    _painter->fillRect( 0, 0, cellWidth( 0 ) - 1, cellHeight() - 1, gray);
  }

  style().drawPanel( _painter, LayerDialog::m_eyeRect.x(), 
		     LayerDialog::m_eyeRect.y(),
		     LayerDialog::m_eyeRect.width(), 
		     LayerDialog::m_eyeRect.height(), colorGroup(),
		      true );
  if( m_doc->layerList().at( _row )->isVisible() )
  {
    _painter->drawPixmap( LayerDialog::m_eyeRect.topLeft(), *LayerDialog::m_eyeIcon );
  }

  style().drawPanel( _painter, LayerDialog::m_linkRect.x(), 
		     LayerDialog::m_linkRect.y(),
		     LayerDialog::m_linkRect.width() , 
		     LayerDialog::m_linkRect.height(), colorGroup(),
		     true );
  if( m_doc->layerList().at( _row )->isLinked() )
  {
    _painter->drawPixmap( LayerDialog::m_linkRect.topLeft(), *LayerDialog::m_linkIcon );
  }

  //  style().drawPanel( _painter, LayerDialog::m_previewRect.topLeft(), ....
  
  _painter->drawRect( 0, 0, cellWidth( 0 ) - 1, cellHeight() - 1);
  _painter->drawText( 80, 20, m_doc->layerList().at( _row )->name() );
}

void LayerView::updateTable()
{
  if( m_doc )
  {
    m_items = m_doc->layerList().count();
    setNumRows( m_items );
    setNumCols( 1 );
  }
  else
  {
    m_items = 0;
    setNumRows( 0 );
    setNumCols( 0 );
  }
  resize( sizeHint() );
}

void LayerView::update_contextmenu( int _index )
{
  m_contextmenu->setItemChecked( VISIBLE, m_doc->layerList().at( _index )->isVisible() );
  m_contextmenu->setItemChecked( LINKING, m_doc->layerList().at( _index )->isLinked() );
}

void LayerView::selectLayer( int _index )
{
  int currentSel = m_selected;
  m_selected = -1;
  updateCell( currentSel, 0 );
  m_selected = _index;
  m_doc->setCurrentLayer( m_selected );
  updateCell( m_selected, 0 );
}

void LayerView::slotInverseVisibility( int _index )
{
  m_doc->layerList().at( _index )->setVisible( !m_doc->layerList().at( _index )->isVisible() );
  updateCell( _index, 0 );
  m_doc->compositeImage( m_doc->layerList().at( _index )->imageExtents() );
}

void LayerView::slotInverseLinking( int _index )
{
  m_doc->layerList().at( _index )->setLinked( !m_doc->layerList().at( _index )->isLinked() );
  updateCell( _index, 0 );
}

void LayerView::slotMenuAction( int _id )
{
  switch( _id )
  {
    case VISIBLE:
      slotInverseVisibility( m_selected );
      break;
    case LINKING:
      slotInverseLinking( m_selected );
      break;
    case PROPERTIES:
      slotProperties();
      break;
    case ADDLAYER:
      slotAddLayer();
      break;
    case REMOVELAYER:
      slotRemoveLayer();
      break;
    case UPPERLAYER:
      slotRaiseLayer();
      break;
    case LOWERLAYER:
      slotLowerLayer();
      break;
    case FRONTLAYER:
      slotFrontLayer();
      break;
    case BACKLAYER:
      slotBackgroundLayer();
      break;
    default:
      cerr << "Unknown context menu action" << endl;
      break;
  }
}

QSize LayerView::sizeHint() const
{
  return QSize( CELLWIDTH, CELLHEIGHT * 5 );
}

void LayerView::mousePressEvent( QMouseEvent *_event )
{
  int row = findRow( _event->pos().y() );
  QPoint localPoint( _event->pos().x() % cellWidth(), _event->pos().y() % cellHeight() );

  if( _event->button() & LeftButton )
  {
    if( LayerDialog::m_eyeRect.contains( localPoint ) )
    {
      slotInverseVisibility( row );
    }
    else if( LayerDialog::m_linkRect.contains( localPoint ) )
    {
      slotInverseLinking( row );
    }
    else if( row != -1 )
    {
      selectLayer( row );
    }
  }
  else if( _event->button() & RightButton )
  {
    // TODO: Should the Layer under the cursor selected when clicking RMB ?
	// Matthias: IMO it should.

    selectLayer( row );
    update_contextmenu( row );
    m_contextmenu->popup( mapToGlobal( _event->pos() ) );
  }
}

void LayerView::mouseDoubleClickEvent( QMouseEvent *_event )
{
  if( _event->button() & LeftButton )
  {
    slotProperties();
  }
}

void LayerView::slotAddLayer()
{
  cout << "LayerView::slotAddLayer()" << endl;

  QString image = locate( "kis_images", "cam9b.jpg", KisFactory::global() );	
  m_doc->addRGBLayer( image );
  m_doc->setLayerOpacity( 255 );

  QRect updateRect = m_doc->layerList().at( m_doc->layerList().count() - 1 )->imageExtents();
  m_doc->compositeImage( updateRect );

  selectLayer( m_doc->layerList().count() - 1 );

  updateTable();
  updateAllCells();
}

void LayerView::slotRemoveLayer()
{
  cout << "LayerView::slotRemoveLayer()" << endl;

  if( m_doc->layerList().count() != 0 )
  {
    QRect updateRect = m_doc->layerList().at( m_selected )->imageExtents();

    m_doc->removeLayer( m_selected );

    m_doc->compositeImage( updateRect );

    if( m_selected == (int)m_doc->layerList().count() )
      m_selected--;

    updateTable();
    updateAllCells();
  }
}

void LayerView::swapLayers( int a, int b )
{
  cout << "LayerView::swapLayers" << endl;

  if( ( m_doc->layerList().at( a )->isVisible() ) &&
      ( m_doc->layerList().at( b )->isVisible() ) )
  {
    QRect l1 = m_doc->layerList().at( a )->imageExtents();
    QRect l2 = m_doc->layerList().at( b )->imageExtents();

    if( l1.intersects( l2 ) )
    {
      QRect rect = l1.intersect( l2 );

      m_doc->compositeImage( rect );
    }
  }
}

void LayerView::slotRaiseLayer()
{
  cout << "LayerView::slotRaiseLayer()" << endl;

  int newpos = m_selected > 0 ? m_selected - 1 : 0;

  if( m_selected != newpos )
  {
    m_doc->upperLayer( m_selected );
    repaint();
    swapLayers( m_selected, newpos );
    selectLayer( newpos );
  }
}

void LayerView::slotLowerLayer()
{
  cout << "LayerView::slotLowerLayer()" << endl;

  int newpos = ( m_selected + 1 ) < (int)m_doc->layerList().count() ? m_selected + 1 : m_selected;

  if( m_selected != newpos )
  {
    m_doc->lowerLayer( m_selected );
    repaint();
    swapLayers( m_selected, newpos );
    selectLayer( newpos );
  }
}

void LayerView::slotFrontLayer()
{
  cout << "LayerView::slotFrontLayer" << endl;

  if( m_selected != (int) ( m_doc->layerList().count() - 1 ) )
  {
    m_doc->setFrontLayer( m_selected );
    selectLayer( m_doc->layerList().count() - 1 );

    QRect updateRect = m_doc->layerList().at( m_selected )->imageExtents();
    m_doc->compositeImage( updateRect );

    updateAllCells();
  }
}

void LayerView::slotBackgroundLayer()
{
  cout << "LayerView::slotBackgroundLayer" << endl;

  if( m_selected != 0 )
  {
    m_doc->setBackgroundLayer( m_selected );

    selectLayer( 0 );

    QRect updateRect = m_doc->layerList().at( m_selected )->imageExtents();
    m_doc->compositeImage( updateRect );

    updateAllCells();
  }
}

void LayerView::updateAllCells()
{
  for( int i = 0; i < (int)m_doc->layerList().count(); i++ )
    updateCell( i, 0 );
}

void LayerView::slotProperties()
{
  cout << "LayerView::slotProperties()" << endl;

  if( LayerPropertyDialog::editProperties( *( m_doc->layerList().at( m_selected ) ) ) )
  {
    QRect updateRect = m_doc->layerList().at( m_selected )->imageExtents();

    updateCell( m_selected, 0 );
    m_doc->compositeImage( updateRect );
  }
}

LayerPropertyDialog::LayerPropertyDialog( QString _layername, uchar _opacity, QWidget *_parent, const char *_name )
  : QDialog( _parent, _name, true )
{
  QGridLayout *layout = new QGridLayout( this, 3, 2, 15, 7 );

  m_name = new QLineEdit( _layername, this );
  layout->addWidget( m_name, 0, 1 );

  QLabel *label1 = new QLabel( m_name, i18n( "Name" ), this );
  layout->addWidget( label1, 0, 0 );

  m_spin = new KIntSpinBox( 0, 255, 1, _opacity, 10, this );
  layout->addWidget( m_spin, 1, 1 );

  QLabel *label2 = new QLabel( m_spin, i18n( "Opacity" ), this );
  layout->addWidget( label2, 1, 0 );

  QPushButton *pbOk = new QPushButton( i18n( "Ok" ), this );
  pbOk->setDefault( true );
  layout->addWidget( pbOk, 2, 0 );
  QObject::connect( pbOk, SIGNAL( clicked() ), this, SLOT( accept() ) );

  QPushButton *pbCancel = new QPushButton( i18n( "Cancel" ), this );
  layout->addWidget( pbCancel, 2, 1 );
  QObject::connect( pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

bool LayerPropertyDialog::editProperties( Layer &_layer )
{
  LayerPropertyDialog *dialog;

  dialog = new LayerPropertyDialog( _layer.name(), _layer.opacity(), NULL, "opacity_dialog" );
  if( dialog->exec() == Accepted )
  {
    _layer.setName( dialog->m_name->text() );
    _layer.setOpacity( dialog->m_spin->value() );

    return true;
  }
  return false;
}

#include "layerview.moc"

