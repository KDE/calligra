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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qclipboard.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmime.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtable.h>
#include <qlayout.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>

#include "kspread_dlg_csv.h"
#include <kspread_cell.h>
#include <kspread_doc.h>
#include <kspread_sheet.h>
#include <kspread_undo.h>
#include <kspread_view.h>

KSpreadCSVDialog::KSpreadCSVDialog( KSpreadView * parent, const char * name, QRect const & rect, Mode mode)
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
    setName( "KSpreadCSVDialog" );

  setSizeGripEnabled( TRUE );

  QWidget* page = new QWidget( this );
  setMainWidget( page );
  //  MyDialogLayout = new QGridLayout( page, 4, 4, marginHint(), spacingHint(), "MyDialogLayout");
  MyDialogLayout = new QGridLayout( page, 1, 1, 11, 6, "MyDialogLayout");

  // Limit the range
  int column = m_targetRect.left();
  KSpreadCell* lastCell = m_pView->activeTable()->getLastCellColumn( column );
  if( lastCell )
    if( m_targetRect.bottom() > lastCell->row() )
      m_targetRect.setBottom( lastCell->row() );

  m_table = new QTable( page, "m_table" );
  //m_table->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)7, 0, 0, m_table->sizePolicy().hasHeightForWidth() ) );
  m_table->setNumRows( 0 );
  m_table->setNumCols( 0 );

  MyDialogLayout->addMultiCellWidget( m_table, 3, 3, 0, 3 );

  // Delimiter: comma, semicolon, tab, space, other
  m_delimiterBox = new QButtonGroup( page, "m_delimiterBox" );
  m_delimiterBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, m_delimiterBox->sizePolicy().hasHeightForWidth() ) );
  m_delimiterBox->setTitle( i18n( "Delimiter" ) );
  m_delimiterBox->setColumnLayout(0, Qt::Vertical );
  m_delimiterBox->layout()->setSpacing( KDialog::spacingHint() );
  m_delimiterBox->layout()->setMargin( KDialog::marginHint() );
  m_delimiterBoxLayout = new QGridLayout( m_delimiterBox->layout() );
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
  m_formatBox = new QButtonGroup( page, "m_formatBox" );
  m_formatBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, m_formatBox->sizePolicy().hasHeightForWidth() ) );
  m_formatBox->setTitle( i18n( "Format" ) );
  m_formatBox->setColumnLayout(0, Qt::Vertical );
  m_formatBox->layout()->setSpacing( KDialog::spacingHint() );
  m_formatBox->layout()->setMargin( KDialog::marginHint() );
  m_formatBoxLayout = new QGridLayout( m_formatBox->layout() );
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
    if (!in.open(IO_ReadOnly))
    {
      KMessageBox::sorry( this, i18n("Cannot open input file.") );
      in.close();
      actionButton( Ok )->setEnabled( false );
      m_cancelled = true;
      return;
    }
    m_fileArray = QByteArray(in.size());
    in.readBlock(m_fileArray.data(), in.size());
    in.close();
  }
  else
  {
    setCaption( i18n( "Text to Columns" ) );
    m_data = "";
    KSpreadCell  * cell;
    KSpreadSheet * table = m_pView->activeTable();
    int col = m_targetRect.left();
    for (int i = m_targetRect.top(); i <= m_targetRect.bottom(); ++i)
    {
      cell = table->cellAt( col, i );
      if ( !cell->isEmpty() && !cell->isDefault() )
      {
        m_data += cell->strOutText();
      }
      m_data += "\n";
    }
  }

  fillTable();
  fillComboBox();

  resize(sizeHint());

  m_table->setSelectionMode(QTable::NoSelection);

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
  connect(m_table, SIGNAL(currentChanged(int, int)),
          this, SLOT(currentCellChanged(int, int)));
  connect(m_ignoreDuplicates, SIGNAL(stateChanged(int)),
          this, SLOT(ignoreDuplicatesChanged(int)));
}

KSpreadCSVDialog::~KSpreadCSVDialog()
{
  // no need to delete child widgets, Qt does it all for us
}

bool KSpreadCSVDialog::cancelled()
{
  return m_cancelled;
}

void KSpreadCSVDialog::fillTable()
{
  int row, column;
  bool lastCharDelimiter = false;
  bool ignoreDups = m_ignoreDuplicates->isChecked();
  enum { S_START, S_QUOTED_FIELD, S_MAYBE_END_OF_QUOTED_FIELD, S_END_OF_QUOTED_FIELD,
         S_MAYBE_NORMAL_FIELD, S_NORMAL_FIELD } state = S_START;

  QChar x;
  QString field = "";

  for (row = 0; row < m_table->numRows(); ++row)
    for (column = 0; column < m_table->numCols(); ++column)
      m_table->clearCell(row, column);

  row = column = 1;
  if (m_mode != Column)
  {
    m_mode = Column;
    m_data = QString(m_fileArray);
    m_fileArray.resize(0);
  }

  QTextStream inputStream(m_data, IO_ReadOnly);
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
      else if (x == m_delimiter)
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
      else if (x == m_delimiter || x == '\n')
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
      if (x == m_delimiter || x == '\n')
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
      if (x == m_delimiter || x == '\n')
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
    if (x != m_delimiter)
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

  for (column = 0; column < m_table->numCols(); ++column)
  {
    QString header = m_table->horizontalHeader()->label(column);
    if (header != i18n("Text") && header != i18n("Number") &&
        header != i18n("Date") && header != i18n("Currency"))
      m_table->horizontalHeader()->setLabel(column, i18n("Text"));

    m_table->adjustColumn(column);
  }
}

void KSpreadCSVDialog::fillComboBox()
{
  m_comboLine->clear();
  for (int row = 0; row < m_table->numRows(); ++row)
    m_comboLine->insertItem(QString::number(row + 1), row);
}

void KSpreadCSVDialog::setText(int row, int col, const QString& text)
{
  if (row < 1) // skipped by the user
    return;

  if (m_table->numRows() < row) {
    m_table->setNumRows(row+5000); /* We add 5000 at a time to limit recalculations */
    m_adjustRows=1;
  }

  if (m_table->numCols() < col)
    m_table->setNumCols(col);

  m_table->setText(row - 1, col - 1, text);
}

/*
 * Called after the first fillTable() when number of rows are unknown.
 */
void KSpreadCSVDialog::adjustRows(int iRows)
{
  if (m_adjustRows)
  {
    m_table->setNumRows( iRows );
    m_adjustRows=0;
  }
}

void KSpreadCSVDialog::returnPressed()
{
  if (m_delimiterBox->id(m_delimiterBox->selected()) != 4)
    return;

  m_delimiter = m_delimiterEdit->text();
  fillTable();
}

void KSpreadCSVDialog::textChanged ( const QString & )
{
  m_radioOther->setChecked ( true );
  delimiterClicked(4); // other
}

void KSpreadCSVDialog::formatClicked(int id)
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

  m_table->horizontalHeader()->setLabel(m_table->currentColumn(), header);
}

void KSpreadCSVDialog::delimiterClicked(int id)
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

  fillTable();
}

void KSpreadCSVDialog::textquoteSelected(const QString& mark)
{
  if (mark == i18n("none"))
    m_textquote = 0;
  else
    m_textquote = mark[0];

  fillTable();
}

void KSpreadCSVDialog::lineSelected(const QString& line)
{
  m_startline = line.toInt() - 1;
  fillTable();
}

void KSpreadCSVDialog::currentCellChanged(int, int col)
{
  int id;
  QString header = m_table->horizontalHeader()->label(col);

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

void KSpreadCSVDialog::accept()
{
  KSpreadSheet * table  = m_pView->activeTable();
  QString csv_delimiter = QString::null;
  KSpreadCell  * cell;

  int numRows = m_table->numRows();
  int numCols = m_table->numCols();

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
    KSpreadDoc * doc = m_pView->doc();
    cell = table->nonDefaultCell( m_targetRect.left(), m_targetRect.top() );
    if ( !doc->undoLocked() )
    {
      KSpreadUndoSetText * undo = new KSpreadUndoSetText( doc, table , cell->text(), m_targetRect.left(),
                                                          m_targetRect.top(), cell->formatType() );
      doc->addCommand( undo );
    }
  }
  else
  {
      KSpreadUndoChangeAreaTextCell * undo = new KSpreadUndoChangeAreaTextCell( m_pView->doc(), table , m_targetRect );
      m_pView->doc()->addCommand( undo );
  }

  m_pView->doc()->emitBeginOperation();

  int i;
  int left = m_targetRect.left();
  int top  = m_targetRect.top();

  QMemArray<double> widths( numCols );
  for ( i = 0; i < numCols; ++i )
  {
    ColumnFormat * c  = table->nonDefaultColumnFormat( left + i );
    widths[i] = c->dblWidth();
  }

  for (int row = 0; row < numRows; ++row)
  {
    for (int col = 0; col < numCols; ++col)
    {
      cell = table->nonDefaultCell( left + col, top + row );
      cell->setCellText( getText( row, col ) );

      QFontMetrics fm = table->painter().fontMetrics();
      double w = fm.width( cell->strOutText() );
      if ( w == 0.0 )
      {
        QFontMetrics fm( cell->textFont( left + col, top + row ) );
        w = fm.width('x') * (double) getText( row, col ).length();
      }

      if ( w > widths[col] )
        widths[col] = w;

      cell->setFormatType (Generic_format);
      /*
      Disabling this code for now, everything will use Generic formatting,
      hoping for the best (Tomas)
      
      switch (getHeader(col))
      {
       case TEXT:
        break;
       case NUMBER:
        cell->setFormatType(Number_format);
        cell->setPrecision(2);
        break;
       case DATE:
        cell->setFormatType(ShortDate_format);
        break;
       case CURRENCY:
        cell->setFormatType(Money_format);
        break;
      }
      */
    }
  }

  for ( i = 0; i < numCols; ++i )
  {
    ColumnFormat * c  = table->nonDefaultColumnFormat( left + i );
    c->setDblWidth( widths[i] );
    table->emit_updateColumn( c, left + i );
  }

  m_pView->slotUpdateView( table );
  QDialog::accept();
}

int KSpreadCSVDialog::getHeader(int col)
{
  QString header = m_table->horizontalHeader()->label(col);

  if (header == i18n("Text"))
    return TEXT;
  else if (header == i18n("Number"))
    return NUMBER;
  else if (header == i18n("Currency"))
    return CURRENCY;
  else
    return DATE;
}

QString KSpreadCSVDialog::getText(int row, int col)
{
  return m_table->text(row, col);
}

void KSpreadCSVDialog::ignoreDuplicatesChanged(int)
{
  fillTable();
}

#include "kspread_dlg_csv.moc"
