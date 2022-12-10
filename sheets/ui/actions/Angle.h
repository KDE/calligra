/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_ANGLE
#define CALLIGRA_SHEETS_ACTION_ANGLE


#include "CellAction.h"



namespace Calligra
{
namespace Sheets
{
class AngleDialog;

class Angle : public CellAction {
Q_OBJECT
public:
    Angle(Actions *actions);
    virtual ~Angle();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    QAction *createAction() override;

    AngleDialog *m_dlg;
};


} // namespace Sheets
} // namespace Calligra

#endif   // CALLIGRA_SHEETS_ACTION_ANGLE
