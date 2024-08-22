/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "CustomLists.h"
#include "Actions.h"
#include "dialogs/ListDialog.h"

#include "core/ApplicationSettings.h"
#include "core/Map.h"
#include "core/Sheet.h"
#include "engine/CalculationSettings.h"
#include "engine/Localization.h"
#include "ui/commands/AutoFillCommand.h"

#include <KLocalizedString>
#include <KSharedConfig>
#include <QAction>

using namespace Calligra::Sheets;

ManageCustomLists::ManageCustomLists(Actions *actions)
    : DialogCellAction(actions, "sortList", i18n("Custom Lists..."), QIcon(), i18n("Create custom lists for sorting or autofill"))
{
}

ManageCustomLists::~ManageCustomLists() = default;

ActionDialog *ManageCustomLists::createDialog(QWidget *canvasWidget)
{
    ListDialog *dlg = new ListDialog(canvasWidget);

    Map *map = m_selection->activeSheet()->fullMap();
    ApplicationSettings *sett = map->applicationSettings();
    Localization *locale = map->calculationSettings()->locale();
    dlg->setCustomLists(sett->sortingList(), locale);
    return dlg;
}

void ManageCustomLists::saveChanges(const QStringList &list)
{
    Map *map = m_selection->activeSheet()->fullMap();
    ApplicationSettings *sett = map->applicationSettings();
    sett->setSortingList(list);
    auto config = KSharedConfig::openConfig();
    config->group("Parameters").writeEntry("Other list", list);

    // TODO do this better
    delete (AutoFillCommand::other);
    AutoFillCommand::other = nullptr;
}
