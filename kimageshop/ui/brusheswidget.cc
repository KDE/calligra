/*
 *  brusheswidget.cc - part of KImageShop
 *
 *  A chooser for Brushes. Makes use of the IconChooser class and maintains
 *  all available brushes for KIS.
 *
 *  Copyright (c) 1999 Carsten Pfeiffer <pfeiffer@kde.org>
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

#include <kinstance.h>
#include <klocale.h>
#include <kstddirs.h>

#include "kis_factory.h"
#include "brusheswidget.h"
#include "iconchooser.h"
#include "integerwidget.h"


BrushesWidget::BrushesWidget( QWidget *parent, const char *name )
  : QWidget( parent, name )
{

  lbSpacing = new QLabel( i18n("Spacing:"), this );
  slSpacing = new IntegerWidget( 1, 100, this, "int widget" );

  slSpacing->setTickmarks( QSlider::Below );
  slSpacing->setTickInterval( 10 );
  QObject::connect( slSpacing, SIGNAL( valueChanged(int) ),
		    this, SLOT( slotSetBrushSpacing(int) ));


  // only serves as beautifier for the iconchooser
  frame = new QHBox( this );
  frame->setFrameStyle( QFrame::Panel | QFrame::Sunken );

  chooser = new IconChooser( frame, QSize(30,30), "icon chooser" );
  loadBrushes();
  QObject::connect( chooser, SIGNAL( selected( IconItem * ) ),
		    this, SLOT( slotItemSelected( IconItem * )));
				

  initGUI();

  const Brush *brush = currentBrush();
  if ( brush )
    slSpacing->setValue( brush->spacing() );
}


BrushesWidget::~BrushesWidget()
{
  delete lbSpacing;
  delete slSpacing;
  delete chooser;
  delete frame;
}


// load all brushes from all available brushes locations
void BrushesWidget::loadBrushes()
{
  // FIXME this needs to be changed in the near future
  // (brushes will be saved differently)

  QStringList list;
  list = KImageShopFactory::global()->dirs()->findAllResources("kis_brushes", "*.png", false, true);
  QString file;
  
  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
    {
      file = *it;
      (void) loadBrush( file );
    }
}


// load one brush and add it to the chooser widget
// return the brush if successful, otherwise return 0L
const Brush * BrushesWidget::loadBrush( const QString& filename ) const
{
  Brush *brush = new Brush( filename );

  if ( brush->isValid() ) {
    chooser->addItem( (IconItem *) brush );
  }
  else {
    delete brush;
    brush = 0L;
  }

  return brush;
}


// set the active brush in the chooser - does NOT emit selected() (should it?)
void BrushesWidget::setCurrentBrush( const Brush *brush )
{
  chooser->setCurrentItem( (IconItem *) brush );
  slSpacing->setValue( brush->spacing() );
}


// return the active brush
const Brush * BrushesWidget::currentBrush() const
{
  return (const Brush *) chooser->currentItem();
}


void BrushesWidget::initGUI()
{
  QVBoxLayout *mainLayout = new QVBoxLayout( this, 2, -1, "main layout" );
  QHBoxLayout *spacingLayout = new QHBoxLayout( -1, "spacing layout" );

  mainLayout->addWidget( frame, 10 );
  mainLayout->addLayout( spacingLayout, 1 );

  spacingLayout->addWidget( lbSpacing, 0 );
  spacingLayout->addStretch();
  spacingLayout->addWidget( slSpacing, 1 );
}


// called when an item is selected in the chooser
// set the slider to the correct position
void BrushesWidget::slotItemSelected( IconItem *item )
{
  const Brush *brush = (Brush *) item;
  slSpacing->setValue( brush->spacing() );
  emit selected( brush );
}


// sliderposition (spacing) changed, apply that to the current brush
void BrushesWidget::slotSetBrushSpacing( int spacing )
{
  Brush *brush = (Brush *) currentBrush();
  if ( brush )
    brush->setSpacing( spacing );
  else
    debug("setting spacing, but no current brush!");
}


#include "brusheswidget.moc"



