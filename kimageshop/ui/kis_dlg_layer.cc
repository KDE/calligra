/*
 *  layerdlg.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Michael Koch    <koch@kde.org>
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
#include <qlayout.h>
#include <qpushbutton.h>
#include <qmessagebox.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kstddirs.h>
#include <iostream.h>
#include "kis_dlg_layer.h"
#include "layerview.h"
#include "channelview.h"
#include "kis_doc.h"
#include "kis_factory.h"

QPixmap *LayerDialog::m_eyeIcon, *LayerDialog::m_linkIcon;
QRect LayerDialog::m_eyeRect, LayerDialog::m_linkRect;
QRect LayerDialog::m_previewRect;

LayerTab::LayerTab( KisDoc *_doc, QWidget *_parent, const char *_name , WFlags _flags )
  : QWidget( _parent, _name, _flags )
{
  QVBoxLayout *layout = new QVBoxLayout( this );

  LayerView* layerview = new LayerView( _doc, this, "layerlist" );
  layout->addWidget( layerview, 1 );

  QHBox *buttons = new QHBox( this );
  layout->addWidget( buttons );

  QPushButton* pbAddLayer = new QPushButton( buttons, "addlayer" );
  pbAddLayer->setPixmap( BarIcon( "newlayer" ) );
  connect( pbAddLayer, SIGNAL( clicked() ), layerview, SLOT( slotAddLayer() ) );

  QPushButton* pbRemoveLayer = new QPushButton( buttons, "removelayer" );
  pbRemoveLayer->setPixmap( BarIcon( "deletelayer" ) );
  connect( pbRemoveLayer, SIGNAL( clicked() ), layerview, SLOT( slotRemoveLayer() ) );

  QPushButton* pbUp = new QPushButton( buttons, "up" );
  pbUp->setPixmap( BarIcon( "raiselayer" ) );
  connect( pbUp, SIGNAL( clicked() ), layerview, SLOT( slotRaiseLayer() ) );

  QPushButton* pbDown = new QPushButton( buttons, "down" );
  pbDown->setPixmap( BarIcon( "lowerlayer" ) );
  connect( pbDown, SIGNAL( clicked() ), layerview, SLOT( slotLowerLayer() ) );

  // Matthias: Never use fixed sizes with kfloatingdialog.
  //setMinimumSize( sizeHint() );
}

ChannelTab::ChannelTab( KisDoc *_doc, QWidget *_parent, const char *_name , WFlags _flags )
  : QWidget( _parent, _name, _flags )
{
  QVBoxLayout *layout = new QVBoxLayout( this );

  ChannelView* channelview = new ChannelView( _doc, this, "channellist" );
  layout->addWidget( channelview );

  QHBoxLayout *buttonlayout = new QHBoxLayout( layout );

  QPushButton* pbAddLayer = new QPushButton( this, "addchannel" );
  pbAddLayer->setPixmap( BarIcon( "newlayer" ) );
  buttonlayout->addWidget( pbAddLayer );

  QPushButton* pbRemoveLayer = new QPushButton( this, "removechannel" );
  pbRemoveLayer->setPixmap( BarIcon( "deletelayer" ) );
  buttonlayout->addWidget( pbRemoveLayer );

  QPushButton* pbUp = new QPushButton( this, "raise" );
  pbUp->setPixmap( BarIcon( "raiselayer" ) );
  buttonlayout->addWidget( pbUp );
  connect( pbUp, SIGNAL( clicked() ), channelview, SLOT( slotRaiseChannel() ) );

  QPushButton* pbDown = new QPushButton( this, "lower" );
  pbDown->setPixmap( BarIcon( "lowerlayer" ) );
  buttonlayout->addWidget( pbDown );
  connect( pbDown, SIGNAL( clicked() ), channelview, SLOT( slotLowerChannel() ) );
}

LayerDialog::LayerDialog( KisDoc *_doc, QWidget *_parent )
  : KFloatingTabDialog( _parent, "layerdialog" )
{
  setCaption( i18n( "Layers&Channels" ) );
  if( !m_eyeIcon )
  {
    QString _icon = locate( "kis_pics", "eye.png", KisFactory::global() );
    m_eyeIcon = new QPixmap;
    if( !m_eyeIcon->load( _icon ) )
      QMessageBox::critical( this, "Canvas", "Can't find eye.png" );
    m_eyeRect = QRect( QPoint( 2,( CELLHEIGHT - m_eyeIcon->height() ) / 2 ), m_eyeIcon->size() );
  }
  if( !m_linkIcon )
  {
    QString _icon = locate( "kis_pics", "link.png", KisFactory::global() );
    m_linkIcon = new QPixmap;
    if( !m_linkIcon->load( _icon ) )
      QMessageBox::critical( this, "Canvas", "Can't find link.png" );
    m_linkRect = QRect( QPoint( 25,( CELLHEIGHT - m_linkIcon->height() ) / 2 ), m_linkIcon->size() );
  }

  // HACK - the size of the preview image should be configurable somewhere
  m_previewRect = QRect( QPoint( 40, (CELLHEIGHT - m_linkIcon->height() ) /2 ),
			 QSize( 15, 15 ) );
  
  m_pLayerTab = new LayerTab( _doc, this, "layertab" );
  m_pChannelTab =  new ChannelTab(_doc, this, "channeltab" );

  addTab(m_pLayerTab, i18n("Layers"));
  addTab(m_pChannelTab, i18n("Channels"));

  QObject::connect( this, SIGNAL( tabSelected( int ) ), this, SLOT( slotTabSelected( int ) ) );
}

void LayerDialog::slotTabSelected( int _tab )
{
  if( _tab == 1 )
  {
    cout << "ChannelView-Tab selected" << endl;
  }
}

#include "kis_dlg_layer.moc"
