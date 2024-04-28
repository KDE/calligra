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

#include "ActionDialog.h"

class QComboBox;

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup UI
 * Dialog to go to a specific cell location.
 */
class GotoDialog : public ActionDialog
{
    Q_OBJECT
public:
    GotoDialog(QWidget *parent, const QList<QString> &choices);

Q_SIGNALS:
    void gotoCell(const QString &);
public Q_SLOTS:
    virtual QWidget *defaultWidget() override;
    virtual void onApply() override;
    void textChanged(const QString &_text);

protected:
    QComboBox *m_nameCell;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_GOTO_DIALOG
