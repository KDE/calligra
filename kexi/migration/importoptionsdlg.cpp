/* This file is part of the KDE project
   Copyright (C) 2005 Jarosław Staniek <staniek@kde.org>

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

#include "importoptionsdlg.h"
#include <widget/kexicharencodingcombobox.h>

#include <KexiIcon.h>

#include <kconfig.h>
#include <kglobal.h>
#include <KSharedConfig>
#include <KConfigGroup>
#include <KLocalizedString>

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDir>
#include <QLabel>
#include <QCheckBox>
#include <QGridLayout>

using namespace KexiMigration;

OptionsDialog::OptionsDialog(const QString& databaseFile, const QString& selectedEncoding,
                             QWidget* parent)
        : QDialog(parent)
{
    setModal(true);
    setObjectName("KexiMigration::OptionsDialog");
    setWindowTitle(xi18nc("@title:window", "Advanced Import Options"));
    setWindowIcon(koIcon("configure"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    QWidget *plainPage = new QWidget(this);
    mainLayout->addWidget(plainPage);
    QGridLayout *lyr = new QGridLayout(plainPage);

    m_encodingComboBox = new KexiCharacterEncodingComboBox(plainPage, selectedEncoding);
    m_encodingComboBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    lyr->addWidget(m_encodingComboBox, 1, 1);
    QLabel* lbl = new QLabel(
        xi18n("<title>Text encoding for Microsoft Access database</title>\n"
             "<para>Database file <filename>%1</filename> appears to be created by a version of Microsoft Access older than 2000.</para>"
             "<para>In order to properly import national characters, you may need to choose a proper text encoding "
             "if the database was created on a computer with a different character set.</para>",
             QDir::convertSeparators(databaseFile)),
        plainPage);
    lbl->setAlignment(Qt::AlignLeft);
    lbl->setWordWrap(true);
    lbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    lyr->addWidget(lbl, 0, 0, 1, 3);

    QLabel* lbl2 = new QLabel(xi18n("Text encoding:"), plainPage);
    lbl2->setBuddy(m_encodingComboBox);
    lyr->addWidget(lbl2, 1, 0);

    m_chkAlwaysUseThisEncoding = new QCheckBox(
        xi18n("Always use this encoding in similar situations"), plainPage);
    lyr->addWidget(m_chkAlwaysUseThisEncoding, 2, 1, 1, 2);

    lyr->addItem(new QSpacerItem(20, 111, QSizePolicy::Minimum, QSizePolicy::Expanding), 3, 1);
    lyr->addItem(new QSpacerItem(121, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 2);

    //read config
    KConfigGroup importExportGroup(KSharedConfig::openConfig()->group("ImportExport"));
    QString defaultEncodingForMSAccessFiles
        = importExportGroup.readEntry("DefaultEncodingForMSAccessFiles");
    if (!defaultEncodingForMSAccessFiles.isEmpty()) {
        m_encodingComboBox->setSelectedEncoding(defaultEncodingForMSAccessFiles);
        m_chkAlwaysUseThisEncoding->setChecked(true);
    }

    // buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    mainLayout->addWidget(buttonBox);

    adjustSize();
    m_encodingComboBox->setFocus();
}

OptionsDialog::~OptionsDialog()
{
}

KexiCharacterEncodingComboBox* OptionsDialog::encodingComboBox() const
{
    return m_encodingComboBox;
}

void OptionsDialog::accept()
{
    KConfigGroup importExportGroup(KSharedConfig::openConfig()->group("ImportExport"));
    if (m_chkAlwaysUseThisEncoding->isChecked())
        importExportGroup.writeEntry("defaultEncodingForMSAccessFiles",
                                     m_encodingComboBox->selectedEncoding());
    else
        importExportGroup.deleteEntry("defaultEncodingForMSAccessFiles");

    QDialog::accept();
}

