/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTTOOLFACTORY_H
#define KOTEXTTOOLFACTORY_H

#include <KoToolFactoryBase.h>

class TextToolFactory : public KoToolFactoryBase
{
public:
    TextToolFactory();
    ~TextToolFactory() override;

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif
