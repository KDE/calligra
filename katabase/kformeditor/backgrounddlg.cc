/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Michael Koch <koch@kde.org>
 
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
#include <qpushbutton.h>

#include <kcolordlg.h>

#include "backgrounddlg.h"

// only for debug
#include <iostream.h>

BackgroundDlg::BackgroundDlg( const QColor& _color, QWidget* _parent, const char* _name )
  : QDialog( _parent, _name, TRUE )
{
  QGridLayout* grid = new QGridLayout( this, 3, 2, 15, 7 );

  m_colorField = new QWidget( this );
  m_colorField->setBackgroundColor( _color );
  m_colorField->setMinimumSize( 100, 30 );
  m_colorField->show();
  grid->addMultiCellWidget( m_colorField, 0, 0, 0, 1 );
  
  QPushButton* colorButton = new QPushButton( this );
  colorButton->setText( "Choose color" );
  colorButton->resize( 100, 30 );
  connect( colorButton, SIGNAL( clicked() ), this, SLOT( slotChooseColor() ) );
  grid->addMultiCellWidget( colorButton, 1, 1, 0, 1 );

  QPushButton* okButton = new QPushButton( this );
  okButton->setText( "Ok" );
  okButton->resize( 100, 30 );
  connect( okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
  grid->addWidget( okButton, 2, 0 );

  QPushButton* cancelButton = new QPushButton( this );
  cancelButton->setText( "Cancel" );
  cancelButton->resize( 100, 30 );
  connect( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
  grid->addWidget( cancelButton, 2, 1 );
}

BackgroundDlg::~BackgroundDlg()
{
}

void BackgroundDlg::setColor( const QColor& _color )
{
  m_colorField->setBackgroundColor( _color );
}

QColor BackgroundDlg::color()
{
  return m_colorField->backgroundColor();
}

void BackgroundDlg::slotChooseColor()
{
  QColor color = m_colorField->backgroundColor();
  int result = KColorDialog::getColor( color );

  if( result == QDialog::Accepted )
    m_colorField->setBackgroundColor( color );
}

#include "backgrounddlg.moc"

