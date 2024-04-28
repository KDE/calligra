/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_ANGLE
#define CALLIGRA_SHEETS_ACTION_ANGLE

#include "DialogCellAction.h"

namespace Calligra
{
namespace Sheets
{

class Angle : public DialogCellAction
{
    Q_OBJECT
public:
    Angle(Actions *actions);
    virtual ~Angle();

protected Q_SLOTS:
    void adjustAngle(int angle);

protected:
    QAction *createAction() override;

    ActionDialog *createDialog(QWidget *canvasWidget) override;
    void onSelectionChanged() override;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_ANGLE
