/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002-2003 Ariya Hidayat <ariya@kde.org>
             (C) 2002      Laurent Montel <montel@kde.org>
             (C) 1999 David Faure <faure@kde.org>

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

#include <QByteArray>
#include <QMimeData>
#include <QString>

#include <kapplication.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>

#include <Cell.h>
#include <DataManipulators.h>
#include <Doc.h>
#include <Sheet.h>
#include <Format.h>
#include <View.h>

#include "CSVDialog.h"

using namespace KSpread;

CSVDialog::CSVDialog( View * parent, QRect const & rect, Mode mode)
  : KoCsvImportDialog( parent ),
    m_pView( parent ),
    m_canceled( false ),
    m_targetRect( rect ),
    m_mode( mode )
{
  // Limit the range
  int column = m_targetRect.left();
  Cell* lastCell = m_pView->activeSheet()->getLastCellColumn( column );
  if( lastCell )
    if( m_targetRect.bottom() > lastCell->row() )
      m_targetRect.setBottom( lastCell->row() );

  if ( m_mode == Clipboard )
  {
    setWindowTitle( i18n( "Inserting From Clipboard" ) );
    const QMimeData* mime = QApplication::clipboard()->mimeData();
    if ( !mime )
    {
      KMessageBox::information( this, i18n("There is no data in the clipboard.") );
      m_canceled = true;
      return;
    }

    if ( !mime->hasText() )
    {
      KMessageBox::information( this, i18n("There is no usable data in the clipboard.") );
      m_canceled = true;
      return;
    }
    m_fileArray = QByteArray( mime->text().toUtf8() );
  }
  else if ( mode == File )
  {
    setWindowTitle( i18n( "Inserting Text File" ) );
    QString file = KFileDialog::getOpenFileName(KUrl("kfiledialog:///"),
                                                "text/plain",
                                                this);
    //cancel action !
    if ( file.isEmpty() )
    {
        enableButton( Ok, false );
        m_canceled = true;
        return;
    }
    QFile in(file);
    if (!in.open(QIODevice::ReadOnly))
    {
      KMessageBox::sorry( this, i18n("Cannot open input file.") );
      in.close();
      enableButton( Ok, false );
      m_canceled = true;
      return;
    }
    m_fileArray = QByteArray(/*in.size()*/);
    in.read(m_fileArray.data(), in.size());
    in.close();
  }
  else
  {
    setWindowTitle( i18n( "Text to Columns" ) );
    m_dialog->tabWidget2->setTabEnabled(0, false);
    m_fileArray.clear();
    Cell  * cell;
    Sheet * sheet = m_pView->activeSheet();
    int col = m_targetRect.left();
    for (int i = m_targetRect.top(); i <= m_targetRect.bottom(); ++i)
    {
      cell = sheet->cellAt( col, i );
      if ( !cell->isEmpty() && !cell->isDefault() )
      {
        m_fileArray.append( cell->strOutText().toUtf8() /* FIXME */ );
      }
      m_fileArray.append( '\n' );
    }
  }
  fillTable();
}

CSVDialog::~CSVDialog()
{
  // no need to delete child widgets, Qt does it all for us
}

bool CSVDialog::canceled()
{
  return m_canceled;
}

void CSVDialog::accept()
{
  Sheet * sheet  = m_pView->activeSheet();

  int numRows = m_dialog->m_sheet->numRows();
  int numCols = m_dialog->m_sheet->numCols();

  if ((numRows == 0) || (numCols == 0))
    return;  // nothing to do here

  if ( (numCols > m_targetRect.width()) && (m_targetRect.width() > 1) )
  {
    numCols = m_targetRect.width();
  }
  else
    m_targetRect.setRight( m_targetRect.left() + numCols - 1 );

  if ( (numRows > m_targetRect.height()) && (m_targetRect.height() > 1) )
    numRows = m_targetRect.height();
  else
    m_targetRect.setBottom( m_targetRect.top() + numRows - 1 );

  Value val (numCols, numRows);
  for (int row = 0; row < numRows; ++row)
    for (int col = 0; col < numCols; ++col)
      val.setElement (col, row, getText (row, col));

  DataManipulator *manipulator = new DataManipulator;
  manipulator->setName (i18n ("Text to Columns"));
  manipulator->setSheet (sheet);
  manipulator->setParsing (true);
  manipulator->setFormat (Generic_format);
  manipulator->setValue (val);
  manipulator->add (m_targetRect);
  manipulator->execute ();

  m_pView->slotUpdateView( sheet );
  QDialog::accept();
}

QString CSVDialog::getText(int row, int col)
{
  return m_dialog->m_sheet->text(row, col);
}

#include "CSVDialog.moc"
