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
#include <qlist.h>

#include "resizewidget.h"
#include "form.h"
#include "formregion.h"
#include "kformeditor_doc.h"

// only for debugging
#include <iostream.h>

Form::Form( KformEditorDoc* _pDoc, QWidget* _parent )
  : QWidget( _parent ), m_pDoc( _pDoc )
{
  resize( m_pDoc->getFormWidth(), m_pDoc->getFormHeight() );

  // TODO: Insert all (not one) form regions here.

  m_layout = new QGridLayout( this, 1, 1 );

  addRegion( new FormRegion( "Formularregion 1", this ) );
  addRegion( new FormRegion( "Formularregion 2", this ) );
}

Form::~Form()
{
}

void Form::slotResizing( const QRect& _rect )
{
  if( _rect.height() >= sizeHint().height() )
    resize( _rect.width(), _rect.height() );
}

void Form::addRegion( FormRegion* _region )
{
  m_lstRegions.append( _region );
  m_layout->addWidget( _region, m_lstRegions.count() - 1, 0 );

  for( int i = 0; i < m_lstRegions.count() - 1; i++ )
  {
    m_layout->setRowStretch( i, 0 );
    m_layout->addRowSpacing( i, 50 );

    // TODO: fix spacing
  }
  m_layout->setRowStretch( m_lstRegions.count() - 1, 1 );
  m_layout->addRowSpacing( m_lstRegions.count() - 1, 1 );
}

#include "form.moc"

