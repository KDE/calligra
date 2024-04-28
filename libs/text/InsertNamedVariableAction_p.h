/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef INSERTNAMEDVARIABLEACTION_H
#define INSERTNAMEDVARIABLEACTION_H

#include "InsertInlineObjectActionBase_p.h"

class KoInlineTextObjectManager;

/**
 * helper class
 */
class InsertNamedVariableAction : public InsertInlineObjectActionBase
{
    Q_OBJECT
public:
    InsertNamedVariableAction(KoCanvasBase *canvas, const KoInlineTextObjectManager *manager, const QString &name);

private:
    KoInlineObject *createInlineObject() override;

    const KoInlineTextObjectManager *m_manager;
    QString m_name;
};

#endif
