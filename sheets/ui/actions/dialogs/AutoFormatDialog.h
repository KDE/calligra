/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2002 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2000-2002 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_AUTO_FORMAT_DIALOG
#define CALLIGRA_SHEETS_AUTO_FORMAT_DIALOG

#include "ActionDialog.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup UI
 * Dialog for the "Auto-Format..." action.
 */
class AutoFormatDialog : public ActionDialog
{
    Q_OBJECT
public:
    explicit AutoFormatDialog(QWidget *parent);
    ~AutoFormatDialog() override;

    void setList(const QMap<QString, QPixmap> &list);
    QString selectedOption();
Q_SIGNALS:
    void applyFormat(const QString &format);
private Q_SLOTS:
    void slotActivated(int index);

protected:
    virtual void onApply() override;

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_AUTO_FORMAT_DIALOG
