/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ANGLE_DIALOG
#define CALLIGRA_SHEETS_ANGLE_DIALOG

#include <KoDialog.h>

class QSpinBox;

namespace Calligra
{
namespace Sheets
{
class Selection;

/**
 * \ingroup UI
 * Dialog to rotate the cell contents.
 */
class AngleDialog : public KoDialog
{
    Q_OBJECT
public:
    AngleDialog(QWidget* parent, Selection* selection);

public Q_SLOTS:
    void slotOk();
    void slotDefault();
protected:
    Selection* m_selection;
    QSpinBox *m_pAngle;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ANGLE_DIALOG
