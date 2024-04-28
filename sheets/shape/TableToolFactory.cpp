/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "TableToolFactory.h"
#include "TableShape.h"
#include "TableTool.h"

#include <KLocalizedString>

#include <KoIcon.h>

using namespace Calligra::Sheets;

TableToolFactory::TableToolFactory()
    : KoToolFactoryBase("TableToolFactoryId")
{
    setToolTip(i18n("Table editing"));
    setIconName(koIconName("spreadsheetshape"));
    setToolType(dynamicToolType());
    setPriority(1);
    setActivationShapeId(TableShapeId);
}

TableToolFactory::~TableToolFactory()
{
}

KoToolBase *TableToolFactory::createTool(KoCanvasBase *canvas)
{
    return new TableTool(canvas);
}
