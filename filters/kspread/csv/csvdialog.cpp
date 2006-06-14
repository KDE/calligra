/* This file is part of the KDE project
   Copyright (C) 1999 David Faure <faure@kde.org>
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>

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

#include <dialogui.h>
#include <csvdialog.h>

#include <q3table.h>
#include <QCheckBox>
#include <QCursor>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QTextStream>
#include <q3buttongroup.h>
#include <QPushButton>
#include <QRadioButton>
#include <QTextCodec>

#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <kcombobox.h>
#include <kmessagebox.h>
#include <kcharsets.h>

CSVDialog::CSVDialog(QWidget* parent, QByteArray& fileArray, const QString /*seperator*/)
    : KDialog(parent),
      m_adjustRows(false),
      m_adjustCols(false),
      m_startRow(0),
      m_startCol(0),
      m_endRow(-1),
      m_endCol(-1),
      m_textquote('"'),
      m_delimiter(','),
      m_ignoreDups(false),
      m_fileArray(fileArray),
      m_dialog(new DialogUI(this)),
      m_codec( QTextCodec::codecForName( "UTF-8" ) )
{
    // ???????????????????????? default button == no but we don't add it !
	setDefaultButton(KDialog::No);
        setButtons( KDialog::Ok|KDialog::Cancel );
    setCaption( i18n( "Import" ) );
    kapp->restoreOverrideCursor();

    QStringList encodings;
    encodings << i18nc( "Descriptive encoding name", "Recommended ( %1 )" , "UTF-8" );
    encodings << i18nc( "Descriptive encoding name", "Locale ( %1 )" , QString(QTextCodec::codecForLocale()->name() ));
    encodings += KGlobal::charsets()->descriptiveEncodingNames();
    // Add a few non-standard encodings, which might be useful for text files
    const QString description(i18nc("Descriptive encoding name","Other ( %1 )"));
    encodings << description.arg("Apple Roman"); // Apple
    encodings << description.arg("IBM 850") << description.arg("IBM 866"); // MS DOS
    encodings << description.arg("CP 1258"); // Windows
    m_dialog->comboBoxEncoding->insertStringList(encodings);

    m_formatList << i18n( "Text" );
    m_formatList << i18n( "Number" );
    m_formatList << i18n( "Currency" );
    m_formatList << i18n( "Date" );
    m_formatList << i18n( "Decimal Comma Number" );
    m_formatList << i18n( "Decimal Point Number" );
    m_dialog->m_formatComboBox->insertStringList( m_formatList );

    m_dialog->m_sheet->setReadOnly( true );

    fillTable();

    //resize(sizeHint());
    resize( 600, 400 ); // Try to show as much as possible of the table view
    setMainWidget(m_dialog);

    m_dialog->m_sheet->setSelectionMode( Q3Table::Multi );

    connect(m_dialog->m_formatComboBox, SIGNAL(activated( const QString& )),
            this, SLOT(formatChanged( const QString& )));
    connect(m_dialog->m_delimiterBox, SIGNAL(clicked(int)),
            this, SLOT(delimiterClicked(int)));
    connect(m_dialog->m_delimiterEdit, SIGNAL(returnPressed()),
            this, SLOT(returnPressed()));
    connect(m_dialog->m_delimiterEdit, SIGNAL(textChanged ( const QString & )),
            this, SLOT(formatChanged ( const QString & ) ));
    connect(m_dialog->m_comboQuote, SIGNAL(activated(const QString &)),
            this, SLOT(textquoteSelected(const QString &)));
    connect(m_dialog->m_sheet, SIGNAL(currentChanged(int, int)),
            this, SLOT(currentCellChanged(int, int)));
    connect(m_dialog->m_ignoreDuplicates, SIGNAL(stateChanged(int)),
            this, SLOT(ignoreDuplicatesChanged(int)));
    connect(m_dialog->m_updateButton, SIGNAL(clicked()),
            this, SLOT(updateClicked()));
    connect(m_dialog->comboBoxEncoding, SIGNAL(textChanged ( const QString & )),
            this, SLOT(encodingChanged ( const QString & ) ));
}

CSVDialog::~CSVDialog()
{
    kapp->setOverrideCursor(Qt::WaitCursor);
}

void CSVDialog::fillTable( )
{
    int row, column;
    bool lastCharDelimiter = false;
    enum { S_START, S_QUOTED_FIELD, S_MAYBE_END_OF_QUOTED_FIELD, S_END_OF_QUOTED_FIELD,
           S_MAYBE_NORMAL_FIELD, S_NORMAL_FIELD } state = S_START;

    QChar x;
    QString field;

    kapp->setOverrideCursor(Qt::WaitCursor);

    for (row = 0; row < m_dialog->m_sheet->numRows(); ++row)
        for (column = 0; column < m_dialog->m_sheet->numCols(); ++column)
            m_dialog->m_sheet->clearCell(row, column);

    int maxColumn = 1;
    row = column = 1;
    QTextStream inputStream(m_fileArray, QIODevice::ReadOnly);
    kDebug(30501) << "Encoding: " << m_codec->name() << endl;
    inputStream.setCodec( m_codec );

    bool lastCharWasCr = false; // Last character was a Carriage Return
    while (!inputStream.atEnd())
    {
        inputStream >> x; // read one char

        // ### TODO: we should perhaps skip all other control characters
        if ( x == '\r' )
        {
            // We have a Carriage Return, assume that its role is the one of a LineFeed
            lastCharWasCr = true;
            x = '\n'; // Replace by Line Feed
        }
        else if ( x == '\n' && lastCharWasCr )
        {
            // The end of line was already handled by the Carriage Return, so do nothing for this character
            lastCharWasCr = false;
            continue;
        }
        else if ( x == QChar( 0xc ) )
        {
            // We have a FormFeed, skip it
            lastCharWasCr = false;
            continue;
        }
        else
        {
            lastCharWasCr = false;
        }

        if ( column > maxColumn )
          maxColumn = column;

        switch (state)
        {
         case S_START :
            if (x == m_textquote)
            {
                state = S_QUOTED_FIELD;
            }
            else if (x == m_delimiter)
            {
                if ((m_ignoreDups == false) || (lastCharDelimiter == false))
                    ++column;
                lastCharDelimiter = true;
            }
            else if (x == '\n')
            {
                ++row;
                column = 1;
                if ( row > ( m_endRow - m_startRow ) && m_endRow >= 0 )
                  break;
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
                setText(row - m_startRow, column - m_startCol, field);
                field = QString::null;
                if (x == '\n')
                {
                    ++row;
                    column = 1;
                    if ( row > ( m_endRow - m_startRow ) && m_endRow >= 0 )
                      break;
                }
                else
                {
                    if ((m_ignoreDups == false) || (lastCharDelimiter == false))
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
                setText(row - m_startRow, column - m_startCol, field);
                field = QString::null;
                if (x == '\n')
                {
                    ++row;
                    column = 1;
                    if ( row > ( m_endRow - m_startRow ) && m_endRow >= 0 )
                      break;
                }
                else
                {
                    if ((m_ignoreDups == false) || (lastCharDelimiter == false))
                        ++column;
                    lastCharDelimiter = true;
                }
                state = S_START;
            }
            break;
         case S_MAYBE_NORMAL_FIELD :
            if (x == m_textquote)
            {
                field = QString::null;
                state = S_QUOTED_FIELD;
                break;
            }
         case S_NORMAL_FIELD :
            if (x == m_delimiter || x == '\n')
            {
                setText(row - m_startRow, column - m_startCol, field);
                field = QString::null;
                if (x == '\n')
                {
                    ++row;
                    column = 1;
                    if ( row > ( m_endRow - m_startRow ) && m_endRow >= 0 )
                      break;
                }
                else
                {
                    if ((m_ignoreDups == false) || (lastCharDelimiter == false))
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

    if ( !field.isEmpty() )
    {
      // the last line of the file had not any line end
      setText(row - m_startRow, column - m_startCol, field);
      ++row;
      field = QString::null;
    }

    m_adjustCols = true;
    adjustRows( row - m_startRow );
    adjustCols( maxColumn - m_startCol );
    m_dialog->m_colEnd->setMaximum( maxColumn );
    if ( m_endCol == -1 )
      m_dialog->m_colEnd->setValue( maxColumn );


    for (column = 0; column < m_dialog->m_sheet->numCols(); ++column)
    {
        const QString header = m_dialog->m_sheet->horizontalHeader()->label(column);
        if ( !m_formatList.contains( header ) )
            m_dialog->m_sheet->horizontalHeader()->setLabel(column, i18n("Text"));

        m_dialog->m_sheet->adjustColumn(column);
    }
    fillComboBox();

    kapp->restoreOverrideCursor();
}

void CSVDialog::fillComboBox()
{
  if ( m_endRow == -1 )
    m_dialog->m_rowEnd->setValue( m_dialog->m_sheet->numRows() );
  else
    m_dialog->m_rowEnd->setValue( m_endRow );

  if ( m_endCol == -1 )
    m_dialog->m_colEnd->setValue( m_dialog->m_sheet->numCols() );
  else
    m_dialog->m_colEnd->setValue( m_endCol );

  m_dialog->m_rowEnd->setMinimum( 1 );
  m_dialog->m_colEnd->setMinimum( 1 );
  m_dialog->m_rowEnd->setMaximum( m_dialog->m_sheet->numRows() );
  m_dialog->m_colEnd->setMaximum( m_dialog->m_sheet->numCols() );

  m_dialog->m_rowStart->setMinimum( 1 );
  m_dialog->m_colStart->setMinimum( 1 );
  m_dialog->m_rowStart->setMaximum( m_dialog->m_sheet->numRows() );
  m_dialog->m_colStart->setMaximum( m_dialog->m_sheet->numCols() );
}

int CSVDialog::getRows()
{
  int rows = m_dialog->m_sheet->numRows();
  if ( m_endRow >= 0 )
  {
    if ( rows > ( m_startRow + m_endRow ) )
      rows = m_startRow + m_endRow;
  }

  return rows;
}

int CSVDialog::getCols()
{
  int cols = m_dialog->m_sheet->numCols();
  if ( m_endCol >= 0 )
  {
    if ( cols > ( m_startCol + m_endCol ) )
      cols = m_startCol + m_endCol;
  }

  return cols;
}

int CSVDialog::getHeader(int col)
{
    QString header = m_dialog->m_sheet->horizontalHeader()->label(col);

    if (header == i18n("Text"))
        return TEXT;
    else if (header == i18n("Number"))
        return NUMBER;
    else if (header == i18n("Currency"))
        return CURRENCY;
    else if ( header == i18n( "Date" ) )
        return DATE;
    else if ( header == i18n( "Decimal Comma Number" ) )
        return COMMANUMBER;
    else if ( header == i18n( "Decimal Point Number" ) )
        return POINTNUMBER;
    else
        return TEXT; // Should not happen
}

QString CSVDialog::getText(int row, int col)
{
    return m_dialog->m_sheet->text( row, col );
}

void CSVDialog::setText(int row, int col, const QString& text)
{
    if ( row < 1 || col < 1 ) // skipped by the user
        return;

    if ( ( row > ( m_endRow - m_startRow ) && m_endRow > 0 ) || ( col > ( m_endCol - m_startCol ) && m_endCol > 0 ) )
      return;

    if ( m_dialog->m_sheet->numRows() < row )
    {
        m_dialog->m_sheet->setNumRows( row + 5000 ); /* We add 5000 at a time to limit recalculations */
        m_adjustRows = true;
    }

    if ( m_dialog->m_sheet->numCols() < col )
    {
        m_dialog->m_sheet->setNumCols( col );
        m_adjustCols = true;
    }

    m_dialog->m_sheet->setText( row - 1, col - 1, text );
}

/*
 * Called after the first fillTable() when number of rows are unknown.
 */
void CSVDialog::adjustRows(int iRows)
{
    if (m_adjustRows)
    {
        m_dialog->m_sheet->setNumRows( iRows );
        m_adjustRows = false;
    }
}

void CSVDialog::adjustCols(int iCols)
{
    if (m_adjustCols)
    {
        m_dialog->m_sheet->setNumCols( iCols );
        m_adjustCols = false;

        if ( m_endCol == -1 )
        {
          if ( iCols > ( m_endCol - m_startCol ) )
            iCols = m_endCol - m_startCol;

          m_dialog->m_sheet->setNumCols( iCols );
        }
    }
}

void CSVDialog::returnPressed()
{
    if (m_dialog->m_delimiterBox->id(m_dialog->m_delimiterBox->selected()) != 4)
        return;

    m_delimiter = m_dialog->m_delimiterEdit->text()[0];
    fillTable();
}

void CSVDialog::textChanged ( const QString & )
{
    m_dialog->m_radioOther->setChecked ( true );
    delimiterClicked(4); // other
}

void CSVDialog::formatChanged( const QString& newValue )
{
    //kDebug(30501) << "CSVDialog::formatChanged:" << newValue << endl;
    for ( int i = 0; i < m_dialog->m_sheet->numSelections(); ++i )
    {
        Q3TableSelection select ( m_dialog->m_sheet->selection( i ) );
        for ( int j = select.leftCol(); j <= select.rightCol() ; ++j )
        {
            m_dialog->m_sheet->horizontalHeader()->setLabel( j, newValue );

        }
    }
}

void CSVDialog::delimiterClicked(int id)
{
    switch (id)
    {
    case 0: // comma
        m_delimiter = ',';
        break;
    case 4: // other
        m_delimiter = m_dialog->m_delimiterEdit->text()[0];
        break;
    case 2: // tab
        m_delimiter = '\t';
        break;
    case 3: // space
        m_delimiter = ' ';
        break;
    case 1: // semicolon
        m_delimiter = ';';
        break;
    }

    fillTable();
}

void CSVDialog::textquoteSelected(const QString& mark)
{
    if (mark == i18n("None"))
        m_textquote = 0;
    else
        m_textquote = mark[0];

    fillTable();
}

void CSVDialog::updateClicked()
{
  if ( !checkUpdateRange() )
    return;

  m_startRow = m_dialog->m_rowStart->value() - 1;
  m_endRow   = m_dialog->m_rowEnd->value();

  m_startCol  = m_dialog->m_colStart->value() - 1;
  m_endCol    = m_dialog->m_colEnd->value();

  fillTable();
}

bool CSVDialog::checkUpdateRange()
{
  if ( ( m_dialog->m_rowStart->value() > m_dialog->m_rowEnd->value() )
       || ( m_dialog->m_colStart->value() > m_dialog->m_colEnd->value() ) )
  {
    KMessageBox::error( this, i18n( "Please check the ranges you specified. The start value must be lower than the end value." ) );
    return false;
  }

  return true;
}

void CSVDialog::currentCellChanged(int, int col)
{
    const QString header = m_dialog->m_sheet->horizontalHeader()->label(col);
    m_dialog->m_formatComboBox->setCurrentText( header );
}

void CSVDialog::ignoreDuplicatesChanged(int)
{
  if (m_dialog->m_ignoreDuplicates->isChecked())
    m_ignoreDups = true;
  else
    m_ignoreDups = false;
  fillTable();
}

QTextCodec* CSVDialog::getCodec(void) const
{
    const QString strCodec( KGlobal::charsets()->encodingForName( m_dialog->comboBoxEncoding->currentText() ) );
    kDebug(30502) << "Encoding: " << strCodec << endl;

    bool ok = false;
    QTextCodec* codec = QTextCodec::codecForName( strCodec.toUtf8() );

    // If QTextCodec has not found a valid encoding, so try with KCharsets.
    if ( codec )
    {
        ok = true;
    }
    else
    {
        codec = KGlobal::charsets()->codecForName( strCodec, ok );
    }

    // Still nothing?
    if ( !codec || !ok )
    {
        // Default: UTF-8
        kWarning(30502) << "Cannot find encoding:" << strCodec << endl;
        // ### TODO: what parent to use?
        KMessageBox::error( 0, i18n("Cannot find encoding: %1", strCodec ) );
        return 0;
    }

    return codec;
}

void CSVDialog::encodingChanged ( const QString & )
{
    QTextCodec* codec = getCodec();

    if ( codec )
    {
        m_codec = codec;
        fillTable();
    }
}


#include <csvdialog.moc>
