/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef INSERTTEXTREFERENCEACTION_H
#define INSERTTEXTREFERENCEACTION_H

#include "InsertInlineObjectActionBase_p.h"

class KoInlineTextObjectManager;

/**
 * helper class
 */
class InsertTextReferenceAction : public InsertInlineObjectActionBase
{
    Q_OBJECT
public:
    InsertTextReferenceAction(KoCanvasBase *canvas, const KoInlineTextObjectManager *manager);

private:
    KoInlineObject *createInlineObject() override;

    const KoInlineTextObjectManager *m_manager;
};

#endif
