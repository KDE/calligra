/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2004 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2000-2003 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2001-2002 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 2001 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 2000 Bernd Johannes Wuebben <wuebben@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_PREFERENCE_DIALOG
#define CALLIGRA_SHEETS_PREFERENCE_DIALOG

#include <kpagedialog.h>

namespace Sonnet
{
}

namespace Calligra
{
namespace Sheets
{
class View;

/**
 * \ingroup UI
 * Dialog to set the application preferences.
 */
class PreferenceDialog : public KPageDialog
{
    Q_OBJECT

public:
    enum { InterfacePage = 2, OpenSavePage = 4, SpellCheckerPage = 8, PluginPage = 16 };

    explicit PreferenceDialog(View *view);
    ~PreferenceDialog() override;

    void openPage(int flags);

public Q_SLOTS:
    void slotApply();
    void slotDefault();
    void slotReset();

private Q_SLOTS:
    void unitChanged(int index);

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_PREFERENCE_DIALOG
