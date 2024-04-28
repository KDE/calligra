/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
// SPDX-FileCopyrightText: 2005 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_INSPECTOR
#define CALLIGRA_SHEETS_ACTION_INSPECTOR

#include "DialogCellAction.h"
#include "dialogs/ActionDialog.h"

namespace Calligra
{
namespace Sheets
{

class Inspector : public DialogCellAction
{
    Q_OBJECT
public:
    Inspector(Actions *actions);
    virtual ~Inspector();

protected:
    virtual ActionDialog *createDialog(QWidget *canvasWidget) override;
    virtual void onSelectionChanged() override;

    virtual QAction *createAction() override;
};

class Cell;

class InspectorDialog : public ActionDialog
{
    Q_OBJECT
public:
    InspectorDialog(QWidget *parent);
    ~InspectorDialog() override;

    void setCell(const Cell &cell);

protected:
    virtual void onApply() override
    {
    }

private:
    Q_DISABLE_COPY(InspectorDialog)

    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_INSPECTOR
