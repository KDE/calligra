/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSIC_TOOL_FACTORY
#define MUSIC_TOOL_FACTORY

#include <KoToolFactoryBase.h>

class MusicToolFactory : public KoToolFactoryBase
{
public:
    MusicToolFactory();
    ~MusicToolFactory() override;

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif
