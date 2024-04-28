/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999-2004 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2002-2004 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_SHOW_DIALOG
#define CALLIGRA_SHEETS_SHOW_DIALOG

#include "../sheets_ui_export.h"
#include <KoDialog.h>

class QListWidget;

namespace Calligra
{
namespace Sheets
{
class Selection;

/**
 * \ingroup UI
 * Dialog to show a hidden sheet.
 * \todo Rename to ShowSheetDialog.
 */
class CALLIGRA_SHEETS_UI_EXPORT ShowDialog : public KoDialog
{
    Q_OBJECT
public:
    ShowDialog(QWidget *parent, Selection *selection);

public Q_SLOTS:
    void accept() override;

private:
    Selection *m_selection;
    QListWidget *m_listWidget;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_SHOW_DIALOG
