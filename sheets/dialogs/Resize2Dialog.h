/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2004 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2000 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_RESIZE2_DIALOG
#define CALLIGRA_SHEETS_RESIZE2_DIALOG

#include <KoDialog.h>

class KoUnitDoubleSpinBox;

namespace Calligra
{
namespace Sheets
{
class Selection;

/**
 * \ingroup UI
 * Dialog to resize rows.
 */
class ResizeRow: public KoDialog
{
    Q_OBJECT

public:
    explicit ResizeRow(QWidget* parent, Selection* selection);
    double rowHeight;

protected Q_SLOTS:
    virtual void slotOk();
    virtual void slotDefault();

protected:
    Selection* m_selection;
    KoUnitDoubleSpinBox *m_pHeight;
};

/**
 * \ingroup UI
 * Dialog to resize columns.
 */
class ResizeColumn: public KoDialog
{
    Q_OBJECT

public:
    explicit ResizeColumn(QWidget* parent, Selection* selection);
    double columnWidth;

protected Q_SLOTS:
    virtual void slotOk();
    virtual void slotDefault();

protected:
    Selection* m_selection;
    KoUnitDoubleSpinBox *m_pWidth;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_RESIZE2_DIALOG
