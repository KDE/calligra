/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_SUBTOTAL_DIALOG
#define CALLIGRA_SHEETS_SUBTOTAL_DIALOG

#include <KoDialog.h>

namespace Calligra
{
namespace Sheets
{
class Selection;

/**
 * \ingroup UI
 * Dialog to add subtotals.
 */
class SubtotalDialog : public KoDialog
{
    Q_OBJECT

public:
    SubtotalDialog(QWidget *parent, Selection *selection);
    ~SubtotalDialog() override;

    bool removeExisting() const;
    bool summaryBelow() const;
    bool summaryOnly() const;
    bool ignoreEmpty() const;

    QList<int> columns() const;
    int primaryColumn() const;
    int funcCode() const;

public Q_SLOTS: // reimplemented
    void accept() override;

private Q_SLOTS:
    void slotUser1();

private:
    void fillColumnBoxes();
    void fillFunctionBox();

    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_SUBTOTAL_DIALOG
