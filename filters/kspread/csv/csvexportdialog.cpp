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

#include "csvexportdialog.h"

#include "kspread/Map.h"
#include "kspread/Sheet.h"

#include <KApplication>
#include <KCharsets>
#include <KMessageBox>

#include <QTextCodec>
#include <QValidator>

using namespace KSpread;

CSVExportDialog::CSVExportDialog(QWidget * parent)
        : KDialog(parent),
        m_dialog(new ExportDialogUI(this)),
        m_delimiter(","),
        m_textquote('"')
{
    setButtons(KDialog::Ok | KDialog::Cancel);
    setDefaultButton(KDialog::No);
    kapp->restoreOverrideCursor();

    QStringList encodings;
    encodings << i18nc("Descriptive encoding name", "Recommended ( %1 )" , "UTF-8");
    encodings << i18nc("Descriptive encoding name", "Locale ( %1 )" , QString(QTextCodec::codecForLocale()->name()));
    encodings += KGlobal::charsets()->descriptiveEncodingNames();
    // Add a few non-standard encodings, which might be useful for text files
    const QString description(i18nc("Descriptive encoding name", "Other ( %1 )"));
    encodings << description.arg("Apple Roman"); // Apple
    encodings << description.arg("IBM 850") << description.arg("IBM 866"); // MS DOS
    encodings << description.arg("CP 1258"); // Windows

    m_dialog->comboBoxEncoding->addItems(encodings);

    setButtonsOrientation(Qt::Vertical);

    setMainWidget(m_dialog);

    // Invalid 'Other' delimiters
    // - Quotes
    // - CR,LF,Vetical-tab,Formfeed,ASCII bel
    QRegExp rx("^[^\"'\r\n\v\f\a]{0,1}$");
    m_delimiterValidator = new QRegExpValidator(rx, m_dialog->m_delimiterBox);
    m_dialog->m_delimiterEdit->setValidator(m_delimiterValidator);

    connect(m_dialog->m_delimiterBox, SIGNAL(clicked(int)),
            this, SLOT(delimiterClicked(int)));
    connect(m_dialog->m_delimiterEdit, SIGNAL(returnPressed()),
            this, SLOT(returnPressed()));
    connect(m_dialog->m_delimiterEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(textChanged(const QString &)));
    connect(m_dialog->m_comboQuote, SIGNAL(activated(const QString &)),
            this, SLOT(textquoteSelected(const QString &)));
    connect(m_dialog->m_selectionOnly, SIGNAL(toggled(bool)),
            this, SLOT(selectionOnlyChanged(bool)));
    connect(this, SIGNAL(okClicked()), SLOT(slotOk()));
    connect(this, SIGNAL(cancelClicked()), this, SLOT(slotCancel()));

    loadSettings();
}

CSVExportDialog::~CSVExportDialog()
{
    saveSettings();
    kapp->setOverrideCursor(Qt::WaitCursor);
    delete m_delimiterValidator;
}

void CSVExportDialog::loadSettings()
{
    KConfigGroup configGroup = KGlobal::config()->group("CSVDialog Settings");
    m_textquote = configGroup.readEntry("textQuote", "\"")[0];
    m_delimiter = configGroup.readEntry("delimiter", ",");
    const QString codecText = configGroup.readEntry("codec", "");
    bool selectionOnly = configGroup.readEntry("selectionOnly", false);
    const QString sheetDelim = configGroup.readEntry("sheetDelimiter", m_dialog->m_sheetDelimiter->text());
    bool delimAbove = configGroup.readEntry("sheetDelimiterAbove", false);
    const QString eol = configGroup.readEntry("eol", "\r\n");

    // update widgets
    if (!codecText.isEmpty()) {
        m_dialog->comboBoxEncoding->setCurrentIndex(m_dialog->comboBoxEncoding->findText(codecText));
    }
    if (m_delimiter == ",")
        m_dialog->m_radioComma->setChecked(true);
    else if (m_delimiter == "\t")
        m_dialog->m_radioTab->setChecked(true);
    else if (m_delimiter == " ")
        m_dialog->m_radioSpace->setChecked(true);
    else if (m_delimiter == ";")
        m_dialog->m_radioSemicolon->setChecked(true);
    else {
        m_dialog->m_radioOther->setChecked(true);
        m_dialog->m_delimiterEdit->setText(m_delimiter);
    }
    m_dialog->m_comboQuote->setCurrentIndex(m_textquote == '\'' ? 1 : m_textquote == '"' ? 0 : 2);
    m_dialog->m_selectionOnly->setChecked(selectionOnly);
    m_dialog->m_sheetDelimiter->setText(sheetDelim);
    m_dialog->m_delimiterAboveAll->setChecked(delimAbove);
    if (eol == "\r\n")
        m_dialog->radioEndOfLineCRLF->setChecked(true);
    else if (eol == "\r")
        m_dialog->radioEndOfLineCR->setChecked(true);
    else
        m_dialog->radioEndOfLineLF->setChecked(true);
}

void CSVExportDialog::saveSettings()
{
    KConfigGroup configGroup = KGlobal::config()->group("CSVDialog Settings");
    configGroup.writeEntry("textQuote", QString(m_textquote));
    configGroup.writeEntry("delimiter", m_delimiter);
    configGroup.writeEntry("codec", m_dialog->comboBoxEncoding->currentText());
    configGroup.writeEntry("selectionOnly", exportSelectionOnly());
    configGroup.writeEntry("sheetDelimiter", getSheetDelimiter());
    configGroup.writeEntry("sheetDelimiterAbove", printAlwaysSheetDelimiter());
    configGroup.writeEntry("eol", getEndOfLine());
    configGroup.sync();
}

void CSVExportDialog::fillSheet(Map * map)
{
    m_dialog->m_sheetList->clear();
    QListWidgetItem *item;

    foreach(Sheet* sheet, map->sheetList()) {
        item = new QListWidgetItem(sheet->sheetName() ,m_dialog->m_sheetList);
        item->setCheckState(Qt::Checked);
        m_dialog->m_sheetList->addItem(item);
    }
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
    for (int i = 0; i < m_dialog->m_sheetList->count(); ++i) {
        QListWidgetItem *const item = m_dialog->m_sheetList->item(i);
        if (item->checkState() == Qt::Checked) {
            if (item->text() == sheetName) {
                return true;
            }
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
    if (!m_dialog->m_radioOther->isChecked())
        return;

    m_delimiter = m_dialog->m_delimiterEdit->text();
}

void CSVExportDialog::textChanged(const QString &)
{

    if (m_dialog->m_delimiterEdit->text().isEmpty()) {
        enableButtonOk(! m_dialog->m_radioOther->isChecked());
        return;
    }

    m_dialog->m_radioOther->setChecked(true);
    delimiterClicked(4);
}

void CSVExportDialog::delimiterClicked(int id)
{
    enableButtonOk(true);

    //Erase "Other Delimiter" text box if the user has selected one of
    //the standard options instead (comma, semicolon, tab or space)
    if (id != 4)
        m_dialog->m_delimiterEdit->setText("");

    switch (id) {
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
        enableButtonOk(! m_dialog->m_delimiterEdit->text().isEmpty());
        m_delimiter = m_dialog->m_delimiterEdit->text();
        break;
    }
}

void CSVExportDialog::textquoteSelected(const QString & mark)
{
    m_textquote = mark[0];
}

void CSVExportDialog::selectionOnlyChanged(bool on)
{
    m_dialog->m_sheetList->setEnabled(!on);
    m_dialog->m_delimiterLineBox->setEnabled(!on);

    if (on)
        m_dialog->m_tabWidget->setCurrentIndex(1);
}

bool CSVExportDialog::exportSelectionOnly() const
{
    return m_dialog->m_selectionOnly->isChecked();
}

QTextCodec* CSVExportDialog::getCodec(void) const
{
    const QString strCodec(KGlobal::charsets()->encodingForName(m_dialog->comboBoxEncoding->currentText()));
    kDebug(30502) << "Encoding:" << strCodec;

    bool ok = false;
    QTextCodec* codec = QTextCodec::codecForName(strCodec.toUtf8());

    // If QTextCodec has not found a valid encoding, so try with KCharsets.
    if (codec) {
        ok = true;
    } else {
        codec = KGlobal::charsets()->codecForName(strCodec, ok);
    }

    // Still nothing?
    if (!codec || !ok) {
        // Default: UTF-8
        kWarning(30502) << "Cannot find encoding:" << strCodec;
        // ### TODO: what parent to use?
        KMessageBox::error(0, i18n("Cannot find encoding: %1", strCodec));
        return 0;
    }

    return codec;
}

QString CSVExportDialog::getEndOfLine(void) const
{
    QString strReturn;
    if (m_dialog->radioEndOfLineLF->isChecked())
        strReturn = "\n";
    else if (m_dialog->radioEndOfLineCRLF->isChecked())
        strReturn = "\r\n";
    else if (m_dialog->radioEndOfLineCR->isChecked())
        strReturn = "\r";
    else
        strReturn = "\n";

    return strReturn;
}

#include "csvexportdialog.moc"
