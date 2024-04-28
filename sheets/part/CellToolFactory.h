/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CELL_TOOL_FACTORY
#define CALLIGRA_SHEETS_CELL_TOOL_FACTORY

#include <KoToolFactoryBase.h>

#include "sheets_part_export.h"

namespace Calligra
{
namespace Sheets
{

/**
 * The Factory, that creates a CellTool.
 */
class CALLIGRA_SHEETS_PART_EXPORT CellToolFactory : public KoToolFactoryBase
{
public:
    explicit CellToolFactory(const QString &id);
    ~CellToolFactory() override;

    KoToolBase *createTool(KoCanvasBase *canvas) override;

    void setPriority(int priority);
    void setToolTip(const QString &toolTip);
    void setIconName(const char *iconName);
    void setIconName(const QString &iconName);
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CELL_TOOL_FACTORY
