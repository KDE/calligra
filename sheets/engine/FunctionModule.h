// This file is part of the KDE project
// SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef CALLIGRA_SHEETS_FUNCTION_MODULE
#define CALLIGRA_SHEETS_FUNCTION_MODULE

#include <KPluginFactory>
#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QString>

#include "sheets_engine_export.h"

namespace Calligra
{
namespace Sheets
{
class Function;

/**
 * \ingroup Value
 * A function module provides several Function objects.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT FunctionModule : public QObject
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
    QList<QSharedPointer<Function>> functions() const;

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
    void add(Function *function);

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

/**
 * Register a function module when it is contained in a loadable plugin
 */
#ifndef SHEETS_NO_PLUGINMODULES
#define CALLIGRA_SHEETS_EXPORT_FUNCTION_MODULE(jsonfile, classname) K_PLUGIN_FACTORY_WITH_JSON(factory, jsonfile, registerPlugin<classname>();)
#else
#define CALLIGRA_SHEETS_EXPORT_FUNCTION_MODULE(libname, classname)
#endif

#endif // CALLIGRA_SHEETS_FUNCTION_MODULE
