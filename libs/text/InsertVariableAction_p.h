/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef INSERTVARIABLEACTION_H
#define INSERTVARIABLEACTION_H

#include "InsertInlineObjectActionBase_p.h"

class KoProperties;
class KoInlineObjectFactoryBase;
struct KoInlineObjectTemplate;

/// \internal
class InsertVariableAction : public InsertInlineObjectActionBase
{
    Q_OBJECT
public:
    InsertVariableAction(KoCanvasBase *base, KoInlineObjectFactoryBase *factory, const KoInlineObjectTemplate &templ);

private:
    KoInlineObject *createInlineObject() override;

    KoInlineObjectFactoryBase *const m_factory;
    const QString m_templateId;
    const KoProperties *const m_properties;
    QString m_templateName;
};

#endif
