/* This file is part of the KDE libraries
    Copyright (C) 1998 Torben Weis <weis@kde.org>

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

#include "koPartSelectDia_data.h"

#include <qlabel.h>
#include <klocale.h>
#include <kapp.h>

DlgPartSelectData::DlgPartSelectData( QWidget* parent, const char* name ) : QDialog( parent, name, TRUE )
{
  objects = new QListBox( this, "ListBox_1" );
  objects->setGeometry( 10, 50, 310, 190 );
  objects->setFrameStyle( 51 );
  objects->setLineWidth( 2 );
  
  QLabel* tmpQLabel;
  tmpQLabel = new QLabel( this, "Label_1" );
  tmpQLabel->setGeometry( 10, 10, 100, 30 );
  tmpQLabel->setText( i18n( "Select an implementation" ) );
  tmpQLabel->setAlignment( 289 );
  tmpQLabel->setMargin( -1 );

  ok = new QPushButton( this, "PushButton_1" );
  ok->setGeometry( 10, 260, 100, 30 );
  ok->setText( i18n( "OK" ) );
  ok->setAutoRepeat( FALSE );
  ok->setAutoResize( FALSE );
  
  cancel = new QPushButton( this, "PushButton_2" );
  cancel->setGeometry( 120, 260, 100, 30 );
  cancel->setText( i18n( "Cancel" ) );
  cancel->setAutoRepeat( FALSE );
  cancel->setAutoResize( FALSE );
  
  resize( 330, 300 );
}


DlgPartSelectData::~DlgPartSelectData()
{
}

#include "koPartSelectDia_data.moc"
