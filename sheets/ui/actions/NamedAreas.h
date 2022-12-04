/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_NAMED_AREAS
#define CALLIGRA_SHEETS_ACTION_NAMED_AREAS


#include "CellAction.h"


namespace Calligra
{
namespace Sheets
{

class AddNamedAreaDialog;
class NamedAreaDialog;

class AddNamedArea : public CellAction {
Q_OBJECT
public:
    AddNamedArea(Actions *actions);
    virtual ~AddNamedArea();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;

    AddNamedAreaDialog *m_dlg;
    Selection *m_selection;
};

class ManageNamedAreas : public CellAction {
Q_OBJECT
public:
    ManageNamedAreas(Actions *actions);
    virtual ~ManageNamedAreas();

protected Q_SLOTS:
    void dialogClosed();
protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual QAction *createAction() override;
    
    NamedAreaDialog *m_dlg;
};



} // namespace Sheets
} // namespace Calligra

#endif   // CALLIGRA_SHEETS_ACTION_NAMED_AREAS
