/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 Laurent Montel <montel@kde.org>

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

#include <qcheckbox.h>
#include <qcombobox.h>
#include <q3listview.h>
#include <QVector>

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

#include "kspread_sheet.h"
#include "kspread_view.h"
#include "kspread_doc.h"
#include "kspread_util.h"

#include "kspread_dlg_subtotal.h"

using namespace KSpread;

SubtotalDialog::SubtotalDialog( View * parent, QRect const & selection, const char * name )
  : KDialogBase(parent, name, true, i18n( "Subtotals" ), Ok | Cancel | User1, Ok, true, KGuiItem(i18n( "Remove All" )) ),
    m_pView( parent ),
    m_pSheet( m_pView->activeSheet() ),
    m_selection( selection ),
    m_widget( new SubtotalWidget( this ) )
{
  setButtonBoxOrientation( Qt::Vertical );
  setMainWidget( m_widget );

  fillColumnBoxes();
  fillFunctionBox();
}

SubtotalDialog::~SubtotalDialog()
{
}

void SubtotalDialog::slotOk()
{
  int numOfCols = m_selection.width();
  QVector<int> columns( numOfCols );

  int n = 0;
  bool empty = true;
  int left = m_selection.left();
  for ( Q3ListViewItem * item = m_widget->m_columnList->firstChild(); item; item = item->nextSibling() )
  {
    if ( ((Q3CheckListItem * ) item)->isOn() )
    {
      columns[n] = left + n;
      empty = false;
    }
    else
      columns[n] = -1;
    ++n;
  }

  if ( empty )
  {
    KMessageBox::sorry( this, i18n("You need to select at least one column for adding subtotals.") );
    return;
  }

  if ( m_widget->m_replaceSubtotals->isChecked() )
    removeSubtotalLines();

  int mainCol = left + m_widget->m_columnBox->currentItem();
  int bottom = m_selection.bottom();
  int top    = m_selection.top();
  left       = m_selection.left();
  QString oldText = m_pSheet->cellAt( mainCol, top )->strOutText();
  QString newText;
  QString result( " " + i18n("Result") );
  int lastChangedRow = top;

  m_pView->doc()->emitBeginOperation( false );
  bool ignoreEmptyCells = m_widget->m_IgnoreBox->isChecked();
  bool addRow;
  if ( !m_widget->m_summaryOnly->isChecked() )
  {
    int y = top + 1;
    kDebug() << "Starting in row " << y << endl;
    while ( y <= bottom )
    {
      addRow = true;
      newText = m_pSheet->cellAt( mainCol, y )->strOutText();

      if ( ignoreEmptyCells && (newText.length() == 0) )
      {
        ++y;
        kDebug() << "Still the same -> " << y << endl;
        continue;
      }

      if (newText != oldText)
      {
        int saveY = y;
        for (int x = 0; x < numOfCols; ++x)
        {
          kDebug() << "Column: " << x << ", " << columns[x] << endl;
          if (columns[x] != -1)
          {
            if (!addSubtotal( mainCol, columns[x], y - 1, lastChangedRow, addRow, oldText + result))
              reject();

            if ( addRow )
            {
              ++saveY;
              ++bottom;
            }

            addRow = false;
          }
        }
        y = saveY;
        lastChangedRow = y;
      }
      oldText = newText;
      ++y;
    }

    addRow = true;
    for ( int x = 0; x < numOfCols; ++x )
    {
      if ( columns[x] != -1 )
      {
        if ( !addSubtotal( mainCol, columns[x], y - 1, lastChangedRow, addRow, oldText + result ) )
          reject();
        addRow = false;
      }
    }
    ++y;
  }

  if ( m_widget->m_summaryBelow->isChecked() )
  {
    addRow = true;
    int bottom = m_selection.bottom();
    for (int x = 0; x < numOfCols; ++x)
    {
      if (columns[x] != -1)
      {
        addSubtotal( mainCol, columns[x], bottom, top, addRow, i18n("Grand Total") );
        addRow = false;
      }
    }
  }

  m_pView->slotUpdateView( m_pView->activeSheet() );
  accept();
}

void SubtotalDialog::slotCancel()
{
  reject();
}

void SubtotalDialog::slotUser1()
{
  m_pView->doc()->emitBeginOperation( false );
  removeSubtotalLines();
  m_pView->slotUpdateView( m_pView->activeSheet() );
  accept();
}

void SubtotalDialog::removeSubtotalLines()
{
  kDebug() << "Removing subtotal lines" << endl;

  int r = m_selection.right();
  int l = m_selection.left();
  int t = m_selection.top();

  Cell * cell;
  QString text;

  for ( int y = m_selection.bottom(); y >= t; --y )
  {
    kDebug() << "Checking row: " << y << endl;
    bool containsSubtotal = false;
    for (int x = l; x <= r; ++x )
    {
      cell = m_pSheet->cellAt( x, y );
      if ( cell->isDefault() || !cell->isFormula() )
        continue;

      text = cell->text();
      if ( text.indexOf( "SUBTOTAL" ) != -1 )
      {
        containsSubtotal = true;
        break;
      }
    }

    if ( containsSubtotal )
    {
      kDebug() << "Line " << y << " contains a subtotal " << endl;
      QRect rect( l, y, m_selection.width(), 1 );

      m_pSheet->unshiftColumn( rect );
      m_selection.setHeight( m_selection.height() - 1 );
    }
  }
  kDebug() << "Done removing subtotals" << endl;
}

void SubtotalDialog::fillColumnBoxes()
{
  int r = m_selection.right();
  int row = m_selection.top();

  Cell    * cell;
  Q3CheckListItem * item;

  QString text;
  QString col( i18n( "Column '%1' ") );

  for ( int i = m_selection.left(); i <= r; ++i )
  {
    cell = m_pSheet->cellAt( i, row );
    text = cell->strOutText();

    if ( text.length() > 0 )
    {
      text = col.arg( Cell::columnName( i ) );
    }

    m_widget->m_columnBox->insertItem( text );

    item = new Q3CheckListItem( m_widget->m_columnList,
                               text,
                               Q3CheckListItem::CheckBox );
    item->setOn(false);
    m_widget->m_columnList->insertItem( item );
  }
}

void SubtotalDialog::fillFunctionBox()
{
    QStringList lst;
    lst << i18n( "Average" );
    lst << i18n( "Count" );
    lst << i18n( "CountA" );
    lst << i18n( "Max" );
    lst << i18n( "Min" );
    lst << i18n( "Product" );
    lst << i18n( "StDev" );
    lst << i18n( "StDevP" );
    lst << i18n( "Sum" );
    lst << i18n( "Var" );
    lst << i18n( "VarP" );
    m_widget->m_functionBox->insertStringList(lst);
}

bool SubtotalDialog::addSubtotal( int mainCol, int column, int row, int topRow,
                                      bool addRow, QString const & text )
{
  kDebug() << "Adding subtotal: " << mainCol << ", " << column << ", Rows: " << row << ", " << topRow
            << ": addRow: " << addRow << ", Text: " << text << endl;
  if ( addRow )
  {
    QRect rect(m_selection.left(), row + 1, m_selection.width(), 1);
    if ( !m_pSheet->shiftColumn( rect ) )
      return false;

    m_selection.setHeight( m_selection.height() + 1 );

    Cell * cell = m_pSheet->nonDefaultCell( mainCol, row + 1 );
    cell->setCellText( text );
    cell->format()->setTextFontBold( true );
    cell->format()->setTextFontItalic( true );
    cell->format()->setTextFontUnderline( true );
  }

  QString colName = Cell::columnName( column );

  QString formula("=SUBTOTAL(");
  formula += QString::number( m_widget->m_functionBox->currentItem() + 1 );
  formula += "; ";
  formula += colName;
  formula += QString::number( topRow );
  // if ( topRow != row )
  {
    formula += ":";
    formula += colName;
    formula += QString::number( row );
  }
  formula += ")";

  Cell * cell = m_pSheet->nonDefaultCell( column, row + 1 );
  cell->setCellText( formula );
  cell->format()->setTextFontBold( true );
  cell->format()->setTextFontItalic( true );
  cell->format()->setTextFontUnderline( true );

  return true;
}

#include "kspread_dlg_subtotal.moc"

