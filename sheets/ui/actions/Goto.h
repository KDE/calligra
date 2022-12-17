/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_GOTO
#define CALLIGRA_SHEETS_ACTION_GOTO


#include "CellAction.h"



namespace Calligra
{
namespace Sheets
{
class GotoDialog;

class Goto : public CellAction {
Q_OBJECT
public:
    Goto(Actions *actions);
    virtual ~Goto();

protected Q_SLOTS:
    void gotoCell(const QString &name);
    void dialogFinished();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    QAction *createAction() override;

    virtual bool enabledIfReadOnly() const override { return true; }
    virtual bool enabledIfProtected() const override { return true; }

    Selection *m_selection;
    QWidget *m_canvasWidget;
    GotoDialog *m_dlg;
};


} // namespace Sheets
} // namespace Calligra

#endif   // CALLIGRA_SHEETS_ACTION_GOTO
