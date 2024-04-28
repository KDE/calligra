/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef INSERTTEXTLOCATOR_H
#define INSERTTEXTLOCATOR_H

#include "InsertInlineObjectActionBase_p.h"

/**
 * helper class
 */
class InsertTextLocator : public InsertInlineObjectActionBase
{
    Q_OBJECT
public:
    explicit InsertTextLocator(KoCanvasBase *canvas);

private:
    KoInlineObject *createInlineObject() override;
};

#endif
