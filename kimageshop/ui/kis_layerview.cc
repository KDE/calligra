/*
 *  kis_layerview.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
 *                1999 Michael Koch    <koch@kde.org>
 *                2000 Matthias Elter  <elter@kde.org>
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

#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qstring.h>
#include <qslider.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qlineedit.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>

#include <kstddirs.h>
#include <iostream>
#include <klocale.h>
#include <knuminput.h>
#include <kiconloader.h>
#include <kmessagebox.h>

#include "kis_doc.h"
#include "kis_util.h"
#include "kis_layerview.h"
#include "kis_factory.h"
#include "kis_framebutton.h"
#include "integerwidget.h"

//#define KISBarIcon( x ) BarIcon( x, KisFactory::global() )

using namespace std;

KisLayerView::KisLayerView( KisDoc *_doc, QWidget *_parent, const char *_name )
  : QWidget( _parent, _name )
{
    QVBoxLayout *layout = new QVBoxLayout( this );

    LayerTable* layertable = new LayerTable( _doc, this, "layerlist" );
    layout->addWidget( layertable, 1 );

    QHBox *buttons = new QHBox( this );
    layout->addWidget( buttons );

    KisFrameButton* pbAddLayer = new KisFrameButton( buttons );
    pbAddLayer->setPixmap( BarIcon( "newlayer" ) );
    connect( pbAddLayer, SIGNAL( clicked() ), layertable, SLOT( slotAddLayer() ) );

    KisFrameButton* pbRemoveLayer = new KisFrameButton( buttons );
    pbRemoveLayer->setPixmap( BarIcon( "deletelayer" ) );
    connect( pbRemoveLayer, SIGNAL( clicked() ), layertable, SLOT( slotRemoveLayer() ) );

    KisFrameButton* pbUp = new KisFrameButton( buttons );
    pbUp->setPixmap( BarIcon( "raiselayer" ) );
    connect( pbUp, SIGNAL( clicked() ), layertable, SLOT( slotRaiseLayer() ) );

    KisFrameButton* pbDown = new KisFrameButton( buttons );
    pbDown->setPixmap( BarIcon( "lowerlayer" ) );
    connect( pbDown, SIGNAL( clicked() ), layertable, SLOT( slotLowerLayer() ) );
}

LayerTable::LayerTable( QWidget* _parent, const char* _name )
  : QTableView( _parent, _name )
{
    init( 0 );
}

LayerTable::LayerTable( KisDoc* doc, QWidget* _parent, const char* _name )
  : QTableView( _parent, _name )
{
    init( doc );
}


void LayerTable::init( KisDoc* doc )
{
    setTableFlags( Tbl_autoHScrollBar | Tbl_autoVScrollBar );

    m_doc = doc;

    setBackgroundColor( white );

    // load icon pixmaps
    QString _icon = locate( "kis_pics", "eye.png", KisFactory::global() );
    m_eyeIcon = new QPixmap;
    if( !m_eyeIcon->load( _icon ) )
	KMessageBox::error( this, "Can't find eye.png", "Canvas" );
    m_eyeRect = QRect( QPoint( 2,( CELLHEIGHT - m_eyeIcon->height() ) / 2 ), 
        m_eyeIcon->size() );

    _icon = locate( "kis_pics", "link.png", KisFactory::global() );
    m_linkIcon = new QPixmap;
    if( !m_linkIcon->load( _icon ) )
	KMessageBox::error( this, "Can't find link.png", "Canvas" );
    m_linkRect = QRect( QPoint( 25,( CELLHEIGHT - m_linkIcon->height() ) / 2 ), 
        m_linkIcon->size() );

    // HACK - the size of the preview image should be configurable somewhere
    m_previewRect = QRect( QPoint( 50, (CELLHEIGHT - m_linkIcon->height() ) /2 ),
	 m_linkIcon->size() );

    updateTable();

    setCellWidth( CELLWIDTH );
    setCellHeight( CELLHEIGHT );
    m_selected = m_doc->current()->layerList().count() - 1;

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

void LayerTable::slotDocUpdated()
{
    updateTable();
    updateAllCells();
}

void LayerTable::paintCell( QPainter* _painter, int _row, int )
{
    if( _row == m_selected )
    {
        _painter->fillRect( 0, 0, cellWidth( 0 ) - 1, cellHeight() - 1, gray);
    }

    style().drawPanel( _painter, m_eyeRect.x(),
		     m_eyeRect.y(),
		     m_eyeRect.width(),
		     m_eyeRect.height(), colorGroup(),
		      true );
                      
    if( m_doc->current()->layerList().at( _row )->visible() )
    {
        _painter->drawPixmap( m_eyeRect.topLeft(), *m_eyeIcon );
    }

    style().drawPanel( _painter, m_linkRect.x(),
		     m_linkRect.y(),
		     m_linkRect.width() ,
		     m_linkRect.height(), colorGroup(),
		     true );
                     
    if( m_doc->current()->layerList().at( _row )->linked() )
    {
        _painter->drawPixmap( m_linkRect.topLeft(), *m_linkIcon );
    }
    
    // jwc - this needs adjusting 
    style().drawPanel( _painter,    
                    m_previewRect.x(), 
                    m_previewRect.y(), 
                    m_previewRect.width(), 
                    m_previewRect.height(),                                     
                    colorGroup(), true );

    _painter->drawRect( 0, 0, cellWidth( 0 ) - 1, cellHeight() - 1);
    _painter->drawText( 80, 20, m_doc->current()->layerList().at( _row )->name() );
}

void LayerTable::updateTable()
{
    if( m_doc->current() )
    {
        m_items = m_doc->current()->layerList().count();
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

void LayerTable::update_contextmenu( int _index )
{
    m_contextmenu->setItemChecked( VISIBLE, m_doc->current()->layerList().at( _index )->visible() );
    m_contextmenu->setItemChecked( LINKING, m_doc->current()->layerList().at( _index )->linked() );
}

void LayerTable::selectLayer( int _index )
{
    int currentSel = m_selected;
    m_selected = -1;
    
    updateCell( currentSel, 0 );
    m_selected = _index;
    m_doc->current()->setCurrentLayer( m_selected );
    updateCell( m_selected, 0 );
}

void LayerTable::slotInverseVisibility( int _index )
{
    m_doc->current()->layerList().at( _index )->setVisible( !m_doc->current()->layerList().at( _index )->visible() );
    updateCell( _index, 0 );
  
    m_doc->current()->markDirty( m_doc->current()->layerList().at( _index )->imageExtents() );
}


void LayerTable::slotInverseLinking( int _index )
{
    m_doc->current()->layerList().at( _index )->setLinked( !m_doc->current()->layerList().at( _index )->linked() );
    updateCell( _index, 0 );
}


void LayerTable::slotMenuAction( int _id )
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


QSize LayerTable::sizeHint() const
{
  return QSize( CELLWIDTH, CELLHEIGHT * 5 );
}


void LayerTable::mousePressEvent( QMouseEvent *_event )
{
  int row = findRow( _event->pos().y() );
  QPoint localPoint( _event->pos().x() % cellWidth(), _event->pos().y() % cellHeight() );

  if( _event->button() & LeftButton )
  {
    if( m_eyeRect.contains( localPoint ) )
    {
      slotInverseVisibility( row );
    }
    else if( m_linkRect.contains( localPoint ) )
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


void LayerTable::mouseDoubleClickEvent( QMouseEvent *_event )
{
    if( _event->button() & LeftButton )
    {
        slotProperties();
    }
}


void LayerTable::slotAddLayer()
{
    KisImage *img = m_doc->current();

    QString name;
    name.sprintf("layer %d", img->layerList().count());

    img->addLayer( img->imageExtents(), KisColor::white(), true, name );

    QRect updateRect = img->layerList().at( img->layerList().count() - 1 )->imageExtents();
    img->markDirty( updateRect );

    selectLayer( img->layerList().count() - 1 );

    updateTable();
    updateAllCells();
}



void LayerTable::slotRemoveLayer()
{
  if( m_doc->current()->layerList().count() != 0 )
  {
    QRect updateRect = m_doc->current()->layerList().at( m_selected )->imageExtents();
    m_doc->current()->removeLayer( m_selected );
    m_doc->current()->markDirty( updateRect );

    if( m_selected == (int)m_doc->current()->layerList().count() )
      m_selected--;

    updateTable();
    updateAllCells();
  }
}

void LayerTable::swapLayers( int a, int b )
{
    if( ( m_doc->current()->layerList().at( a )->visible() ) &&
      ( m_doc->current()->layerList().at( b )->visible() ) )
    {
        QRect l1 = m_doc->current()->layerList().at( a )->imageExtents();
        QRect l2 = m_doc->current()->layerList().at( b )->imageExtents();

        if( l1.intersects( l2 ) )
        {
            QRect rect = l1.intersect( l2 );
            m_doc->current()->markDirty( rect );
        }
    }
}


void LayerTable::slotRaiseLayer()
{
    int newpos = m_selected > 0 ? m_selected - 1 : 0;

    if( m_selected != newpos )
    {
        m_doc->current()->upperLayer( m_selected );
        repaint();
        swapLayers( m_selected, newpos );
        selectLayer( newpos );
    }
}


void LayerTable::slotLowerLayer()
{
    int newpos = ( m_selected + 1 ) < (int)m_doc->current()->layerList().count() ? m_selected + 1 : m_selected;

    if( m_selected != newpos )
    {
        m_doc->current()->lowerLayer( m_selected );
        repaint();
        swapLayers( m_selected, newpos );
        selectLayer( newpos );
    }
}


void LayerTable::slotFrontLayer()
{
  if( m_selected != (int) ( m_doc->current()->layerList().count() - 1 ) )
  {
    m_doc->current()->setFrontLayer( m_selected );
    selectLayer( m_doc->current()->layerList().count() - 1 );

    QRect updateRect = m_doc->current()->layerList().at( m_selected )->imageExtents();
    m_doc->current()->markDirty( updateRect );

    updateAllCells();
  }
}

void LayerTable::slotBackgroundLayer()
{

  if( m_selected != 0 )
  {
    m_doc->current()->setBackgroundLayer( m_selected );

    selectLayer( 0 );

    QRect updateRect = m_doc->current()->layerList().at( m_selected )->imageExtents();
    m_doc->current()->markDirty( updateRect );

    updateAllCells();
  }
}

void LayerTable::updateAllCells()
{
    for( int i = 0; i < (int)m_doc->current()->layerList().count(); i++ )
        updateCell( i, 0 );
}


void LayerTable::slotProperties()
{

    if( LayerPropertyDialog::editProperties( *( m_doc->current()->layerList().at( m_selected ) ) ) )
    {
        QRect updateRect = m_doc->current()->layerList().at( m_selected )->imageExtents();

        updateCell( m_selected, 0 );
        m_doc->current()->markDirty( updateRect );
    }
  
}

LayerPropertyDialog::LayerPropertyDialog( QString _layername, uchar _opacity, QWidget *_parent, const char *_name )
  : QDialog( _parent, _name, true )
{
    QGridLayout *layout = new QGridLayout( this, 4, 2, 15, 7 );

    m_name = new QLineEdit( _layername, this );
    layout->addWidget( m_name, 0, 1 );

    QLabel *lblName = new QLabel( m_name, i18n( "Name" ), this );
    layout->addWidget( lblName, 0, 0 );

/*
  m_spin = new KIntSpinBox( 0, 255, 1, _opacity, 10, this );
  layout->addWidget( m_spin, 1, 1 );

  QLabel *label2 = new QLabel( m_spin, i18n( "Opacity" ), this );
  layout->addWidget( label2, 1, 0 );
*/

    m_opacity = new IntegerWidget( 0, 255, this );
    m_opacity->setValue( _opacity );
    m_opacity->setTickmarks( QSlider::Below );
    m_opacity->setTickInterval( 32 );
    layout->addWidget( m_opacity, 1, 1 );

    QLabel *lblOpacity = new QLabel( m_opacity, i18n( "Opacity" ), this );
    layout->addWidget( lblOpacity, 1, 0 );

    layout->setRowStretch( 2, 1 );

    QHBox *buttons = new QHBox( this );
    layout->addMultiCellWidget( buttons, 3, 4, 0, 1 );

    (void) new QWidget( buttons );

    QPushButton *pbOk = new QPushButton( i18n( "OK" ), buttons );
    pbOk->setDefault( true );
    QObject::connect( pbOk, SIGNAL( clicked() ), this, SLOT( accept() ) );

    QPushButton *pbCancel = new QPushButton( i18n( "Cancel" ), buttons );
    QObject::connect( pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}


bool LayerPropertyDialog::editProperties( KisLayer &_layer )
{
    LayerPropertyDialog *dialog;

    dialog = new LayerPropertyDialog( _layer.name(), _layer.opacity(), NULL, "opacity_dialog" );
    if( dialog->exec() == Accepted )
    {
        _layer.setName( dialog->m_name->text() );
        _layer.setOpacity( dialog->m_opacity->value() );

        return true;
    }
    return false;
}

#include "kis_layerview.moc"

