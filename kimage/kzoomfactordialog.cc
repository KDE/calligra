/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#include "kzoomfactordialog.h"

#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include <klocale.h>

/*****************************************************************************
 *
 * KZoomFactorDialog
 *
 *****************************************************************************/

KZoomFactorDialog::KZoomFactorDialog( QWidget* parent, const char* name, WFlags f )
  : KDialog( parent, name, TRUE, f )
{
  // Layout
  QGridLayout* grid = new QGridLayout( this, 6, 4, 15, 7 );

  // Inputline X
  m_pLineEditXFactor = new KIntLineEdit( this, i18n( "inputZoomX" ) );
  grid->addMultiCellWidget( m_pLineEditXFactor, 1, 1, 0, 3 );
  QLabel* label = new QLabel( m_pLineEditXFactor, i18n( "Zoom factor for width in % (1x = 100)" ), this );
  grid->addWidget( label, 0, 0 );

  // Inputline Y
  m_pLineEditYFactor = new KIntLineEdit( this, i18n( "inputZoomX" ) );
  grid->addMultiCellWidget( m_pLineEditYFactor, 3, 3, 0, 3 );
  label = new QLabel( m_pLineEditYFactor, i18n( "Zoom factor for height in % (1x = 100)" ), this );
  grid->addWidget( label, 2, 0 );

  // OK-Button
  QPushButton* buttonOK = new QPushButton( this, "ButtonOK" );
  buttonOK->setText( i18n( "OK" ) );
  connect( buttonOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
  grid->addWidget( buttonOK, 5, 2 );
	
  // Cancel-Button
  QPushButton* buttonCancel = new QPushButton( this, "ButtonCancel" );
  buttonCancel->setText( i18n( "Cancel" ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  grid->addWidget( buttonCancel, 5, 3 ); 

   // Dialog
   grid->setRowStretch( 4, 1 );
   grid->setColStretch( 1, 1 );

   setMinimumSize( label->sizeHint().width() + m_pLineEditXFactor->sizeHint().width() + buttonOK->sizeHint().width() + 30, 
                   label->sizeHint().height() + buttonOK->sizeHint().height() + buttonCancel->sizeHint().height() + 40);
}

KZoomFactorDialog::~KZoomFactorDialog()
{
}

void KZoomFactorDialog::setValue( const QPoint& theValue )
{
  m_pLineEditXFactor->setValue( theValue.x() );
  m_pLineEditYFactor->setValue( theValue.y() );
}

int KZoomFactorDialog::getValue( QPoint& theValue)
{
  int result;

  setValue( theValue );
  result = exec();

  if( result == Accepted )
    theValue = QPoint( m_pLineEditXFactor->value(), m_pLineEditYFactor->value() );

  return result;
}

#include "kzoomfactordialog.moc"
