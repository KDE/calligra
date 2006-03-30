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

#include <q3buttongroup.h>
#include <qcheckbox.h>
#include <qclipboard.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmime.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <q3table.h>
#include <qlayout.h>
//Added by qt3to4:
#include <QTextStream>
#include <Q3MemArray>
#include <Q3GridLayout>

#include <kapplication.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>

#include <kspread_cell.h>
#include <kspread_doc.h>
#include <kspread_sheet.h>
#include <kspread_undo.h>
#include <kspread_view.h>

#include "kspread_dlg_csv.h"

using namespace KSpread;

CSVDialog::CSVDialog( View * parent, const char * name, QRect const & rect, Mode mode)
  : KDialogBase( parent, name, true, QString::null, Ok|Cancel ),
    m_pView( parent ),
    m_cancelled( false ),
    m_adjustRows( 0 ),
    m_startline( 0 ),
    m_textquote( '"' ),
    m_delimiter( "," ),
    m_targetRect( rect ),
    m_mode( mode )
{
  if ( !name )
    setName( "CSV" );

  setSizeGripEnabled( TRUE );

  QWidget* page = new QWidget( this );
  setMainWidget( page );
  //  MyDialogLayout = new QGridLayout( page, 4, 4, marginHint(), spacingHint(), "MyDialogLayout");
  MyDialogLayout = new Q3GridLayout( page, 1, 1, 11, 6, "MyDialogLayout");

  // Limit the range
  int column = m_targetRect.left();
  Cell* lastCell = m_pView->activeSheet()->getLastCellColumn( column );
  if( lastCell )
    if( m_targetRect.bottom() > lastCell->row() )
      m_targetRect.setBottom( lastCell->row() );

  m_sheet = new Q3Table( page, "m_table" );
  //m_sheet->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)7, 0, 0, m_sheet->sizePolicy().hasHeightForWidth() ) );
  m_sheet->setNumRows( 0 );
  m_sheet->setNumCols( 0 );

  MyDialogLayout->addMultiCellWidget( m_sheet, 3, 3, 0, 3 );

  // Delimiter: comma, semicolon, tab, space, other
  m_delimiterBox = new Q3ButtonGroup( page, "m_delimiterBox" );
  m_delimiterBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, m_delimiterBox->sizePolicy().hasHeightForWidth() ) );
  m_delimiterBox->setTitle( i18n( "Delimiter" ) );
  m_delimiterBox->setColumnLayout(0, Qt::Vertical );
  m_delimiterBox->layout()->setSpacing( KDialog::spacingHint() );
  m_delimiterBox->layout()->setMargin( KDialog::marginHint() );
  m_delimiterBoxLayout = new Q3GridLayout( m_delimiterBox->layout() );
  m_delimiterBoxLayout->setAlignment( Qt::AlignTop );
  MyDialogLayout->addMultiCellWidget( m_delimiterBox, 0, 2, 0, 0 );

  m_ignoreDuplicates = new QCheckBox( page, "m_ignoreDuplicates" );
  m_ignoreDuplicates->setText( i18n( "Ignore duplicate delimiters" ) );

  MyDialogLayout->addMultiCellWidget( m_ignoreDuplicates, 2, 2, 2, 3 );

  m_radioComma = new QRadioButton( m_delimiterBox, "m_radioComma" );
  m_radioComma->setText( i18n( "Comma" ) );
  m_radioComma->setChecked( TRUE );
  m_delimiterBoxLayout->addWidget( m_radioComma, 0, 0 );

  m_radioSemicolon = new QRadioButton( m_delimiterBox, "m_radioSemicolon" );
  m_radioSemicolon->setText( i18n( "Semicolon" ) );
  m_delimiterBoxLayout->addWidget( m_radioSemicolon, 0, 1 );

  m_radioTab = new QRadioButton( m_delimiterBox, "m_radioTab" );
  m_radioTab->setText( i18n( "Tabulator" ) );
  m_delimiterBoxLayout->addWidget( m_radioTab, 1, 0 );

  m_radioSpace = new QRadioButton( m_delimiterBox, "m_radioSpace" );
  m_radioSpace->setText( i18n( "Space" ) );
  m_delimiterBoxLayout->addWidget( m_radioSpace, 1, 1 );

  m_radioOther = new QRadioButton( m_delimiterBox, "m_radioOther" );
  m_radioOther->setText( i18n( "Other" ) );
  m_delimiterBoxLayout->addWidget( m_radioOther, 0, 2 );

  m_delimiterEdit = new QLineEdit( m_delimiterBox, "m_delimiterEdit" );
  m_delimiterEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_delimiterEdit->sizePolicy().hasHeightForWidth() ) );
  m_delimiterEdit->setMaximumSize( QSize( 30, 32767 ) );
  m_delimiterBoxLayout->addWidget( m_delimiterEdit, 1, 2 );


  // Format: number, text, currency,
  m_formatBox = new Q3ButtonGroup( page, "m_formatBox" );
  m_formatBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, m_formatBox->sizePolicy().hasHeightForWidth() ) );
  m_formatBox->setTitle( i18n( "Format" ) );
  m_formatBox->setColumnLayout(0, Qt::Vertical );
  m_formatBox->layout()->setSpacing( KDialog::spacingHint() );
  m_formatBox->layout()->setMargin( KDialog::marginHint() );
  m_formatBoxLayout = new Q3GridLayout( m_formatBox->layout() );
  m_formatBoxLayout->setAlignment( Qt::AlignTop );
  MyDialogLayout->addMultiCellWidget( m_formatBox, 0, 2, 1, 1 );

  m_radioNumber = new QRadioButton( m_formatBox, "m_radioNumber" );
  m_radioNumber->setText( i18n( "Number" ) );
  m_formatBoxLayout->addMultiCellWidget( m_radioNumber, 1, 1, 0, 1 );

  m_radioText = new QRadioButton( m_formatBox, "m_radioText" );
  m_radioText->setText( i18n( "Text" ) );
  m_radioText->setChecked( TRUE );
  m_formatBoxLayout->addWidget( m_radioText, 0, 0 );

  m_radioCurrency = new QRadioButton( m_formatBox, "m_radioCurrency" );
  m_radioCurrency->setText( i18n( "Currency" ) );
  m_formatBoxLayout->addMultiCellWidget( m_radioCurrency, 0, 0, 1, 2 );

  m_radioDate = new QRadioButton( m_formatBox, "m_radioDate" );
  m_radioDate->setText( i18n( "Date" ) );
  m_formatBoxLayout->addWidget( m_radioDate, 1, 2 );

  m_comboLine = new QComboBox( FALSE, page, "m_comboLine" );
  m_comboLine->insertItem( i18n( "1" ) );
  m_comboLine->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, m_comboLine->sizePolicy().hasHeightForWidth() ) );

  MyDialogLayout->addWidget( m_comboLine, 1, 3 );

  m_comboQuote = new QComboBox( FALSE, page, "m_comboQuote" );
  m_comboQuote->insertItem( i18n( "\"" ) );
  m_comboQuote->insertItem( i18n( "'" ) );
  m_comboQuote->insertItem( i18n( "None" ) );
  m_comboQuote->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, m_comboQuote->sizePolicy().hasHeightForWidth() ) );

  MyDialogLayout->addWidget( m_comboQuote, 1, 2 );
  QSpacerItem* spacer_2 = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
  MyDialogLayout->addItem( spacer_2, 2, 3 );

  TextLabel3 = new QLabel( page, "TextLabel3" );
  TextLabel3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, TextLabel3->sizePolicy().hasHeightForWidth() ) );
  TextLabel3->setText( i18n( "Start at line:" ) );

  MyDialogLayout->addWidget( TextLabel3, 0, 3 );

  TextLabel2 = new QLabel( page, "TextLabel2" );
  TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, TextLabel2->sizePolicy().hasHeightForWidth() ) );
  TextLabel2->setText( i18n( "Textquote:" ) );

  MyDialogLayout->addWidget( TextLabel2, 0, 2 );

  if ( m_mode == Clipboard )
  {
    setCaption( i18n( "Inserting From Clipboard" ) );
    QMimeSource * mime = QApplication::clipboard()->data();
    if ( !mime )
    {
      KMessageBox::information( this, i18n("There is no data in the clipboard.") );
      m_cancelled = true;
      return;
    }

    if ( !mime->provides( "text/plain" ) )
    {
      KMessageBox::information( this, i18n("There is no usable data in the clipboard.") );
      m_cancelled = true;
      return;
    }
    m_fileArray = QByteArray(mime->encodedData( "text/plain" ) );
  }
  else if ( mode == File )
  {
    setCaption( i18n( "Inserting Text File" ) );
    QString file = KFileDialog::getOpenFileName(":",
                                                "text/plain",
                                                this);
    //cancel action !
    if ( file.isEmpty() )
    {
        actionButton( Ok )->setEnabled( false );
        m_cancelled = true;
        return;
    }
    QFile in(file);
    if (!in.open(QIODevice::ReadOnly))
    {
      KMessageBox::sorry( this, i18n("Cannot open input file.") );
      in.close();
      actionButton( Ok )->setEnabled( false );
      m_cancelled = true;
      return;
    }
    m_fileArray = QByteArray(in.size());
    in.read(m_fileArray.data(), in.size());
    in.close();
  }
  else
  {
    setCaption( i18n( "Text to Columns" ) );
    m_data = "";
    Cell  * cell;
    Sheet * sheet = m_pView->activeSheet();
    int col = m_targetRect.left();
    for (int i = m_targetRect.top(); i <= m_targetRect.bottom(); ++i)
    {
      cell = sheet->cellAt( col, i );
      if ( !cell->isEmpty() && !cell->isDefault() )
      {
        m_data += cell->strOutText();
      }
      m_data += "\n";
    }
  }

  fillSheet();
  fillComboBox();

  resize(sizeHint());

  m_sheet->setSelectionMode(Q3Table::NoSelection);

  connect(m_formatBox, SIGNAL(clicked(int)),
          this, SLOT(formatClicked(int)));
  connect(m_delimiterBox, SIGNAL(clicked(int)),
          this, SLOT(delimiterClicked(int)));
  connect(m_delimiterEdit, SIGNAL(returnPressed()),
          this, SLOT(returnPressed()));
  connect(m_delimiterEdit, SIGNAL(textChanged ( const QString & )),
          this, SLOT(textChanged ( const QString & ) ));
  connect(m_comboLine, SIGNAL(activated(const QString&)),
          this, SLOT(lineSelected(const QString&)));
  connect(m_comboQuote, SIGNAL(activated(const QString&)),
          this, SLOT(textquoteSelected(const QString&)));
  connect(m_sheet, SIGNAL(currentChanged(int, int)),
          this, SLOT(currentCellChanged(int, int)));
  connect(m_ignoreDuplicates, SIGNAL(stateChanged(int)),
          this, SLOT(ignoreDuplicatesChanged(int)));
}

CSVDialog::~CSVDialog()
{
  // no need to delete child widgets, Qt does it all for us
}

bool CSVDialog::cancelled()
{
  return m_cancelled;
}

void CSVDialog::fillSheet()
{
  int row, column;
  bool lastCharDelimiter = false;
  bool ignoreDups = m_ignoreDuplicates->isChecked();
  enum { S_START, S_QUOTED_FIELD, S_MAYBE_END_OF_QUOTED_FIELD, S_END_OF_QUOTED_FIELD,
         S_MAYBE_NORMAL_FIELD, S_NORMAL_FIELD } state = S_START;

  QChar x;
  QString field = "";

  for (row = 0; row < m_sheet->numRows(); ++row)
    for (column = 0; column < m_sheet->numCols(); ++column)
      m_sheet->clearCell(row, column);

  row = column = 1;
  if (m_mode != Column)
  {
    m_mode = Column;
    m_data = QString(m_fileArray);
    m_fileArray.resize(0);
  }

  QTextStream inputStream(&m_data, QIODevice::ReadOnly);
  inputStream.setEncoding(QTextStream::Locale);

  while (!inputStream.atEnd())
  {
    inputStream >> x; // read one char

    if (x == '\r') inputStream >> x; // eat '\r', to handle DOS/LOSEDOWS files correctly

    switch (state)
    {
     case S_START :
      if (x == m_textquote)
      {
        state = S_QUOTED_FIELD;
      }
      else if (QString(x) == m_delimiter)
      {
        if ((ignoreDups == false) || (lastCharDelimiter == false))
          ++column;
        lastCharDelimiter = true;
      }
      else if (x == '\n')
      {
        ++row;
        column = 1;
      }
      else
      {
        field += x;
        state = S_MAYBE_NORMAL_FIELD;
      }
      break;
     case S_QUOTED_FIELD :
      if (x == m_textquote)
      {
        state = S_MAYBE_END_OF_QUOTED_FIELD;
      }
      else if (x == '\n')
      {
        setText(row - m_startline, column, field);
        field = "";
        if (x == '\n')
        {
          ++row;
          column = 1;
        }
        else
        {
          if ((ignoreDups == false) || (lastCharDelimiter == false))
            ++column;
          lastCharDelimiter = true;
        }
        state = S_START;
      }
      else
      {
        field += x;
      }
      break;
     case S_MAYBE_END_OF_QUOTED_FIELD :
      if (x == m_textquote)
      {
        field += x;
        state = S_QUOTED_FIELD;
      }
      else if (QString(x) == m_delimiter || x == '\n')
      {
        setText(row - m_startline, column, field);
        field = "";
        if (x == '\n')
        {
          ++row;
          column = 1;
        }
        else
        {
          if ((ignoreDups == false) || (lastCharDelimiter == false))
            ++column;
          lastCharDelimiter = true;
        }
        state = S_START;
      }
      else
      {
        state = S_END_OF_QUOTED_FIELD;
      }
      break;
     case S_END_OF_QUOTED_FIELD :
       if (QString(x) == m_delimiter || x == '\n')
      {
        setText(row - m_startline, column, field);
        field = "";
        if (x == '\n')
        {
          ++row;
          column = 1;
        }
        else
        {
          if ((ignoreDups == false) || (lastCharDelimiter == false))
            ++column;
          lastCharDelimiter = true;
        }
        state = S_START;
      }
      else
      {
        state = S_END_OF_QUOTED_FIELD;
      }
      break;
     case S_MAYBE_NORMAL_FIELD :
      if (x == m_textquote)
      {
        field = "";
        state = S_QUOTED_FIELD;
        break;
      }
     case S_NORMAL_FIELD :
       if (QString(x) == m_delimiter || x == '\n')
      {
        setText(row - m_startline, column, field);
        field = "";
        if (x == '\n')
        {
          ++row;
          column = 1;
        }
        else
        {
          if ((ignoreDups == false) || (lastCharDelimiter == false))
            ++column;
          lastCharDelimiter = true;
        }
        state = S_START;
      }
      else
      {
        field += x;
      }
    }
    if (QString(x) != m_delimiter)
      lastCharDelimiter = false;
  }

  // file with only one line without '\n'
  if (field.length() > 0)
  {
    setText(row - m_startline, column, field);
    ++row;
    field = "";
  }

  adjustRows( row - m_startline );

  for (column = 0; column < m_sheet->numCols(); ++column)
  {
    QString header = m_sheet->horizontalHeader()->label(column);
    if (header != i18n("Text") && header != i18n("Number") &&
        header != i18n("Date") && header != i18n("Currency"))
      m_sheet->horizontalHeader()->setLabel(column, i18n("Text"));

    m_sheet->adjustColumn(column);
  }
}

void CSVDialog::fillComboBox()
{
  m_comboLine->clear();
  for (int row = 0; row < m_sheet->numRows(); ++row)
    m_comboLine->insertItem(QString::number(row + 1), row);
}

void CSVDialog::setText(int row, int col, const QString& text)
{
  if (row < 1) // skipped by the user
    return;

  if (m_sheet->numRows() < row) {
    m_sheet->setNumRows(row+5000); /* We add 5000 at a time to limit recalculations */
    m_adjustRows=1;
  }

  if (m_sheet->numCols() < col)
    m_sheet->setNumCols(col);

  m_sheet->setText(row - 1, col - 1, text);
}

/*
 * Called after the first fillSheet() when number of rows are unknown.
 */
void CSVDialog::adjustRows(int iRows)
{
  if (m_adjustRows)
  {
    m_sheet->setNumRows( iRows );
    m_adjustRows=0;
  }
}

void CSVDialog::returnPressed()
{
  if (m_delimiterBox->id(m_delimiterBox->selected()) != 4)
    return;

  m_delimiter = m_delimiterEdit->text();
  fillSheet();
}

void CSVDialog::textChanged ( const QString & )
{
  m_radioOther->setChecked ( true );
  delimiterClicked(4); // other
}

void CSVDialog::formatClicked(int id)
{
  QString header;

  switch (id)
  {
   case 1: // text
    header = i18n("Text");
    break;
   case 0: // number
    header = i18n("Number");
    break;
   case 3: // date
    header = i18n("Date");
    break;
   case 2: // currency
    header = i18n("Currency");
    break;
  }

  m_sheet->horizontalHeader()->setLabel(m_sheet->currentColumn(), header);
}

void CSVDialog::delimiterClicked(int id)
{
  switch (id)
  {
   case 0: // comma
    m_delimiter = ",";
    break;
   case 4: // other
    m_delimiter = m_delimiterEdit->text();
    break;
   case 2: // tab
    m_delimiter = "\t";
    break;
   case 3: // space
    m_delimiter = " ";
    break;
   case 1: // semicolon
    m_delimiter = ";";
    break;
  }

  fillSheet();
}

void CSVDialog::textquoteSelected(const QString& mark)
{
  if (mark == i18n("none"))
    m_textquote = 0;
  else
    m_textquote = mark[0];

  fillSheet();
}

void CSVDialog::lineSelected(const QString& line)
{
  m_startline = line.toInt() - 1;
  fillSheet();
}

void CSVDialog::currentCellChanged(int, int col)
{
  int id;
  QString header = m_sheet->horizontalHeader()->label(col);

  if (header == i18n("Text"))
    id = 1;
  else if (header == i18n("Number"))
    id = 0;
  else if (header == i18n("Date"))
    id = 3;
  else
    id = 2;

  m_formatBox->setButton(id);
}

void CSVDialog::accept()
{
  Sheet * sheet  = m_pView->activeSheet();
  QString csv_delimiter = QString::null;
  Cell  * cell;

  int numRows = m_sheet->numRows();
  int numCols = m_sheet->numCols();

  if (numRows == 0)
    ++numRows;

  if ( (numCols > m_targetRect.width()) && (m_targetRect.width() > 1) )
  {
    numCols = m_targetRect.width();
  }
  else
    m_targetRect.setRight( m_targetRect.left() + numCols );

  if ( (numRows > m_targetRect.height()) && (m_targetRect.height() > 1) )
    numRows = m_targetRect.height();
  else
    m_targetRect.setBottom( m_targetRect.top() + numRows );

  if ( numRows == 1 && numCols == 1)
  {
    Doc * doc = m_pView->doc();
    cell = sheet->nonDefaultCell( m_targetRect.left(), m_targetRect.top() );
    if ( !doc->undoLocked() )
    {
      UndoSetText * undo = new UndoSetText( doc, sheet , cell->text(), m_targetRect.left(),
                                                          m_targetRect.top(), cell->formatType() );
      doc->addCommand( undo );
    }
  }
  else
  {
      UndoChangeAreaTextCell * undo = new UndoChangeAreaTextCell( m_pView->doc(), sheet , m_targetRect );
      m_pView->doc()->addCommand( undo );
  }

  m_pView->doc()->emitBeginOperation();

  int i;
  int left = m_targetRect.left();
  int top  = m_targetRect.top();

  Q3MemArray<double> widths( numCols );
  for ( i = 0; i < numCols; ++i )
  {
    ColumnFormat * c  = sheet->nonDefaultColumnFormat( left + i );
    widths[i] = c->dblWidth();
  }

  for (int row = 0; row < numRows; ++row)
  {
    for (int col = 0; col < numCols; ++col)
    {
      cell = sheet->nonDefaultCell( left + col, top + row );
      cell->setCellText( getText( row, col ) );

      QFontMetrics fm = sheet->painter().fontMetrics();
      double w = fm.width( cell->strOutText() );
      if ( w == 0.0 )
      {
        QFontMetrics fm( cell->format()->textFont( left + col, top + row ) );
        w = fm.width('x') * (double) getText( row, col ).length();
      }

      if ( w > widths[col] )
        widths[col] = w;

      cell->format()->setFormatType(Generic_format);
      /*
      Disabling this code for now, everything will use Generic formatting,
      hoping for the best (Tomas)
      //### FIXME: long term solution is to allow to select Generic format ("autodetect") in the dialog and make it the default

      switch (getHeader(col))
      {
       case TEXT:
        break;
       case NUMBER:
        cell->format()->setFormatType(Number_format);
        cell->setPrecision(2);
        break;
       case DATE:
        cell->format()->setFormatType(ShortDate_format);
        break;
       case CURRENCY:
        cell->format()->setFormatType(Money_format);
        break;
      }
      */
    }
  }

  for ( i = 0; i < numCols; ++i )
  {
    ColumnFormat * c  = sheet->nonDefaultColumnFormat( left + i );
    c->setDblWidth( widths[i] );
    sheet->emit_updateColumn( c, left + i );
  }

  m_pView->slotUpdateView( sheet );
  QDialog::accept();
}

int CSVDialog::getHeader(int col)
{
  QString header = m_sheet->horizontalHeader()->label(col);

  if (header == i18n("Text"))
    return TEXT;
  else if (header == i18n("Number"))
    return NUMBER;
  else if (header == i18n("Currency"))
    return CURRENCY;
  else
    return DATE;
}

QString CSVDialog::getText(int row, int col)
{
  return m_sheet->text(row, col);
}

void CSVDialog::ignoreDuplicatesChanged(int)
{
  fillSheet();
}

#include "kspread_dlg_csv.moc"
