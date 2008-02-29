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
   Boston, MA 02110-1301, USA.
*/

// Local
#include "CSVDialog.h"

#include <QApplication>
#include <QByteArray>
#include <QMimeData>
#include <QString>
#include <QTimer>

#include <kdebug.h>
#include <kdialog.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>

#include "Cell.h"
#include "CellStorage.h"
#include "Doc.h"
#include "Sheet.h"
#include "View.h"

#include "commands/CSVDataCommand.h"

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
  Cell lastCell = m_pView->activeSheet()->cellStorage()->lastInColumn( column );
  if ( !lastCell.isNull() )
    if( m_targetRect.bottom() > lastCell.row() )
      m_targetRect.setBottom( lastCell.row() );

  if ( m_mode == Clipboard )
  {
    setWindowTitle( i18n( "Inserting From Clipboard" ) );
    const QMimeData* mime = QApplication::clipboard()->mimeData();
    if ( !mime )
    {
      KMessageBox::information( this, i18n("There is no data in the clipboard.") );
      m_canceled = true;
    }
    if ( !mime->hasText() )
    {
      KMessageBox::information( this, i18n("There is no usable data in the clipboard.") );
      m_canceled = true;
    }
  }
  else if ( m_mode == File )
  {
    setWindowTitle( i18n( "Inserting Text File" ) );
    m_filename = KFileDialog::getOpenFileName(KUrl("kfiledialog:///"),
                                                "text/plain",
                                                this);
    //cancel action !
    if ( m_filename.isEmpty() )
    {
        enableButton( Ok, false );
        m_canceled = true;
    }
  }
  else // if ( m_mode == Column )
  {
    setWindowTitle( i18n( "Text to Columns" ) );
    setDataWidgetEnabled(false);
  }

  if ( ! m_canceled )
    QTimer::singleShot(0, this, SLOT(init()));
}

CSVDialog::~CSVDialog()
{
  // no need to delete child widgets, Qt does it all for us
}

void CSVDialog::init()
{
  if ( m_canceled )
    return;

  if ( m_mode == Clipboard )
  {
    const QMimeData* mime = QApplication::clipboard()->mimeData();
    Q_ASSERT( mime );
    Q_ASSERT( mime->hasText() );
    setData(QByteArray(mime->text().toUtf8()));
  }
  else if ( m_mode == File )
  {
    Q_ASSERT( ! m_filename.isEmpty() );
    QFile in(m_filename);
    if (!in.open(QIODevice::ReadOnly))
    {
      KMessageBox::sorry( this, i18n("Cannot open input file.") );
      in.close();
      enableButton( Ok, false );
      m_canceled = true;
      return;
    }
    setData(in.readAll());
    in.close();
  }
  else // if ( m_mode == Column )
  {
    setData(QByteArray());
    Cell cell;
    Sheet * sheet = m_pView->activeSheet();
    QByteArray data;
    int col = m_targetRect.left();
    for (int i = m_targetRect.top(); i <= m_targetRect.bottom(); ++i)
    {
      cell = Cell( sheet, col, i );
      if ( !cell.isEmpty() )
      {
        data.append( cell.displayText().toUtf8() /* FIXME */ );
      }
      data.append( '\n' );
    }
    setData(data);
  }
}

bool CSVDialog::canceled()
{
  return m_canceled;
}

void CSVDialog::accept()
{
  Sheet * sheet  = m_pView->activeSheet();

  int numRows = rows();
  int numCols = cols();

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

    QList<KoCsvImportDialog::DataType> dataTypes;
    Value value(Value::Array);
    for (int row = 0; row < numRows; ++row)
    {
        for (int col = 0; col < numCols; ++col)
        {
            value.setElement(col, row, Value(text(row, col)));
            if (row == 0)
                dataTypes.insert(col, dataType(col));
        }
    }

    CSVDataCommand* command = new CSVDataCommand();
    if (m_mode == Clipboard)
        command->setText(i18n("Inserting From Clipboard"));
    else if (m_mode == File)
        command->setText(i18n("Inserting Text File"));
    else
        command->setText(i18n("Text to Columns"));
    command->setSheet(sheet);
    command->setValue(value);
    command->setColumnDataTypes(dataTypes);
    command->setDecimalSymbol(decimalSymbol());
    command->setThousandsSeparator(thousandsSeparator());
    command->add(m_targetRect);
    if (!command->execute())
        delete command;

  m_pView->slotUpdateView( sheet );
  KoCsvImportDialog::accept();
}

#include "CSVDialog.moc"
