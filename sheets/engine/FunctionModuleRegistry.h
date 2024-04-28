// This file is part of the KDE project
// SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef CALLIGRA_SHEETS_FUNCTION_MODULE_REGISTRY
#define CALLIGRA_SHEETS_FUNCTION_MODULE_REGISTRY

#include <KoGenericRegistry.h>

#include "sheets_engine_export.h"

#include "FunctionModule.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Plugin
 * \ingroup Value
 * Registry for function modules.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT FunctionModuleRegistry : public KoGenericRegistry<FunctionModule *>
{
public:
    /**
     * Creates the registry and loads the function modules.
     */
    FunctionModuleRegistry();
    ~FunctionModuleRegistry() override;

    /**
     * \return the singleton instance
     */
    static FunctionModuleRegistry *instance();

    /**
     * Loads the function modules.
     * Depending on their activation state read from the config,
     * the modules are added or removed from the registry.
     */
    void loadFunctionModules();

    /**
     * Registers the functions from all modules in the function repository
     * and adds their descriptions.
     */
    void registerFunctions();

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_FUNCTION_MODULE_REGISTRY
