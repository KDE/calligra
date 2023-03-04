/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DocumentSettings.h"
#include "Actions.h"
#include "dialogs/DocumentSettingsDialog.h"

#include "core/Sheet.h"

#include <QAction>
#include <KLocalizedString>


using namespace Calligra::Sheets;



DocumentSettings::DocumentSettings(Actions *actions)
    : CellAction(actions, "documentSettingsDialog", i18n("Document Settings..."), koIcon("application-vnd.oasis.opendocument.spreadsheet"), i18n("Show document settings dialog"))
    , m_dlg(nullptr)
{
}

DocumentSettings::~DocumentSettings()
{
    if (m_dlg) delete m_dlg;
}

void DocumentSettings::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    // TODO - make this non-modal, save on each change
    m_dlg = new DocumentSettingsDialog(sheet->map(), canvasWidget);
    m_dlg->exec();
    delete m_dlg;
    m_dlg = nullptr;
}




