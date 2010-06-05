/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_FUNCTION_MODULE_REGISTRY
#define KSPREAD_FUNCTION_MODULE_REGISTRY

#include <QObject>

#include <KoGenericRegistry.h>

#include "kspread_export.h"

namespace KSpread
{
class FunctionModule;

/**
 * \ingroup Plugin
 * \ingroup Value
 * Registry for function modules.
 */
class KSPREAD_EXPORT FunctionModuleRegistry : public QObject, public KoGenericRegistry<FunctionModule*>
{
public:
    /**
     * Creates the registry and loads the function modules.
     */
    FunctionModuleRegistry();
    ~FunctionModuleRegistry();

    /**
     * \return the singleton instance
     */
    static FunctionModuleRegistry* instance();

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
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_FUNCTION_MODULE_REGISTRY
