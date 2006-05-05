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

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

#include <kspread_map.h>

#include "kwmailmerge_kspread.h"
#include "kwmailmerge_kspread_config.h"

using namespace KSpread;

KWMailMergeKSpread::KWMailMergeKSpread( KInstance *instance, QObject *parent )
  : KWMailMergeDataSource( instance, parent ), _spreadSheetNumber( 1 )
{
}

KWMailMergeKSpread::~KWMailMergeKSpread()
{
}

int KWMailMergeKSpread::getNumRecords() const
{
  return rows() - 2;
}

QString KWMailMergeKSpread::getValue( const QString &name, int record ) const
{
  if ( record < 0 )
    return name;

  const Cell* cell = _sheet->cellAt( _columnMap[ name ], record + 2 );

  if ( cell )
    return cellText( cell );
  else
    return i18n( "Unkown mail merge variable: %1" , name );
}

void KWMailMergeKSpread::load( QDomElement& parentElem )
{
  QDomNode contentNode = parentElem.namedItem( "CONTENT" );
  if ( contentNode.isNull() )
    return;

  QDomElement element = contentNode.toElement();
  if ( element.isNull() )
    return;

  _url = element.attribute( QString::fromLatin1( "URL" ) );
  _spreadSheetNumber = element.attribute( QString::fromLatin1( "SpreadSheetNumber" ) ).toInt();

  initDocument();
}

void KWMailMergeKSpread::save( QDomDocument& doc, QDomElement& parent )
{
  QDomElement content = doc.createElement( QString::fromLatin1( "CONTENT" ) );
  parent.appendChild( content );

  content.setAttribute( "URL", _url.url() );
  content.setAttribute( "SpreadSheetNumber", _spreadSheetNumber );
}

void KWMailMergeKSpread::refresh( bool )
{
}

bool KWMailMergeKSpread::showConfigDialog( QWidget *parent, int )
{
  KWMailMergeKSpreadConfig dlg( parent, this );

  int retval = dlg.exec();
  if ( retval )
    initDocument();

  return retval;
}

void KWMailMergeKSpread::initDocument()
{
  _document = new Doc();

  connect( _document, SIGNAL( completed() ), SLOT( initSpreadSheets() ) );

  _document->openURL( _url );
}

void KWMailMergeKSpread::initSpreadSheets()
{

  _columnMap.clear();
  sampleRecord.clear();

  QListIterator<Sheet*> it( _document->map()->sheetList() );
  int counter = 0;
  it.toFront();
  while( counter < _spreadSheetNumber )
  {
    _sheet = it.next();
    counter++;
  }

  if ( !_sheet ) {
    kError() << "No spread sheet available" << endl;
    return;
  }

  if ( rows() < 2 ) // empty table
    return;

  int cols = columns();
  for ( int i = 1; i < cols; ++i ) {
    const Cell* cell = _sheet->cellAt( i, 1 );

    // init record list
    sampleRecord[ cellText( cell ) ] = cellText( cell );
    _columnMap.insert( cellText( cell ), i );
  }
}

int KWMailMergeKSpread::rows() const
{
  if ( !_sheet )
    return 0;

  int row = 1;

  for (; row < _sheet->maxRow(); ) {
    const Cell* cell = _sheet->cellAt( 1, row );
    if ( cellText( cell ).isEmpty() )
      break;

    row++;
  }

  return row;
}

int KWMailMergeKSpread::columns() const
{
  if ( !_sheet )
    return 0;

  int col = 1;

  for (; col < _sheet->maxColumn(); ) {
    const Cell* cell = _sheet->cellAt( col, 1 );
    if ( cellText( cell ).isEmpty() )
      break;

    col++;
  }

  return col;
}

QString KWMailMergeKSpread::cellText( const Cell *cell ) const
{
  QString text = QString::null;

  if ( !cell->isDefault() && !cell->isEmpty() ) {
      if ( cell->isFormula() )
          text = cell->strOutText();
      else if ( !cell->link().isEmpty() )
          text = cell->link();
      else
          text = cell->text();
  }
#if 0
      switch( cell->content() ) {
     case Cell::Text:
     case Cell::Formula:
      text = cell->strOutText();
      break;
     case Cell::RichText:
     case Cell::VisualFormula:
      text = cell->text(); // untested
      break;
    }
  }
#endif
  return text;
}

extern "C"
{
  KWORD_MAILMERGE_EXPORT KWMailMergeDataSource *create_kwmailmerge_kspread( KInstance *instance, QObject *parent )
  {
    return new KWMailMergeKSpread( instance, parent );
  }
}

#include "kwmailmerge_kspread.moc"
