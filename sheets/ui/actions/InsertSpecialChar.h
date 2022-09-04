/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_INSERT_SPECIAL_CHAR
#define CALLIGRA_SHEETS_ACTION_INSERT_SPECIAL_CHAR


#include "CellAction.h"

namespace Calligra
{
namespace Sheets
{
class CharacterSelectDialog;

class InsertSpecialChar : public CellAction {
Q_OBJECT
public:
    InsertSpecialChar(Actions *actions);
    virtual ~InsertSpecialChar();

protected Q_SLOTS:
    void specialCharDialogClosed();
    void specialChar(QChar character, const QString& fontName);

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;

    CharacterSelectDialog *m_dlg;
    Selection *m_selection;
};


} // namespace Sheets
} // namespace Calligra

#endif   // CALLIGRA_SHEETS_ACTION_INSERT_SPECIAL_CHAR
