/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_DOCUMENT_SETTINGS
#define CALLIGRA_SHEETS_ACTION_DOCUMENT_SETTINGS


#include "CellAction.h"


namespace Calligra
{
namespace Sheets
{

class DocumentSettingsDialog;

class DocumentSettings : public CellAction {
Q_OBJECT
public:
    DocumentSettings(Actions *actions);
    virtual ~DocumentSettings();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    
    DocumentSettingsDialog *m_dlg;
};



} // namespace Sheets
} // namespace Calligra

#endif   // CALLIGRA_SHEETS_ACTION_DOCUMENT_SETTINGS
