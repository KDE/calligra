/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DocumentSettings.h"
#include "Actions.h"
#include "dialogs/DocumentSettingsDialog.h"

#include "core/Sheet.h"

#include <KLocalizedString>
#include <QAction>

using namespace Calligra::Sheets;

DocumentSettings::DocumentSettings(Actions *actions)
    : DialogCellAction(actions,
                       "documentSettingsDialog",
                       i18n("Document Settings..."),
                       koIcon("application-vnd.oasis.opendocument.spreadsheet"),
                       i18n("Show document settings dialog"))
{
}

DocumentSettings::~DocumentSettings() = default;

ActionDialog *DocumentSettings::createDialog(QWidget *canvasWidget)
{
    return new DocumentSettingsDialog(m_selection->activeSheet()->map(), canvasWidget);
}
