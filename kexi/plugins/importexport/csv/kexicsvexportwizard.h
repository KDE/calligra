/* This file is part of the KDE project
   Copyright (C) 2005,2006 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXI_CSVEXPORTWIZARD_H
#define KEXI_CSVEXPORTWIZARD_H

#include <k3wizard.h>
#include <KConfigGroup>

#include "kexicsvexport.h"

//Added by qt3to4:
#include <Q3HBoxLayout>

class QCheckBox;
class Q3GroupBox;
class KPushButton;
class KexiStartupFileWidget;
class KexiCSVDelimiterWidget;
class KexiCSVTextQuoteComboBox;
class KexiCSVInfoLabel;
class KexiCharacterEncodingComboBox;
namespace KexiDB
{
class TableOrQuerySchema;
}

/*! @short Kexi CSV export wizard
 Supports exporting to a file and to a clipboard. */
class KexiCSVExportWizard : public K3Wizard
{
    Q_OBJECT

public:
    KexiCSVExportWizard(const KexiCSVExport::Options& options,
                        QWidget * parent = 0);

    virtual ~KexiCSVExportWizard();

    bool cancelled() const;

    virtual void showPage(QWidget * page);

protected slots:
    virtual void next();
    virtual void done(int result);
    void slotShowOptionsButtonClicked();
    void slotDefaultsButtonClicked();

protected:
    //! reimplemented to add "Defaults" button on the left hand
    virtual void layOutButtonRow(QHBoxLayout * layout);

    //! \return default delimiter depending on mode.
    QString defaultDelimiter() const;

    //! \return default text quote depending on mode.
    QString defaultTextQuote() const;

    //! Helper, works like KGlobal::config()->readBoolEntry(const char*, bool) but if mode is Clipboard,
    //! "Exporting" is replaced with "Copying" and "Export" is replaced with "Copy"
    //! and "CSVFiles" is replaced with "CSVToClipboard"
    //! in \a key, to keep the setting separate.
    bool readBoolEntry(const char *key, bool defaultValue);

    //! Helper like \ref readBoolEntry(const char *, bool), but for QString values.
    QString readEntry(const char *key, const QString& defaultValue = QString());

    //! Helper, works like KGlobal::config()->writeEntry(const char*,bool) but if mode is Clipboard,
    //! "Exporting" is replaced with "Copying" and "Export" is replaced with "Copy"
    //! and "CSVFiles" is replaced with "CSVToClipboard"
    //! in \a key, to keep the setting separate.
    void writeEntry(const char *key, bool value);

    //! Helper like \ref writeEntry(const char *, bool), but for QString values.
    void writeEntry(const char *key, const QString& value);

    //! Helper like \ref writeEntry(const char *, bool), but for deleting config entry.
    void deleteEntry(const char *key);

    KexiCSVExport::Options m_options;
//  Mode m_mode;
//  int m_itemId;
    KexiStartupFileWidget* m_fileSavePage;
    QWidget* m_exportOptionsPage;
    KPushButton *m_showOptionsButton;
    KPushButton *m_defaultsBtn;
    Q3GroupBox* m_exportOptionsSection;
    KexiCSVInfoLabel *m_infoLblFrom, *m_infoLblTo;
    KexiCSVDelimiterWidget* m_delimiterWidget;
    KexiCSVTextQuoteComboBox* m_textQuote;
    KexiCharacterEncodingComboBox *m_characterEncodingCombo;
    QCheckBox* m_addColumnNamesCheckBox, *m_alwaysUseCheckBox;
    KexiDB::TableOrQuerySchema* m_tableOrQuery;
    KConfigGroup m_importExportGroup;
    int m_rowCount; //!< Cached row count for a table/query.
bool m_rowCountDetermined : 1;
bool m_cancelled : 1;
};

#endif
