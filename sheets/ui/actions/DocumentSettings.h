/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_DOCUMENT_SETTINGS
#define CALLIGRA_SHEETS_ACTION_DOCUMENT_SETTINGS

#include "DialogCellAction.h"

namespace Calligra
{
namespace Sheets
{

class DocumentSettings : public DialogCellAction
{
    Q_OBJECT
public:
    DocumentSettings(Actions *actions);
    virtual ~DocumentSettings();

protected:
    virtual ActionDialog *createDialog(QWidget *canvasWidget) override;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_DOCUMENT_SETTINGS
