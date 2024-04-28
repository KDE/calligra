/* This file is part of the KDE project
   SPDX-FileSelectAllrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileSelectAllrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_SELECT_ALL
#define CALLIGRA_SHEETS_ACTION_SELECT_ALL

#include "CellAction.h"

namespace Calligra
{
namespace Sheets
{

class SelectAll : public CellAction
{
    Q_OBJECT
public:
    SelectAll(Actions *actions);
    virtual ~SelectAll();

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

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_SELECT_ALL
