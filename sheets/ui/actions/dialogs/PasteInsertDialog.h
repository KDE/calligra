/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2000-2002 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_PASTE_INSERT_DIALOG
#define CALLIGRA_SHEETS_PASTE_INSERT_DIALOG

#include <KoDialog.h>

class QRadioButton;

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup UI
 * Dialog to select the direction for the paste with insertion action.
 */
class PasteInsertDialog : public KoDialog
{
    Q_OBJECT
public:
    PasteInsertDialog(QWidget *parent);

    bool checkedRight() const;
    bool checkedBottom() const;
public Q_SLOTS:
    void slotOk();

private:
    QRadioButton *rb1;
    QRadioButton *rb2;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_PASTE_INSERT_DIALOG
