/*
*
* Copyright (C) 2005  Fredrik Edemar
*                     f_edemar@linux.se
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*
*/


// Local
#include "KCPrinterDialog.h"

// Qt includes
#include <QLayout>
#include <QLabel>

// KDE includes
#include <kdebug.h>
#include <kdialog.h>
#include <klocale.h>


namespace KChart
{

KCPrinterDialog::KCPrinterDialog( QWidget *parent, const char *name )
  : QWidget( parent )
{
  Q_UNUSED( name );

  setWindowTitle( i18n( "KChart Options" ) );
  QVBoxLayout *layout = new QVBoxLayout( this );
  layout->setMargin( KDialog::marginHint() );
  layout->setSpacing( KDialog::spacingHint() );

  txtSizex = new KIntNumInput(this );
  txtSizex->setSuffix("%");
  txtSizex->setMinimum(1);
  txtSizex->setMaximum(100);
  txtSizex->setValue(100);

  txtSizey = new KIntNumInput(this );
  txtSizey->setSuffix("%");
  txtSizey->setMinimum(1);
  txtSizey->setMaximum(100);
  txtSizey->setValue(100);
  
  layout->addWidget( new QLabel(i18n("Print Size"), this) );
  layout->addWidget( new QLabel(i18n("Width: "), this) );
  layout->addWidget( txtSizex );
  layout->addWidget( new QLabel(i18n("Height: "), this) );
  layout->addWidget( txtSizey );
  layout->addStretch( 1 );
}

int KCPrinterDialog::printSizeX()
{
  return txtSizex->value();
}

void KCPrinterDialog::setPrintSizeX( int sizeX )
{
  txtSizex->setValue( sizeX );
}

int KCPrinterDialog::printSizeY()
{
  return txtSizey->value();
}

void KCPrinterDialog::setPrintSizeY( int sizeY )
{
  txtSizey->setValue( sizeY );
}

}  //namespace KChart
#include "KCPrinterDialog.moc"
