/* This file is part of the KDE project
   Copyright (C) 2012 Oleg Kukharchuk <oleg.kuh@gmail.org>
   Copyright (C) 2005-2013 Jarosław Staniek <staniek@kde.org>

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

#include "kexicsvexportwizard.h"
#include "kexicsvwidgets.h"
#include <db/cursor.h>
#include <db/utils.h>
#include <core/KexiMainWindowIface.h>
#include <core/kexiproject.h>
#include <core/kexipartinfo.h>
#include <core/kexipartmanager.h>
#include <core/kexiguimsghandler.h>
#include <kexiutils/utils.h>
#include <widget/kexicharencodingcombobox.h>
#include <widget/KexiFileWidget.h>
#include <KoIcon.h>

#include <QCheckBox>
#include <QGroupBox>
#include <QClipboard>
#include <QTextStream>
#include <QGridLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QDesktopWidget>
#include <kapplication.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kapplication.h>
#include <kdebug.h>
#include <ksavefile.h>
#include <kglobal.h>
#include <KDialog>

KexiCSVExportWizard::KexiCSVExportWizard(const KexiCSVExport::Options& options,
        QWidget * parent)
        : KAssistantDialog(parent)
        , m_options(options)
// , m_mode(mode)
// , m_itemId(itemId)
        , m_fileSavePage(0)
        , m_defaultsBtn(0)
        , m_importExportGroup(KGlobal::config()->group("ImportExport"))
        , m_cancelled(false)
{
    if (m_options.mode == KexiCSVExport::Clipboard) {
        button(KDialog::User1)->setText(i18n("Copy"));
    } else {
        button(KDialog::User1)->setText(i18n("Export"));
    }
    showButton(KDialog::Help, false);

    QString infoLblFromText;
    KexiGUIMessageHandler msgh(this);
    m_tableOrQuery = new KexiDB::TableOrQuerySchema(
        KexiMainWindowIface::global()->project()->dbConnection(), m_options.itemId);
    if (m_tableOrQuery->table()) {
        if (m_options.mode == KexiCSVExport::Clipboard) {
            setWindowTitle(i18n("Copy Data From Table to Clipboard"));
            infoLblFromText = i18n("Copying data from table:");
        } else {
            setWindowTitle(i18n("Export Data From Table to CSV File"));
            infoLblFromText = i18n("Exporting data from table:");
        }
    } else if (m_tableOrQuery->query()) {
        if (m_options.mode == KexiCSVExport::Clipboard) {
            setWindowTitle(i18n("Copy Data From Query to Clipboard"));
            infoLblFromText = i18n("Copying data from table:");
        } else {
            setWindowTitle(i18n("Export Data From Query to CSV File"));
            infoLblFromText = i18n("Exporting data from query:");
        }
    } else {
        msgh.showErrorMessage(KexiMainWindowIface::global()->project()->dbConnection(),
                              i18n("Could not open data for exporting."));
        m_cancelled = true;
        return;
    }

    QString text = "\n" + m_tableOrQuery->captionOrName();
    int m_rowCount = KexiDB::rowCount(*m_tableOrQuery);
    int columns = KexiDB::fieldCount(*m_tableOrQuery);
    text += "\n";
    if (m_rowCount > 0)
        text += i18n("(rows: %1, columns: %2)", m_rowCount, columns);
    else
        text += i18n("(columns: %1)", columns);
    infoLblFromText.append(text);

    // OK, source data found.

    // Setup pages

    // 1. File Save Page
    if (m_options.mode == KexiCSVExport::File) {
        m_fileSaveWidget = new KexiFileWidget(
            KUrl("kfiledialog:///CSVImportExport"), //startDir
            KexiFileWidget::Custom | KexiFileWidget::SavingFileBasedDB,
            this);
        m_fileSaveWidget->setObjectName("m_fileSavePage");
        //m_fileSavePage->setMinimumHeight(kapp->desktop()->availableGeometry().height() / 2);
        m_fileSaveWidget->setAdditionalFilters(csvMimeTypes().toSet());
        m_fileSaveWidget->setDefaultExtension("csv");
        m_fileSaveWidget->setLocationText(
            KexiUtils::stringToFileName(m_tableOrQuery->captionOrName()));
        m_fileSavePage = new KPageWidgetItem(m_fileSaveWidget, i18n("Enter Name of File You Want to Save Data To"));
        addPage(m_fileSavePage);
        connect(this, SIGNAL(currentPageChanged(KPageWidgetItem*, KPageWidgetItem*)),
                this, SLOT(slotCurrentPageChanged(KPageWidgetItem*, KPageWidgetItem*)));
    }

    /* 2. Export options
        m_exportOptionsPage
        exportOptionsLyr
            m_infoLblFrom
            m_infoLblTo
            m_showOptionsButton
            m_exportOptionsSection
            exportOptionsSectionLyr
    */
    m_exportOptionsWidget = new QWidget(this);
    m_exportOptionsWidget->setObjectName("m_exportOptionsPage");

    QGridLayout *exportOptionsLyr = new QGridLayout(m_exportOptionsWidget);
    exportOptionsLyr->setObjectName("exportOptionsLyr");

    m_infoLblFrom = new KexiCSVInfoLabel(infoLblFromText, m_exportOptionsWidget, true/*showFnameLine*/);
    KexiPart::Info *partInfo = Kexi::partManager().infoForClass(
            QString("org.kexi-project.%1").arg(m_tableOrQuery->table() ? "table" : "query"));
    if (partInfo)
        m_infoLblFrom->setIcon(partInfo->itemIconName());
    m_infoLblFrom->separator()->hide();
    m_infoLblFrom->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    exportOptionsLyr->addWidget(m_infoLblFrom, 0, 0, 1, 2);

    m_infoLblTo = new KexiCSVInfoLabel(
        (m_options.mode == KexiCSVExport::File) ? i18n("To CSV file:") : i18n("To clipboard."),
        m_exportOptionsWidget, true/*showFnameLine*/);

    if (m_options.mode == KexiCSVExport::Clipboard)
        m_infoLblTo->setIcon(koIconName("edit-paste"));

    m_infoLblTo->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    exportOptionsLyr->addWidget(m_infoLblTo, 1, 0, 1, 2);

//    QWidget *stretchWidget = new QWidget();
//    exportOptionsLyr->addWidget(stretchWidget, 2, 0, 2, 2);
    exportOptionsLyr->setRowStretch(2, 1);
    m_showOptionsButton = new KPushButton(KGuiItem(i18n("Show Options >>"), koIconName("configure")));
    m_showOptionsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(m_showOptionsButton, SIGNAL(clicked()), this, SLOT(slotShowOptionsButtonClicked()));
    exportOptionsLyr->addWidget(m_showOptionsButton, 3, 1, Qt::AlignRight);

    // -<options section>
    m_exportOptionsSection = new QGroupBox(""/*i18n("Options")*/);
    m_exportOptionsSection->setObjectName("m_exportOptionsSection");
    m_exportOptionsSection->setAlignment(Qt::Vertical);
    m_exportOptionsSection->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    exportOptionsLyr->addWidget(m_exportOptionsSection, 4, 0, 1, 2);
#ifdef __GNUC__
#warning TODO    exportOptionsLyr->setRowStretch();
#else
#pragma WARNING( TODO exportOptionsLyr->setRowStretch(); )
#endif

    QGridLayout *exportOptionsSectionLyr = new QGridLayout;
    exportOptionsLyr->setObjectName("exportOptionsLyr");
    m_exportOptionsSection->setLayout(exportOptionsSectionLyr);

    // -delimiter
    QLabel *delimiterLabel = new QLabel(i18n("Delimiter:"));
    exportOptionsSectionLyr->addWidget(delimiterLabel, 0, 0);

    m_delimiterWidget = new KexiCSVDelimiterWidget(false /* !lineEditOnBottom*/);
    m_delimiterWidget->setDelimiter(defaultDelimiter());
    delimiterLabel->setBuddy(m_delimiterWidget);
    exportOptionsSectionLyr->addWidget(m_delimiterWidget, 0, 1);

    // -text quote
    QLabel *textQuoteLabel = new QLabel(i18n("Text quote:"));
    exportOptionsSectionLyr->addWidget(textQuoteLabel, 1, 0);

    QWidget *textQuoteWidget = new QWidget;
    QHBoxLayout *textQuoteLyr = new QHBoxLayout(textQuoteWidget);

    m_textQuote = new KexiCSVTextQuoteComboBox(textQuoteWidget);
    m_textQuote->setTextQuote(defaultTextQuote());
    textQuoteLabel->setBuddy(m_textQuote);
    textQuoteLyr->addWidget(m_textQuote);
    textQuoteLyr->addStretch(0);

    exportOptionsSectionLyr->addWidget(textQuoteWidget, 1, 1);

    // - character encoding
    QLabel *characterEncodingLabel = new QLabel(i18n("Text encoding:"));
    exportOptionsSectionLyr->addWidget(characterEncodingLabel, 2, 0);

    m_characterEncodingCombo = new KexiCharacterEncodingComboBox();
    m_characterEncodingCombo->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    characterEncodingLabel->setBuddy(m_characterEncodingCombo);
    exportOptionsSectionLyr->addWidget(m_characterEncodingCombo, 2, 1);

    // - checkboxes
    m_addColumnNamesCheckBox = new QCheckBox(i18n("Add column names as the first row"));
    m_addColumnNamesCheckBox->setChecked(true);
    exportOptionsSectionLyr->addWidget(m_addColumnNamesCheckBox, 3, 1);

    m_defaultsBtn = new KPushButton(i18n("Defaults"), this);
    connect(m_defaultsBtn, SIGNAL(clicked()), this, SLOT(slotDefaultsButtonClicked()));
    exportOptionsLyr->addWidget(m_defaultsBtn, 5, 0);
    exportOptionsLyr->setColumnStretch(1, 1);
    m_alwaysUseCheckBox = new QCheckBox(
        m_options.mode == KexiCSVExport::Clipboard ?
          i18n("Always use above options for copying")
        : i18n("Always use above options for exporting"));
    exportOptionsLyr->addWidget(m_alwaysUseCheckBox, 5, 1, Qt::AlignRight);

    m_exportOptionsSection->hide();
    m_defaultsBtn->hide();
    m_alwaysUseCheckBox->hide();
    // -</options section>

    m_exportOptionsPage = new KPageWidgetItem(m_exportOptionsWidget, m_options.mode == KexiCSVExport::Clipboard ? i18n("Copying") : i18n("Exporting"));
    addPage(m_exportOptionsPage);

    // load settings
    if (m_options.mode != KexiCSVExport::Clipboard
            && readBoolEntry("ShowOptionsInCSVExportDialog", false)) {
        show();
        slotShowOptionsButtonClicked();
    }
    if (readBoolEntry("StoreOptionsForCSVExportDialog", false)) {
        // load defaults:
        m_alwaysUseCheckBox->setChecked(true);
        QString s = readEntry("DefaultDelimiterForExportingCSVFiles", defaultDelimiter());
        if (!s.isEmpty())
            m_delimiterWidget->setDelimiter(s);
        s = readEntry("DefaultTextQuoteForExportingCSVFiles", defaultTextQuote());
        m_textQuote->setTextQuote(s); //will be invaliudated here, so not a problem
        s = readEntry("DefaultEncodingForExportingCSVFiles");
        if (!s.isEmpty())
            m_characterEncodingCombo->setSelectedEncoding(s);
        m_addColumnNamesCheckBox->setChecked(
            readBoolEntry("AddColumnNamesForExportingCSVFiles", true));
    }

    // -keep widths equal on page #2:
    int width = qMax(m_infoLblFrom->leftLabel()->sizeHint().width(),
                     m_infoLblTo->leftLabel()->sizeHint().width());
    m_infoLblFrom->leftLabel()->setFixedWidth(width);
    m_infoLblTo->leftLabel()->setFixedWidth(width);

    updateGeometry();
}

KexiCSVExportWizard::~KexiCSVExportWizard()
{
    delete m_tableOrQuery;
}

bool KexiCSVExportWizard::cancelled() const
{
    return m_cancelled;
}

void KexiCSVExportWizard::slotCurrentPageChanged(KPageWidgetItem *page, KPageWidgetItem *prev)
{
    Q_UNUSED(prev)

    if (page == m_fileSavePage) {
        m_fileSaveWidget->setFocus();
    } else if (page == m_exportOptionsPage) {
        if (m_options.mode == KexiCSVExport::File)
            m_infoLblTo->setFileName(m_fileSaveWidget->highlightedFile());
    }

}

void KexiCSVExportWizard::next()
{
    if (currentPage() == m_fileSavePage) {
        if (!m_fileSaveWidget->checkSelectedFile()) {
            return;
        }
        kDebug() << "selectedFile:" << m_fileSaveWidget->selectedFile();
        kDebug() << "selectedUrl:" << m_fileSaveWidget->selectedUrl();
        kDebug() << "highlightedFile:" << m_fileSaveWidget->highlightedFile();
        KAssistantDialog::next();
        return;
    }
    KAssistantDialog::next();
}

void KexiCSVExportWizard::done(int result)
{
    if (QDialog::Accepted == result) {
        if (m_fileSavePage) {
            kDebug() << m_fileSaveWidget->highlightedFile();
            m_options.fileName = m_fileSaveWidget->highlightedFile();
        }
        m_options.delimiter = m_delimiterWidget->delimiter();
        m_options.textQuote = m_textQuote->textQuote();
        m_options.addColumnNames = m_addColumnNamesCheckBox->isChecked();
        if (!KexiCSVExport::exportData(*m_tableOrQuery, m_options))
            return;

        //store options
        if (m_options.mode != KexiCSVExport::Clipboard)
            writeEntry("ShowOptionsInCSVExportDialog", m_exportOptionsSection->isVisible());
        const bool store = m_alwaysUseCheckBox->isChecked();
        writeEntry("StoreOptionsForCSVExportDialog", store);
        // only save if an option differs from default

        if (store && m_delimiterWidget->delimiter() != defaultDelimiter())
            writeEntry("DefaultDelimiterForExportingCSVFiles", m_delimiterWidget->delimiter());
        else
            deleteEntry("DefaultDelimiterForExportingCSVFiles");
        if (store && m_textQuote->textQuote() != defaultTextQuote())
            writeEntry("DefaultTextQuoteForExportingCSVFiles", m_textQuote->textQuote());
        else
            deleteEntry("DefaultTextQuoteForExportingCSVFiles");
        if (store && !m_characterEncodingCombo->defaultEncodingSelected())
            writeEntry(
                "DefaultEncodingForExportingCSVFiles", m_characterEncodingCombo->selectedEncoding());
        else
            deleteEntry("DefaultEncodingForExportingCSVFiles");
        if (store && !m_addColumnNamesCheckBox->isChecked())
            writeEntry(
                "AddColumnNamesForExportingCSVFiles", m_addColumnNamesCheckBox->isChecked());
        else
            deleteEntry("AddColumnNamesForExportingCSVFiles");
    }
    else if (QDialog::Rejected == result) {
        //nothing to do
    }

    KAssistantDialog::done(result);
}

void KexiCSVExportWizard::slotShowOptionsButtonClicked()
{
    if (m_exportOptionsSection->isVisible()) {
        m_showOptionsButton->setText(i18n("Show Options >>"));
        m_exportOptionsSection->hide();
        m_alwaysUseCheckBox->hide();
        m_defaultsBtn->hide();
    } else {
        m_showOptionsButton->setText(i18n("Hide Options <<"));
        m_exportOptionsSection->show();
        m_alwaysUseCheckBox->show();
        m_defaultsBtn->show();
    }
}

void KexiCSVExportWizard::slotDefaultsButtonClicked()
{
    m_delimiterWidget->setDelimiter(defaultDelimiter());
    m_textQuote->setTextQuote(defaultTextQuote());
    m_addColumnNamesCheckBox->setChecked(true);
    m_characterEncodingCombo->selectDefaultEncoding();
}


static QString convertKey(const char *key, KexiCSVExport::Mode mode)
{
    QString _key(QString::fromLatin1(key));
    if (mode == KexiCSVExport::Clipboard) {
        _key.replace("Exporting", "Copying");
        _key.replace("Export", "Copy");
        _key.replace("CSVFiles", "CSVToClipboard");
    }
    return _key;
}

bool KexiCSVExportWizard::readBoolEntry(const char *key, bool defaultValue)
{
    return m_importExportGroup.readEntry(convertKey(key, m_options.mode), defaultValue);
}

QString KexiCSVExportWizard::readEntry(const char *key, const QString& defaultValue)
{
    return m_importExportGroup.readEntry(convertKey(key, m_options.mode), defaultValue);
}

void KexiCSVExportWizard::writeEntry(const char *key, const QString& value)
{
    m_importExportGroup.writeEntry(convertKey(key, m_options.mode), value);
}

void KexiCSVExportWizard::writeEntry(const char *key, bool value)
{
    m_importExportGroup.writeEntry(convertKey(key, m_options.mode), value);
}

void KexiCSVExportWizard::deleteEntry(const char *key)
{
    m_importExportGroup.deleteEntry(convertKey(key, m_options.mode));
}

QString KexiCSVExportWizard::defaultDelimiter() const
{
    if (m_options.mode == KexiCSVExport::Clipboard) {
        if (!m_options.forceDelimiter.isEmpty())
            return m_options.forceDelimiter;
        else
            return KEXICSV_DEFAULT_CLIPBOARD_DELIMITER;
    }
    return KEXICSV_DEFAULT_FILE_DELIMITER;
}

QString KexiCSVExportWizard::defaultTextQuote() const
{
    if (m_options.mode == KexiCSVExport::Clipboard)
        return KEXICSV_DEFAULT_CLIPBOARD_TEXT_QUOTE;
    return KEXICSV_DEFAULT_FILE_TEXT_QUOTE;
}

#include "kexicsvexportwizard.moc"
