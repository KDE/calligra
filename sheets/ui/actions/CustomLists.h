/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_CUSTOM_LISTS
#define CALLIGRA_SHEETS_ACTION_CUSTOM_LISTS


#include "CellAction.h"


namespace Calligra
{
namespace Sheets
{

class ListDialog;

class ManageCustomLists : public CellAction {
Q_OBJECT
public:
    ManageCustomLists(Actions *actions);
    virtual ~ManageCustomLists();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    
    ListDialog *m_dlg;
};



} // namespace Sheets
} // namespace Calligra

#endif   // CALLIGRA_SHEETS_ACTION_CUSTOM_LISTS
