/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "InsertNamedVariableAction_p.h"
#include "KoInlineTextObjectManager.h"

#include "KoVariable.h"

InsertNamedVariableAction::InsertNamedVariableAction(KoCanvasBase *canvas, const KoInlineTextObjectManager *manager, const QString &name)
    : InsertInlineObjectActionBase(canvas, name)
    , m_manager(manager)
    , m_name(name)
{
}

KoInlineObject *InsertNamedVariableAction::createInlineObject()
{
    return m_manager->variableManager()->createVariable(m_name);
}
