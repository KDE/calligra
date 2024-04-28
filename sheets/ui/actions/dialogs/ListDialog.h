/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2001-2003 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_LIST_DIALOG
#define CALLIGRA_SHEETS_LIST_DIALOG

#include "ActionDialog.h"

namespace Calligra
{
namespace Sheets
{
class Localization;

/**
 * \ingroup UI
 * Dialog to edit custom value lists.
 */
class ListDialog : public ActionDialog
{
    Q_OBJECT
public:
    explicit ListDialog(QWidget *parent);
    ~ListDialog() override;

    void setCustomLists(const QStringList &list, Localization *locale);
    QStringList customLists();
    bool changed();

Q_SIGNALS:
    void saveChanges(const QStringList &list);

public Q_SLOTS:
    void slotDoubleClicked();
    void slotCurrentRowChanged(int row);
    void slotAdd();
    void slotCancel();
    void slotNew();
    void slotRemove();
    void slotModify();
    void slotCopy();

protected:
    virtual void onApply() override;

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_LIST_DIALOG
