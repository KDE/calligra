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

#include <csvexportdialog.h>
#include <exportdialogui.h>

#include <kspread_map.h>
#include <kspread_sheet.h>

#include <q3buttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qcursor.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3listview.h>
#include <q3ptrlist.h>
#include <qradiobutton.h>
#include <qtextstream.h>
#include <qtabwidget.h>
#include <qtextcodec.h>
#include <qvalidator.h>

#include <kapplication.h>
#include <klocale.h>
#include <kdebug.h>
#include <kcombobox.h>
#include <kmessagebox.h>
#include <kcharsets.h>

using namespace KSpread;

CSVExportDialog::CSVExportDialog( QWidget * parent )
  : KDialogBase( parent, 0, true, QString::null, Ok | Cancel, No, true ),
    m_dialog( new ExportDialogUI( this ) ),
    m_delimiter( "," ),
    m_textquote('"')
{
  kapp->restoreOverrideCursor();

  QStringList encodings;
  encodings << i18nc( "Descriptive encoding name", "Recommended ( %1 )" , "UTF-8" );
  encodings << i18nc( "Descriptive encoding name", "Locale ( %1 )" ,QString(QTextCodec::codecForLocale()->name()) );
  encodings += KGlobal::charsets()->descriptiveEncodingNames();
  // Add a few non-standard encodings, which might be useful for text files
  const QString description(i18nc("Descriptive encoding name","Other ( %1 )"));
  encodings << description.arg("Apple Roman"); // Apple
  encodings << description.arg("IBM 850") << description.arg("IBM 866"); // MS DOS
  encodings << description.arg("CP 1258"); // Windows

  m_dialog->comboBoxEncoding->insertStringList(encodings);

  setButtonBoxOrientation ( Qt::Vertical );

  setMainWidget(m_dialog);

  // Invalid 'Other' delimiters
  // - Quotes
  // - CR,LF,Vetical-tab,Formfeed,ASCII bel
  QRegExp rx( "^[^\"'\r\n\v\f\a]{0,1}$" );
  m_delimiterValidator = new QRegExpValidator( rx, m_dialog->m_delimiterBox );
  m_dialog->m_delimiterEdit->setValidator( m_delimiterValidator );

  connect( m_dialog->m_delimiterBox, SIGNAL( clicked(int) ),
           this, SLOT( delimiterClicked( int ) ) );
  connect( m_dialog->m_delimiterEdit, SIGNAL( returnPressed() ),
           this, SLOT( returnPressed() ) );
  connect( m_dialog->m_delimiterEdit, SIGNAL( textChanged ( const QString & ) ),
           this, SLOT(textChanged ( const QString & ) ) );
  connect( m_dialog->m_comboQuote, SIGNAL( activated( const QString & ) ),
           this, SLOT( textquoteSelected( const QString & ) ) );
  connect( m_dialog->m_selectionOnly, SIGNAL( toggled( bool ) ),
           this, SLOT( selectionOnlyChanged( bool ) ) );
}

CSVExportDialog::~CSVExportDialog()
{
  kapp->setOverrideCursor(Qt::WaitCursor);
  delete m_delimiterValidator;
}

void CSVExportDialog::fillSheet( Map * map )
{
  m_dialog->m_sheetList->clear();
  Q3CheckListItem * item;

  Q3PtrListIterator<Sheet> it( map->sheetList() );
  for( ; it.current(); ++it )
  {
    item = new Q3CheckListItem( m_dialog->m_sheetList,
                               it.current()->sheetName(),
                               Q3CheckListItem::CheckBox );
    item->setOn(true);
    m_dialog->m_sheetList->insertItem( item );
  }

  m_dialog->m_sheetList->setSorting(0, true);
  m_dialog->m_sheetList->sort();
  m_dialog->m_sheetList->setSorting( -1 );
}

QChar CSVExportDialog::getDelimiter() const
{
  return m_delimiter[0];
}

QChar CSVExportDialog::getTextQuote() const
{
  return m_textquote;
}

bool CSVExportDialog::printAlwaysSheetDelimiter() const
{
  return m_dialog->m_delimiterAboveAll->isChecked();
}

QString CSVExportDialog::getSheetDelimiter() const
{
  return m_dialog->m_sheetDelimiter->text();
}

bool CSVExportDialog::exportSheet(QString const & sheetName) const
{
  for (Q3ListViewItem * item = m_dialog->m_sheetList->firstChild(); item; item = item->nextSibling())
  {
    if (((Q3CheckListItem * ) item)->isOn())
    {
      if ( ((Q3CheckListItem * ) item)->text() == sheetName )
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

  if ( m_dialog->m_delimiterEdit->text().isEmpty() )
  {
    enableButtonOK( ! m_dialog->m_radioOther->isChecked() );
    return;
  }

  m_dialog->m_radioOther->setChecked ( true );
  delimiterClicked(4);
}

void CSVExportDialog::delimiterClicked( int id )
{
  enableButtonOK( true );

  //Erase "Other Delimiter" text box if the user has selected one of 
  //the standard options instead (comma, semicolon, tab or space)
  if (id != 4)
  	m_dialog->m_delimiterEdit->setText("");
  
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
      enableButtonOK( ! m_dialog->m_delimiterEdit->text().isEmpty() );
      m_delimiter = m_dialog->m_delimiterEdit->text();
      break;
  }
}

void CSVExportDialog::textquoteSelected( const QString & mark )
{
  m_textquote = mark[0];
}

void CSVExportDialog::selectionOnlyChanged( bool on )
{
  m_dialog->m_sheetList->setEnabled( !on );
  m_dialog->m_delimiterLineBox->setEnabled( !on );

  if ( on )
    m_dialog->m_tabWidget->setCurrentPage( 1 );
}

bool CSVExportDialog::exportSelectionOnly() const
{
  return m_dialog->m_selectionOnly->isChecked();
}

QTextCodec* CSVExportDialog::getCodec(void) const
{
    const QString strCodec( KGlobal::charsets()->encodingForName( m_dialog->comboBoxEncoding->currentText() ) );
    kDebug(30502) << "Encoding: " << strCodec << endl;

    bool ok = false;
    QTextCodec* codec = QTextCodec::codecForName( strCodec.utf8() );

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

QString CSVExportDialog::getEndOfLine(void) const
{
    QString strReturn;
    if (m_dialog->radioEndOfLineLF==m_dialog->buttonGroupEndOfLine->selected())
        strReturn="\n";
    else if (m_dialog->radioEndOfLineCRLF==m_dialog->buttonGroupEndOfLine->selected())
        strReturn="\r\n";
    else if (m_dialog->radioEndOfLineCR==m_dialog->buttonGroupEndOfLine->selected())
        strReturn="\r";
    else
        strReturn="\n";

    return strReturn;
}

#include "csvexportdialog.moc"

