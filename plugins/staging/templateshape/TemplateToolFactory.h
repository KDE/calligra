/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TEMPLATE_TOOL_FACTORY
#define TEMPLATE_TOOL_FACTORY

#include <KoToolFactoryBase.h>

class TemplateToolFactory : public KoToolFactoryBase
{
public:
    TemplateToolFactory();
    ~TemplateToolFactory();

    KoToolBase *createTool(KoCanvasBase *canvas);
};

#endif
