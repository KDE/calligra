/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2004 Nicolas GOUTTE <goutte@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "csvexportdialog.h"
#include "csvexport.h"

#include "sheets/core/Map.h"
#include "sheets/engine/SheetBase.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KSharedConfig>

#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QGuiApplication>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

using namespace Calligra::Sheets;
using namespace Qt::StringLiterals;

CSVExportDialog::CSVExportDialog(QWidget *parent)
    : QDialog(parent)
    , m_delimiter(",")
    , m_textquote('"')
{
    m_dialog.setupUi(this);

    setWindowTitle(i18nc("@title:dialog", "Export to CVS"));

    m_dialog.m_buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    m_dialog.m_buttonBox->setProperty("_breeze_force_frame", true);
    m_dialog.encodingLayout->insertStretch(0);
    m_dialog.encodingLayout->insertStretch(2);
    m_dialog.cellLayout->insertStretch(0);
    m_dialog.cellLayout->insertStretch(2);
    auto okButton = m_dialog.m_buttonBox->button(QDialogButtonBox::Ok);
    okButton->setText(i18nc("@action:button", "Export"));
    okButton->setIcon(QIcon::fromTheme(u"document-export-symbolic"_s));
    okButton->setDefault(true);
    QGuiApplication::restoreOverrideCursor();

    m_dialog.bottomLayout->setContentsMargins(style()->pixelMetric(QStyle::PM_LayoutLeftMargin),
                                              style()->pixelMetric(QStyle::PM_LayoutTopMargin),
                                              style()->pixelMetric(QStyle::PM_LayoutRightMargin),
                                              style()->pixelMetric(QStyle::PM_LayoutBottomMargin));

    m_dialog.m_buttonBox->setContentsMargins(style()->pixelMetric(QStyle::PM_LayoutLeftMargin),
                                             style()->pixelMetric(QStyle::PM_LayoutTopMargin),
                                             style()->pixelMetric(QStyle::PM_LayoutRightMargin),
                                             style()->pixelMetric(QStyle::PM_LayoutBottomMargin));

    auto group = new QButtonGroup(this);
    group->addButton(m_dialog.m_radioComma, 0);
    group->addButton(m_dialog.m_radioSemicolon, 1);
    group->addButton(m_dialog.m_radioTab, 2);
    group->addButton(m_dialog.m_radioSpace, 3);
    group->addButton(m_dialog.m_radioOther, 4);

    m_dialog.m_tabWidget->tabBar()->setExpanding(true);

    // Invalid 'Other' delimiters
    // - Quotes
    // - CR,LF,Vertical-tab,Formfeed,ASCII bel
    QRegularExpression rx("^[^\"'\r\n\v\f\a]{0,1}$");
    m_delimiterValidator = new QRegularExpressionValidator(rx, m_dialog.m_delimiterEdit);
    m_dialog.m_delimiterEdit->setValidator(m_delimiterValidator);

    connect(group, &QButtonGroup::idClicked, this, &CSVExportDialog::delimiterClicked);
    connect(m_dialog.m_delimiterEdit, &QLineEdit::returnPressed, this, &CSVExportDialog::returnPressed);
    connect(m_dialog.m_delimiterEdit, &QLineEdit::textChanged, this, &CSVExportDialog::textChanged);
    connect(m_dialog.m_comboQuote, &QComboBox::textActivated, this, &CSVExportDialog::textquoteSelected);
    connect(m_dialog.m_selectionOnly, &QAbstractButton::toggled, this, &CSVExportDialog::selectionOnlyChanged);
    connect(m_dialog.m_buttonBox, &QDialogButtonBox::accepted, this, &CSVExportDialog::slotOk);
    connect(m_dialog.m_buttonBox, &QDialogButtonBox::rejected, this, &CSVExportDialog::slotCancel);

    loadSettings();
}

CSVExportDialog::~CSVExportDialog()
{
    saveSettings();
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    delete m_delimiterValidator;
}

void CSVExportDialog::loadSettings()
{
    KConfigGroup configGroup = KSharedConfig::openConfig()->group("CSVDialog Settings");
    m_textquote = configGroup.readEntry("textQuote", "\"")[0];
    m_delimiter = configGroup.readEntry("delimiter", ",");
    const QString codecText = configGroup.readEntry("codec", "");
    bool selectionOnly = configGroup.readEntry("selectionOnly", false);
    const QString sheetDelim = configGroup.readEntry("sheetDelimiter", m_dialog.m_sheetDelimiter->text());
    bool delimAbove = configGroup.readEntry("sheetDelimiterAbove", false);
    const QString eol = configGroup.readEntry("eol", "\r\n");

    if (m_delimiter == ",")
        m_dialog.m_radioComma->setChecked(true);
    else if (m_delimiter == "\t")
        m_dialog.m_radioTab->setChecked(true);
    else if (m_delimiter == " ")
        m_dialog.m_radioSpace->setChecked(true);
    else if (m_delimiter == ";")
        m_dialog.m_radioSemicolon->setChecked(true);
    else {
        m_dialog.m_radioOther->setChecked(true);
        m_dialog.m_delimiterEdit->setText(m_delimiter);
    }
    m_dialog.m_comboQuote->setCurrentIndex(m_textquote == '\'' ? 1 : m_textquote == '"' ? 0 : 2);
    m_dialog.m_selectionOnly->setChecked(selectionOnly);
    m_dialog.m_sheetDelimiter->setText(sheetDelim);
    m_dialog.m_delimiterAboveAll->setChecked(delimAbove);
    if (eol == "\r\n")
        m_dialog.radioEndOfLineCRLF->setChecked(true);
    else if (eol == "\r")
        m_dialog.radioEndOfLineCR->setChecked(true);
    else
        m_dialog.radioEndOfLineLF->setChecked(true);
}

void CSVExportDialog::saveSettings()
{
    KConfigGroup configGroup = KSharedConfig::openConfig()->group("CSVDialog Settings");
    configGroup.writeEntry("textQuote", QString(m_textquote));
    configGroup.writeEntry("delimiter", m_delimiter);
    configGroup.writeEntry("selectionOnly", exportSelectionOnly());
    configGroup.writeEntry("sheetDelimiter", getSheetDelimiter());
    configGroup.writeEntry("sheetDelimiterAbove", printAlwaysSheetDelimiter());
    configGroup.writeEntry("eol", getEndOfLine());
    configGroup.sync();
}

void CSVExportDialog::fillSheet(Map *map)
{
    m_dialog.m_sheetList->clear();
    QListWidgetItem *item;

    for (SheetBase *sheet : map->sheetList()) {
        item = new QListWidgetItem(sheet->sheetName(), m_dialog.m_sheetList);
        item->setCheckState(Qt::Checked);
        m_dialog.m_sheetList->addItem(item);
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
    return m_dialog.m_delimiterAboveAll->isChecked();
}

QString CSVExportDialog::getSheetDelimiter() const
{
    return m_dialog.m_sheetDelimiter->text();
}

bool CSVExportDialog::exportSheet(QString const &sheetName) const
{
    for (int i = 0; i < m_dialog.m_sheetList->count(); ++i) {
        QListWidgetItem *const item = m_dialog.m_sheetList->item(i);
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
    if (!m_dialog.m_radioOther->isChecked())
        return;

    m_delimiter = m_dialog.m_delimiterEdit->text();
}

void CSVExportDialog::textChanged(const QString &)
{
    if (m_dialog.m_delimiterEdit->text().isEmpty()) {
        auto okButton = m_dialog.m_buttonBox->button(QDialogButtonBox::Ok);
        okButton->setEnabled(!m_dialog.m_radioOther->isChecked());
        return;
    }

    m_dialog.m_radioOther->setChecked(true);
    delimiterClicked(4);
}

void CSVExportDialog::delimiterClicked(int id)
{
    auto okButton = m_dialog.m_buttonBox->button(QDialogButtonBox::Ok);
    okButton->setEnabled(true);

    // Erase "Other Delimiter" text box if the user has selected one of
    // the standard options instead (comma, semicolon, tab or space)
    if (id != 4)
        m_dialog.m_delimiterEdit->setText("");

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
        okButton->setEnabled(!m_dialog.m_delimiterEdit->text().isEmpty());
        m_delimiter = m_dialog.m_delimiterEdit->text();
        break;
    }
}

void CSVExportDialog::textquoteSelected(const QString &mark)
{
    m_textquote = mark[0];
}

void CSVExportDialog::selectionOnlyChanged(bool on)
{
    m_dialog.m_sheetList->setEnabled(!on);
    m_dialog.m_delimiterLineBox->setEnabled(!on);

    if (on)
        m_dialog.m_tabWidget->setCurrentIndex(1);
}

bool CSVExportDialog::exportSelectionOnly() const
{
    return m_dialog.m_selectionOnly->isChecked();
}

QString CSVExportDialog::getEndOfLine(void) const
{
    QString strReturn;
    if (m_dialog.radioEndOfLineLF->isChecked())
        strReturn = "\n";
    else if (m_dialog.radioEndOfLineCRLF->isChecked())
        strReturn = "\r\n";
    else if (m_dialog.radioEndOfLineCR->isChecked())
        strReturn = "\r";
    else
        strReturn = "\n";

    return strReturn;
}
