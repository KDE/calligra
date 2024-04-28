/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_COPY
#define CALLIGRA_SHEETS_ACTION_COPY

// Copy and Cut

#include "CellAction.h"

namespace Calligra
{
namespace Sheets
{

class Copy : public CellAction
{
    Q_OBJECT
public:
    Copy(Actions *actions);
    virtual ~Copy();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    QAction *createAction() override;

    virtual bool enabledIfReadOnly() const override
    {
        return true;
    }
    virtual bool enabledIfProtected() const override
    {
        return true;
    }
};

class Cut : public CellAction
{
    Q_OBJECT
public:
    Cut(Actions *actions);
    virtual ~Cut();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    QAction *createAction() override;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_COPY
