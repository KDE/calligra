/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
// SPDX-FileCopyrightText: 2005 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_INSPECTOR
#define CALLIGRA_SHEETS_ACTION_INSPECTOR


#include "CellAction.h"
#include <kpagedialog.h>


namespace Calligra
{
namespace Sheets
{

class Inspector : public CellAction {
Q_OBJECT
public:
    Inspector(Actions *actions);
    virtual ~Inspector();
protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual QAction *createAction() override;
};

class Cell;

class InspectorDialog : public KPageDialog
{
    Q_OBJECT
public:
    InspectorDialog(const Cell& cell);
    ~InspectorDialog() override;

private:
    Q_DISABLE_COPY(InspectorDialog)

    class Private;
    Private * const d;
};



} // namespace Sheets
} // namespace Calligra

#endif   // CALLIGRA_SHEETS_ACTION_INSPECTOR
