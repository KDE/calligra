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

#ifndef CALLIGRA_SHEETS_FUNCTION_MODULE
#define CALLIGRA_SHEETS_FUNCTION_MODULE

#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <kpluginfactory.h>

#include "sheets_odf_export.h"

namespace Calligra
{
namespace Sheets
{
class Function;

/**
 * \ingroup Value
 * A function module provides several Function objects.
 */
class CALLIGRA_SHEETS_ODF_EXPORT FunctionModule : public QObject
{
    Q_OBJECT
public:
    /**
     * Creates the function module.
     * The derived class should create here the Function objects and
     * should register them via \ref add.
     */
    explicit FunctionModule(QObject *parent);

    /**
     * Destroys the module and the provided Function objects.
     * Check, if this module isRemovable(), before you unload the plugin.
     */
    ~FunctionModule() override;

    /**
     * Returns the file name of the XML description for the functions.
     */
    virtual QString descriptionFileName() const = 0;

    /**
     * Returns a list of the provided Function objects.
     */
    QList<QSharedPointer<Function> > functions() const;

    /**
     * Checks whether this module can be removed, because none of its
     * Function objects is in use.
     * Used by the FunctionModuleRegistry to check, if the plugin can be unloaded.
     * \return \c true on success; \c false on failure
     */
    bool isRemovable();

    /**
     * Returns the identifier (if defined). Function of the KoGenericRegistry
     * template, that has to be implemented.
     */
    virtual QString id() const;

protected:
    /**
     * Adds \p function to the list of provided Function objects.
     */
    void add(Function* function);

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

/**
* Register a function module when it is contained in a loadable plugin
*/
#ifndef SHEETS_NO_PLUGINMODULES
#define CALLIGRA_SHEETS_EXPORT_FUNCTION_MODULE(jsonfile, classname) \
    K_PLUGIN_FACTORY_WITH_JSON(factory, jsonfile, registerPlugin<classname>();)
#else
#define CALLIGRA_SHEETS_EXPORT_FUNCTION_MODULE(libname, classname)
#endif

#endif // CALLIGRA_SHEETS_FUNCTION_MODULE
