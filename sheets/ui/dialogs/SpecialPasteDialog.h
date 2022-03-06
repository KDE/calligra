/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             SPDX-FileCopyrightText: 1999-2004 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_SPECIAL_PASTE_DIALOG
#define CALLIGRA_SHEETS_SPECIAL_PASTE_DIALOG

#include <KoDialog.h>

#include "ui_SpecialPasteWidget.h"

namespace Calligra
{
namespace Sheets
{
class Selection;

/**
 * \ingroup UI
 * Dialog for the special paste action.
 */
class SpecialPasteDialog : public KoDialog, public Ui::SpecialPasteWidget
{
    Q_OBJECT
public:
    explicit SpecialPasteDialog(QWidget* parent, Selection* selection);

public Q_SLOTS:
    void slotOk();
    void slotToggled(bool);

private:
    Selection* m_selection;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_SPECIAL_PASTE_DIALOG
