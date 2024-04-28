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

#include "ActionDialog.h"

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
class AngleDialog : public ActionDialog
{
    Q_OBJECT
public:
    AngleDialog(QWidget *parent, int angle);

    void setAngle(int angle);
    int angle();

    virtual QWidget *defaultWidget() override;

Q_SIGNALS:
    void adjustAngle(int);
public Q_SLOTS:
    virtual void onApply() override;
    void slotDefault();

protected:
    QSpinBox *m_pAngle;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ANGLE_DIALOG
