/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_TOOL_REGISTRY
#define CALLIGRA_SHEETS_TOOL_REGISTRY

#include <QObject>

#include "sheets_part_export.h"

namespace Calligra
{
namespace Sheets
{

/**
 * Registry for tools.
 * \ingroup Plugin
 */
class CALLIGRA_SHEETS_PART_EXPORT ToolRegistry : public QObject
{
public:
    /**
     * Creates the registry.
     */
    ToolRegistry();
    ~ToolRegistry() override;

    /**
     * \return the singleton instance
     */
    static ToolRegistry *instance();

    /**
     * Loads the tools.
     * Depending on their activation state read from the config,
     * the tools are added or removed from the registry.
     */
    void loadTools();

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TOOL_REGISTRY
