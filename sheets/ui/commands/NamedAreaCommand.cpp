/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "NamedAreaCommand.h"

#include "core/Sheet.h"
#include "engine/Damages.h"
#include "engine/FormulaStorage.h"
#include "engine/MapBase.h"
#include "engine/NamedAreaManager.h"

using namespace Calligra::Sheets;

NamedAreaCommand::NamedAreaCommand(KUndo2Command *parent)
    : AbstractRegionCommand(parent)
    , m_remove(false)
{
    setText(kundo2_i18n("Add Named Area"));
}

NamedAreaCommand::~NamedAreaCommand() = default;

void NamedAreaCommand::setAreaName(const QString &name)
{
    m_areaName = name;
}

void NamedAreaCommand::setNewAreaName(const QString &name)
{
    m_newAreaName = name;
}

void NamedAreaCommand::setRemove(bool remove)
{
    m_remove = remove;
    if (!m_remove)
        setText(kundo2_i18n("Add Named Area"));
    else
        setText(kundo2_i18n("Remove Named Area"));
}

bool NamedAreaCommand::preProcess()
{
    if (!m_firstrun)
        return true;

    const Region namedArea = m_sheet->map()->namedAreaManager()->namedArea(m_areaName);
    if (!namedArea.isEmpty())
        m_oldArea = namedArea;
    if (m_remove)
        return true;
    // no protection or matrix lock check needed
    return isContiguous();
}

bool NamedAreaCommand::performNonCommandActions()
{
    NamedAreaManager *manager = m_sheet->map()->namedAreaManager();

    if (!m_remove) {
        const Region origArea = manager->namedArea(m_areaName);
        if (!m_oldArea.isEmpty())
            manager->remove(m_areaName);

        QString name = m_newAreaName;
        if (!name.length())
            name = m_areaName;
        manager->insert(*this, name);
    } else {
        QString name = m_newAreaName;
        if (!name.length())
            name = m_areaName;
        manager->remove(name);
        if (!m_oldArea.isEmpty())
            manager->insert(m_oldArea, m_areaName);
    }
    return true;
}

bool NamedAreaCommand::undoNonCommandActions()
{
    m_remove = !m_remove;
    performNonCommandActions();
    m_remove = !m_remove;
    return true;
}
