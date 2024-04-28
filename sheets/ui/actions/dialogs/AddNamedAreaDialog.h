/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 1999-2001 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ADD_NAMED_AREA_DIALOG
#define CALLIGRA_SHEETS_ADD_NAMED_AREA_DIALOG

#include "ActionDialog.h"

class QLineEdit;

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup UI
 * Dialog to add a named area.
 */
class AddNamedAreaDialog : public ActionDialog
{
    Q_OBJECT

public:
    AddNamedAreaDialog(QWidget *parent);

    QString areaName() const;

Q_SIGNALS:
    void addArea(const QString &);

public Q_SLOTS:
    void slotAreaNameChanged(const QString &name);

protected:
    virtual void onApply() override;

    QLineEdit *m_areaName;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ADD_NAMED_AREA_DIALOG
