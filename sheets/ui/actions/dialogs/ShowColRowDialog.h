/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2000-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_SHOW_COL_ROW_DIALOG
#define CALLIGRA_SHEETS_SHOW_COL_ROW_DIALOG

#include <QList>

#include <KoDialog.h>

class QListWidget;
class QListWidgetItem;

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup UI
 * Dialog to show hidden columns/rows.
 */
class ShowColRowDialog : public KoDialog
{
    Q_OBJECT
public:
    ShowColRowDialog(QWidget *parent, bool rows);

    void setList(const QVector<QString> &captions);
    QVector<int> selectedIndexes() const;

public Q_SLOTS:
    void slotOk();
    void slotDoubleClicked(QListWidgetItem *);

protected:
    QListWidget *list;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_SHOW_COL_ROW_DIALOG
