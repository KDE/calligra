/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "CustomLists.h"
#include "Actions.h"
#include "dialogs/ListDialog.h"

#include "engine/CalculationSettings.h"
#include "engine/Localization.h"
#include "core/ApplicationSettings.h"
#include "core/Map.h"
#include "core/Sheet.h"
#include "ui/commands/AutoFillCommand.h"

#include <QAction>
#include <KLocalizedString>
#include <KSharedConfig>


using namespace Calligra::Sheets;



ManageCustomLists::ManageCustomLists(Actions *actions)
    : CellAction(actions, "sortList", i18n("Custom Lists..."), QIcon(), i18n("Create custom lists for sorting or autofill"))
    , m_dlg(nullptr)
{
}

ManageCustomLists::~ManageCustomLists()
{
    if (m_dlg) delete m_dlg;
}

void ManageCustomLists::execute(Selection *, Sheet *sheet, QWidget *canvasWidget)
{
    // TODO - make this non-modal, save on each change
    if (!m_dlg) m_dlg = new ListDialog(canvasWidget);
    Map *map = sheet->fullMap();
    ApplicationSettings *sett = map->applicationSettings();
    Localization *locale = map->calculationSettings()->locale();
    m_dlg->setCustomLists(sett->sortingList(), locale);
    if (m_dlg->exec() && m_dlg->changed()) {
        QStringList result = m_dlg->customLists();
        sett->setSortingList(result);
        auto config = KSharedConfig::openConfig();
        config->group("Parameters").writeEntry("Other list", result);

        // TODO do this better
        delete(AutoFillCommand::other);
        AutoFillCommand::other = nullptr;
    }
    delete m_dlg;
    m_dlg = nullptr;
}




