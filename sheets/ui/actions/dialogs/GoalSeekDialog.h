/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2002 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 2002 Harri Porten <porten@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_GOALSEEK_DIALOG
#define CALLIGRA_SHEETS_GOALSEEK_DIALOG

#include <KoDialog.h>

class QCloseEvent;

namespace Calligra
{
namespace Sheets
{
class Selection;
class RegionSelector;

/**
 * \ingroup UI
 * Dialog to seek a specific value for a set of parameter values.
 */
class GoalSeekDialog : public KoDialog
{
    Q_OBJECT

public:
    GoalSeekDialog(QWidget *parent, Selection *selection);
    ~GoalSeekDialog() override;

    QString selectorValue(int id);
    void focusSelector(int id);
    void setNotice(const QString &text);

Q_SIGNALS:
    void calculate();

protected:
    void closeEvent(QCloseEvent *) override;
    RegionSelector *getSelector(int id);

protected Q_SLOTS:
    void textChanged();
    void slotStart();
    void slotClose();

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_GOALSEEK_DIALOG
