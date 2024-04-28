/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Montel Laurent <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PICTURE_TOOL_FACTORY
#define PICTURE_TOOL_FACTORY

#include <KoToolFactoryBase.h>

class PictureToolFactory : public KoToolFactoryBase
{
public:
    PictureToolFactory();
    ~PictureToolFactory() override;

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif
