/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_TABLE_TOOL_FACTORY
#define CALLIGRA_SHEETS_TABLE_TOOL_FACTORY

#include <KoToolFactoryBase.h>

namespace Calligra
{
namespace Sheets
{

class TableToolFactory : public KoToolFactoryBase
{
public:
    TableToolFactory();
    ~TableToolFactory();

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TABLE_TOOL_FACTORY
