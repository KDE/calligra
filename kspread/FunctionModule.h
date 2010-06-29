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

#ifndef KSPREAD_FUNCTION_MODULE
#define KSPREAD_FUNCTION_MODULE

#include <kofficeversion.h>

#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QString>

#include "kspread_export.h"

namespace KSpread
{
class Function;

/**
 * \ingroup Value
 * A function module provides several Function objects.
 */
class KSPREAD_EXPORT FunctionModule : public QObject
{
    Q_OBJECT
public:
    /**
     * Creates the function module.
     * The derived class should create here the Function objects and
     * should register them via \ref add.
     */
    FunctionModule(QObject* parent);

    /**
     * Destroys the module and the provided Function objects.
     * Check, if this module isRemovable(), before you unload the plugin.
     */
    virtual ~FunctionModule();

    /**
     * Returns the file name of the XML description for the functions.
     */
    virtual QString descriptionFileName() const = 0;

    /**
     * Returns a list of the provided Function objects.
     */
    QList<QSharedPointer<Function> > functions() const;

    /**
     * Checks wether this module can be removed, because none of its
     * Function objects is in use.
     * Used by the FunctionModuleRegistry to check, if the plugin can be unloaded.
     * \return \c true on success; \c false on failure
     */
    bool isRemovable();
	
	/**
	  * function of the KoGenericRegistry template, that has to be implemented
	  */
	QString id() const;

protected:
    /**
     * Adds \p function to the list of provided Function objects.
     */
    void add(Function* function);

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

/**
* Register a function module when it is contained in a loadable plugin
*/
#define KSPREAD_EXPORT_FUNCTION_MODULE(libname, classname) \
    K_PLUGIN_FACTORY(factory, registerPlugin<classname>();) \
    K_EXPORT_PLUGIN(factory("kspread-functions-" #libname)) \
    K_EXPORT_PLUGIN_VERSION(KOFFICE_VERSION)

#endif // KSPREAD_FUNCTION_MODULE
