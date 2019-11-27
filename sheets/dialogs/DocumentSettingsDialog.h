/* This file is part of the KDE project
   Copyright (C) 2002-2004 Ariya Hidayat <ariya@kde.org>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2000-2003 Laurent Montel <montel@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2001-2002 David Faure <faure@kde.org>
             (C) 2001 Werner Trobin <trobin@kde.org>
             (C) 2000 Bernd Johannes Wuebben <wuebben@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef CALLIGRA_SHEETS_DOCUMENT_SETTINGS_DIALOG
#define CALLIGRA_SHEETS_DOCUMENT_SETTINGS_DIALOG

#include <QGroupBox>
#include <QLabel>

#include <kpagedialog.h>

class KoVBox;
class KLocale;
class QCheckBox;
class QSpinBox;
class QComboBox;
class QPushButton;

namespace Sonnet
{
}

namespace Calligra
{
namespace Sheets
{
class Selection;
class CalculationSettings;

/**
 * \ingroup UI
 * Dialog page to show the calculation settings used by the document.
 */
class calcSettings :  public QObject
{
    Q_OBJECT
public:
    calcSettings(Selection* selection, KoVBox *box);
    void apply();
protected:
    CalculationSettings *m_cs;
    QCheckBox *m_caseSensitiveCheckbox, *m_precisionAsShownCheckbox, *m_searchCriteriaMustApplyToWholeCellCheckbox, *m_automaticFindLabelsCheckbox;
    QComboBox *m_matchModeCombobox;
    QSpinBox *m_nullYearEdit;
};

/**
 * \ingroup UI
 * Dialog page to show the current locale used by the document.
 */
class parameterLocale :  public QObject
{
    Q_OBJECT
public:
    parameterLocale(Selection* selection, KoVBox *box);
    void apply();
public Q_SLOTS:
    void updateDefaultSystemConfig();
protected:
    /**
     * Updates the GUI widgets to match the specified locale.
     */
    void updateToMatchLocale(KLocale* locale);

    QLabel *m_shortDate, *m_time, *m_money, *m_date, *m_language, *m_number;
    QPushButton *m_updateButton;
    Selection* m_selection;
    bool m_bUpdateLocale;
};


/**
 * \ingroup UI
 * Dialog to alter document settings.
 */
class DocumentSettingsDialog : public KPageDialog
{
    Q_OBJECT
public:
    explicit DocumentSettingsDialog(Selection* selection, QWidget* parent);
    ~DocumentSettingsDialog() override;

public Q_SLOTS:
    void slotApply();
    void slotDefault();
    void slotReset();

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_DOCUMENT_SETTINGS_DIALOG
