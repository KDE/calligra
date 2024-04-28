/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_GOTO
#define CALLIGRA_SHEETS_ACTION_GOTO

#include "DialogCellAction.h"

namespace Calligra
{
namespace Sheets
{

class Goto : public DialogCellAction
{
    Q_OBJECT
public:
    Goto(Actions *actions);
    virtual ~Goto();

protected Q_SLOTS:
    void gotoCell(const QString &name);

protected:
    virtual ActionDialog *createDialog(QWidget *canvasWidget) override;
    QAction *createAction() override;

    virtual bool enabledIfReadOnly() const override
    {
        return true;
    }
    virtual bool enabledIfProtected() const override
    {
        return true;
    }

    QWidget *m_canvasWidget;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_GOTO
