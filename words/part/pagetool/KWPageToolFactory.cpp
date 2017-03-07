/*
 *  Copyright (C) 2011 Jignesh Kakadiya <jigneshhk1992@gmail.com>
 *  
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
*/

#include "KWPageToolFactory.h"
#include "KWPageTool.h"
#include <KoCanvasBase.h>
#include <KoIcon.h>

#include <klocalizedstring.h>

KWPageToolFactory::KWPageToolFactory()
    : KoToolFactoryBase("PageToolFactory_ID")
{
    setToolTip(i18n("Page layout"));
    setToolType(dynamicToolType()+",calligrawords, calligraauthor");
    setIconName(koIconName("tool_pagelayout"));
    setPriority(25);
    setActivationShapeId("flake/always");
}

KWPageToolFactory::~KWPageToolFactory()
{
}

KoToolBase* KWPageToolFactory::createTool(KoCanvasBase *canvas)
{
    return new KWPageTool(canvas);
}

