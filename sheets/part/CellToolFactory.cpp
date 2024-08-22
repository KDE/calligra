/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "CellToolFactory.h"
#include "CellTool.h"

#include <KLocalizedString>

#include <KoIcon.h>

using namespace Calligra::Sheets;

CellToolFactory::CellToolFactory(const QString &id)
    : KoToolFactoryBase(id)
{
    setToolTip(i18n("Cell formatting"));
    setIconName(koIconName("tool_cellformatting"));
    setToolType(dynamicToolType() + ",calligrasheets");
    setPriority(0);
    setActivationShapeId("flake/always");
}

CellToolFactory::~CellToolFactory() = default;

KoToolBase *CellToolFactory::createTool(KoCanvasBase *canvas)
{
    return new CellTool(canvas);
}

void CellToolFactory::setPriority(int priority)
{
    KoToolFactoryBase::setPriority(priority);
}

void CellToolFactory::setToolTip(const QString &toolTip)
{
    KoToolFactoryBase::setToolTip(toolTip);
}

void CellToolFactory::setIconName(const char *iconName)
{
    KoToolFactoryBase::setIconName(iconName);
}

void CellToolFactory::setIconName(const QString &iconName)
{
    KoToolFactoryBase::setIconName(iconName);
}
