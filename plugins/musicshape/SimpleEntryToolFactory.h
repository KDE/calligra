/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SIMPLEENTRY_TOOL_FACTORY
#define SIMPLEENTRY_TOOL_FACTORY

#include <KoToolFactoryBase.h>

class SimpleEntryToolFactory : public KoToolFactoryBase
{
public:
    SimpleEntryToolFactory();
    ~SimpleEntryToolFactory() override;

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif
