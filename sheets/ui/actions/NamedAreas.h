/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_NAMED_AREAS
#define CALLIGRA_SHEETS_ACTION_NAMED_AREAS

#include "DialogCellAction.h"

namespace Calligra
{
namespace Sheets
{

class AddNamedArea : public DialogCellAction
{
    Q_OBJECT
public:
    AddNamedArea(Actions *actions);
    virtual ~AddNamedArea();

protected Q_SLOTS:
    void addArea(const QString &name);

protected:
    virtual ActionDialog *createDialog(QWidget *canvasWidget) override;
};

class ManageNamedAreas : public DialogCellAction
{
    Q_OBJECT
public:
    ManageNamedAreas(Actions *actions);
    virtual ~ManageNamedAreas();

protected Q_SLOTS:
    void updateSelection(const Region &region, Sheet *sheet);

protected:
    virtual ActionDialog *createDialog(QWidget *canvasWidget) override;
    virtual QAction *createAction() override;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_NAMED_AREAS
