/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999-2003 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2003 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2003 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 1999 Stephan Kulow <coolo@kde.org>
             SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_GOTO_DIALOG
#define CALLIGRA_SHEETS_GOTO_DIALOG

#include <KoDialog.h>

class KComboBox;

namespace Calligra
{
namespace Sheets
{
class Selection;

/**
 * \ingroup UI
 * Dialog to go to a specific cell location.
 */
class GotoDialog : public KoDialog
{
    Q_OBJECT
public:
    GotoDialog(QWidget* parent, Selection* selection);

public Q_SLOTS:
    void slotOk();
    void textChanged(const QString &_text);


protected:
    Selection* m_selection;
    KComboBox* m_nameCell;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_GOTO_DIALOG
