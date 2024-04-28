/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Montel Laurent <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef VECTOR_TOOL_FACTORY
#define VECTOR_TOOL_FACTORY

#include <KoToolFactoryBase.h>

class VectorToolFactory : public KoToolFactoryBase
{
public:
    VectorToolFactory();
    ~VectorToolFactory() override;

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif
