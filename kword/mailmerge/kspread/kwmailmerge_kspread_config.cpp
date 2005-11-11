/*
   This file is part of the KDE project
   Copyright (C) 2004 Tobias Koenig <tokoe@kde.org>

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


#include <qlabel.h>
#include <qlayout.h>

#include <kcombobox.h>
#include <klineedit.h>
#include <klocale.h>
#include <kurlrequester.h>

#include <kspread_map.h>

#include "kwmailmerge_kspread.h"
#include "kwmailmerge_kspread_config.h"

using namespace KSpread;

KWMailMergeKSpreadConfig::KWMailMergeKSpreadConfig( QWidget *parent, KWMailMergeKSpread *object )
  : KDialogBase( Plain, i18n( "Mail Merge - Editor" ),
                 Ok | Cancel, Ok, parent, "", true ),
    _document( 0 ), _initialPage( 1 )
{
  _object = object;

  initGUI();

  _urlRequester->setURL( _object->url().url() );
  _initialPage = _object->spreadSheetNumber();

  connect( _urlRequester, SIGNAL( urlSelected( const QString& ) ),
           SLOT( loadDocument() ) );

  loadDocument();
  slotTextChanged( _urlRequester->lineEdit()->text() );
}


KWMailMergeKSpreadConfig::~KWMailMergeKSpreadConfig()
{
}

void KWMailMergeKSpreadConfig::slotOk()
{
  _object->setURL( _urlRequester->url() );
  _object->setSpreadSheetNumber( _pageNumber->currentText().toInt() );

  KDialogBase::slotOk();
}

void KWMailMergeKSpreadConfig::loadDocument()
{
  delete _document;
  _document = 0;

  _pageNumber->setEnabled( false );

  if ( !_urlRequester->url().isEmpty() ) {
    _document = new Doc();
    connect( _document, SIGNAL( completed() ), SLOT( documentLoaded() ) );

    _document->openURL( _urlRequester->url() );
  }
}

void KWMailMergeKSpreadConfig::documentLoaded()
{
  _pageNumber->clear();

  QPtrListIterator<Sheet> it( _document->map()->sheetList() );
  int counter = 1;
  for ( it.toFirst(); it.current(); ++it ) {
    _pageNumber->insertItem( QString::number( counter ) );
    counter++;
  }

  _pageNumber->setEnabled( true );
  _pageNumber->setCurrentText( QString::number( _initialPage ) );
}

void KWMailMergeKSpreadConfig::initGUI()
{
  QFrame *page = plainPage();

  QGridLayout *layout = new QGridLayout( page, 2, 2, marginHint(), spacingHint() );

  QLabel *label = new QLabel( i18n( "URL:" ), page );
  layout->addWidget( label, 0, 0 );

  _urlRequester = new KURLRequester( page );
  layout->addWidget( _urlRequester, 0, 1 );

  label = new QLabel( i18n( "Page number:" ), page );
  layout->addWidget( label, 1, 0 );

  _pageNumber = new KComboBox( page );
  _pageNumber->setEnabled( false );
  layout->addWidget( _pageNumber, 1, 1 );
  connect( _urlRequester->lineEdit() , SIGNAL( textChanged ( const QString & ) ), this, SLOT( slotTextChanged( const QString & ) ) );
}

void KWMailMergeKSpreadConfig::slotTextChanged( const QString & _text )
{
    enableButtonOK( !_text.isEmpty() );
}

#include "kwmailmerge_kspread_config.moc"
