/* This file is part of the KDE project
   Copyright (C) 1999 David Faure <faure@kde.org>

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

#include <csvexportdialog.h>
#include <exportdialogui.h>

#include <kspread_map.h>
#include <kspread_table.h>

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qcursor.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qptrlist.h> 
#include <qradiobutton.h>
#include <qtextstream.h>

#include <kapplication.h>
#include <klocale.h>

CSVExportDialog::CSVExportDialog( QWidget * parent )
  : KDialogBase( parent, 0, true, QString::null, Ok | Cancel, No, true ),
    m_dialog( new ExportDialogUI( this ) ),
    m_delimiter( "," ),
    m_textquote('"')
{
  kapp->restoreOverrideCursor();

  setButtonBoxOrientation ( Vertical );

  setMainWidget(m_dialog);

  connect( m_dialog->m_delimiterBox, SIGNAL( clicked(int) ),
           this, SLOT( delimiterClicked( int ) ) );
  connect( m_dialog->m_delimiterEdit, SIGNAL( returnPressed() ),
           this, SLOT( returnPressed() ) );
  connect( m_dialog->m_delimiterEdit, SIGNAL( textChanged ( const QString & ) ),
           this, SLOT(textChanged ( const QString & ) ) );
  connect( m_dialog->m_comboQuote, SIGNAL( activated( const QString & ) ),
           this, SLOT( textquoteSelected( const QString & ) ) );
}

CSVExportDialog::~CSVExportDialog()
{
  kapp->setOverrideCursor(Qt::waitCursor);
}

void CSVExportDialog::fillTable( KSpreadMap * map )
{
  m_dialog->m_tableList->clear();
  QCheckListItem * item;

  QPtrListIterator<KSpreadTable> it( map->tableList() );
  for( ; it.current(); ++it )
  {
    item = new QCheckListItem( m_dialog->m_tableList, 
                               it.current()->tableName(),
                               QCheckListItem::CheckBox );
    item->setOn(false);
    m_dialog->m_tableList->insertItem( item );
  }

  m_dialog->m_tableList->setSorting(1, true);
  m_dialog->m_tableList->sort();
  m_dialog->m_tableList->setSorting( -1 );  
}

QChar CSVExportDialog::getDelimiter() const
{
  return m_delimiter[0];
}

QChar CSVExportDialog::getTextQuote() const
{
  return m_textquote;
}

bool CSVExportDialog::printAlwaysTableDelimiter() const
{
  return m_dialog->m_delimiterAboveAll->isChecked();
}

QString CSVExportDialog::getTableDelimiter() const
{
  return m_dialog->m_tableDelimiter->text();
}

bool CSVExportDialog::exportTable(QString const & tableName) const
{
  for (QListViewItem * item = m_dialog->m_tableList->firstChild(); item; item = item->nextSibling())
  {
    if (((QCheckListItem * ) item)->isOn())
    {
      if ( ((QCheckListItem * ) item)->text() == tableName )
        return true;
    }
  }  
  return false;
}

void CSVExportDialog::slotOk()
{
  accept();
}

void CSVExportDialog::slotCancel()
{
  reject();
}

void CSVExportDialog::returnPressed()
{
  if ( m_dialog->m_delimiterBox->id( m_dialog->m_delimiterBox->selected() ) != 4 )
    return;

  m_delimiter = m_dialog->m_delimiterEdit->text();
}

void CSVExportDialog::textChanged ( const QString & )
{
  m_dialog->m_radioOther->setChecked ( true );
  delimiterClicked(4); // other
}

void CSVExportDialog::delimiterClicked( int id )
{
  switch (id)
  {
    case 0: // comma
     m_delimiter = ",";
     break;
   case 1: // semicolon
    m_delimiter = ";";
    break;
   case 2: // tab
    m_delimiter = "\t";
    break;
   case 3: // space
    m_delimiter = " ";
    break;
   case 4: // other
    m_delimiter = m_dialog->m_delimiterEdit->text();
    break;
  }
}

void CSVExportDialog::textquoteSelected( const QString & mark )
{
  m_textquote = mark[0];
}

#include "csvexportdialog.moc"

