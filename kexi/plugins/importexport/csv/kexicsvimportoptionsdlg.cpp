/* This file is part of the KDE project
   Copyright (C) 2005-2012 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "kexicsvimportoptionsdlg.h"
#include <widget/kexicharencodingcombobox.h>
#include <kexiutils/utils.h>

#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <KSharedConfig>
#include <KConfigGroup>

#include <QLabel>
#include <QCheckBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

KexiCSVImportOptions::DateFormat dateFormatFromString(const QString& s)
{
    QString str(s.toLower().trimmed());
    if (str == "dmy")
        return KexiCSVImportOptions::DMY;
    if (str == "ymd")
        return KexiCSVImportOptions::YMD;
    if (str == "mdy")
        return KexiCSVImportOptions::MDY;
    return KexiCSVImportOptions::AutoDateFormat;
}

QString dateFormatToString(KexiCSVImportOptions::DateFormat format)
{
    switch (format) {
    case KexiCSVImportOptions::DMY: return "DMY";
    case KexiCSVImportOptions::YMD: return "YMD";
    case KexiCSVImportOptions::MDY: return "MDY";
    default: break;
    }
    return QString();
}

KexiCSVImportOptions::KexiCSVImportOptions()
{
    KConfigGroup importExportGroup(KSharedConfig::openConfig()->group("ImportExport"));
    encoding = importExportGroup.readEntry("DefaultEncodingForImportingCSVFiles");
    if (encoding.isEmpty()) {
        encoding = QString::fromLatin1(KLocale::global()->encoding());
        defaultEncodingExplicitySet = false;
    } else
        defaultEncodingExplicitySet = true;

    dateFormat = dateFormatFromString(importExportGroup.readEntry("DateFormatWhenImportingCSVFiles"));

    trimmedInTextValuesChecked
        = importExportGroup.readEntry("StripBlanksOffOfTextValuesWhenImportingCSVFiles", true);
    nullsImportedAsEmptyTextChecked = importExportGroup.readEntry("ImportNULLsAsEmptyText", true);
}

KexiCSVImportOptions::~KexiCSVImportOptions()
{
}

bool KexiCSVImportOptions::operator== (const KexiCSVImportOptions & opt) const
{
    return defaultEncodingExplicitySet == opt.defaultEncodingExplicitySet
           && trimmedInTextValuesChecked == opt.trimmedInTextValuesChecked
           && encoding == opt.encoding
           && dateFormat == opt.dateFormat
           && nullsImportedAsEmptyTextChecked == opt.nullsImportedAsEmptyTextChecked;
}

bool KexiCSVImportOptions::operator!= (const KexiCSVImportOptions & opt) const
{
    return !(*this == opt);
}

//----------------------------------

KexiCSVImportOptionsDialog::KexiCSVImportOptionsDialog(
    const KexiCSVImportOptions& options, QWidget* parent)
        : QDialog(parent)
{
    setObjectName("KexiCSVImportOptionsDialog");
    setWindowTitle(xi18nc("@title:window", "CSV Import Options"));
    setModal(true);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    QGridLayout *lyr = new QGridLayout;
    mainLayout->addLayout(lyr);

    QGroupBox* textEncodingGroupBox = new QGroupBox(xi18n("Text encoding"), this);
    lyr->addWidget(textEncodingGroupBox, 0, 0, 1, 2);
    QVBoxLayout* textEncodingGroupBoxLyr = new QVBoxLayout;
    KexiUtils::setStandardMarginsAndSpacing(textEncodingGroupBoxLyr);
    textEncodingGroupBox->setLayout(textEncodingGroupBoxLyr);

    textEncodingGroupBoxLyr->addItem(new QSpacerItem(20, 15, QSizePolicy::Fixed, QSizePolicy::Fixed));

    m_encodingComboBox = new KexiCharacterEncodingComboBox(textEncodingGroupBox, options.encoding);
    textEncodingGroupBoxLyr->addWidget(m_encodingComboBox);

    lyr->addItem(new QSpacerItem(20, KexiUtils::spacingHint(), QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 2);

    m_chkAlwaysUseThisEncoding = new QCheckBox(
        xi18n("Always use this encoding when importing CSV data files"), textEncodingGroupBox);
    textEncodingGroupBoxLyr->addWidget(m_chkAlwaysUseThisEncoding);

    m_comboDateFormat = new QComboBox(this);
    m_comboDateFormat->setObjectName("m_comboDateFormat");
    m_comboDateFormat->addItem(xi18nc("Date format: Auto", "Auto"));
    QString year(xi18n("year")), month(xi18n("month")), day(xi18n("day"));
    KLocalizedString mask = kxi18nc("do not reorder placeholders, just translate e.g. and - to the "
                                   "separator used by dates in your language",
                                   "%1, %2, %3 (e.g. %4-%5-%6)");
    m_comboDateFormat->addItem(
        mask.subs(day).subs(month).subs(year).subs(30).subs(12).subs(2008).toString());
    m_comboDateFormat->addItem(
        mask.subs(year).subs(month).subs(day).subs(2008).subs(12).subs(30).toString());
    m_comboDateFormat->addItem(
        mask.subs(month).subs(day).subs(year).subs(12).subs(30).subs(2008).toString());
    lyr->addWidget(m_comboDateFormat, 1, 1);

    QLabel* lblDateFormat = new QLabel(xi18n("Date format:"), this);
    lblDateFormat->setBuddy(m_comboDateFormat);
    lyr->addWidget(lblDateFormat, 1, 0);

    m_chkStripWhiteSpaceInTextValues = new QCheckBox(
        xi18n("Strip leading and trailing blanks off of text values"), this);
    lyr->addWidget(m_chkStripWhiteSpaceInTextValues, 2, 0, 1, 2);

    m_chkImportNULLsAsEmptyText = new QCheckBox(
                xi18n("Import missing text values as empty texts"), this);
    lyr->addWidget(m_chkImportNULLsAsEmptyText, 3, 0, 1, 2);
    lyr->addItem(new QSpacerItem(30, KexiUtils::spacingHint(), QSizePolicy::Minimum, QSizePolicy::Expanding), 4, 0);
    //update widgets
    m_encodingComboBox->setSelectedEncoding(options.encoding);
    if (options.defaultEncodingExplicitySet) {
        m_chkAlwaysUseThisEncoding->setChecked(true);
    }
    m_comboDateFormat->setCurrentIndex((int)options.dateFormat);
    m_chkStripWhiteSpaceInTextValues->setChecked(options.trimmedInTextValuesChecked);
    m_chkImportNULLsAsEmptyText->setChecked(options.nullsImportedAsEmptyTextChecked);

    // buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    okButton->setDefault(true);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    mainLayout->addWidget(buttonBox);

    adjustSize();
    m_encodingComboBox->setFocus();
}

KexiCSVImportOptionsDialog::~KexiCSVImportOptionsDialog()
{
}

KexiCSVImportOptions KexiCSVImportOptionsDialog::options() const
{
    KexiCSVImportOptions opt;
    opt.encoding = m_encodingComboBox->selectedEncoding();
    opt.trimmedInTextValuesChecked = m_chkStripWhiteSpaceInTextValues->isChecked();
    opt.nullsImportedAsEmptyTextChecked = m_chkImportNULLsAsEmptyText->isChecked();
    return opt;
}

void KexiCSVImportOptionsDialog::accept()
{
    KConfigGroup importExportGroup(KSharedConfig::openConfig()->group("ImportExport"));
    if (m_chkAlwaysUseThisEncoding->isChecked())
        importExportGroup.writeEntry("DefaultEncodingForImportingCSVFiles",
                                     m_encodingComboBox->selectedEncoding());
    else
        importExportGroup.deleteEntry("DefaultEncodingForImportingCSVFiles");

    const KexiCSVImportOptions::DateFormat dateFormat
        = (KexiCSVImportOptions::DateFormat)m_comboDateFormat->currentIndex();
    if (dateFormat == KexiCSVImportOptions::AutoDateFormat)
        importExportGroup.deleteEntry("DateFormatWhenImportingCSVFiles");
    else
        importExportGroup.writeEntry("DateFormatWhenImportingCSVFiles", dateFormatToString(dateFormat));

    importExportGroup.writeEntry("StripBlanksOffOfTextValuesWhenImportingCSVFiles",
                                 m_chkStripWhiteSpaceInTextValues->isChecked());
    importExportGroup.writeEntry("ImportNULLsAsEmptyText",
                                 m_chkImportNULLsAsEmptyText->isChecked());

    QDialog::accept();
}

