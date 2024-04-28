/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_INSERT_SPECIAL_CHAR
#define CALLIGRA_SHEETS_ACTION_INSERT_SPECIAL_CHAR

#include "DialogCellAction.h"

namespace Calligra
{
namespace Sheets
{
class InsertSpecialChar : public DialogCellAction
{
    Q_OBJECT
public:
    InsertSpecialChar(Actions *actions);
    virtual ~InsertSpecialChar();

protected Q_SLOTS:
    void specialChar(QChar character, const QString &fontName);

protected:
    virtual ActionDialog *createDialog(QWidget *canvasWidget) override;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_INSERT_SPECIAL_CHAR
