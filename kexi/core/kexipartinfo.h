/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2015 Jarosław Staniek <staniek@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIPARTINFO_H
#define KEXIPARTINFO_H

#include <KPluginMetaData>

#include "kexi.h"

class QAction;
class KexiProject;
class KexiWindow;

namespace KexiPart
{
class Manager;
class Item;
class Part;

//! Provides information about a single Kexi Part plugin
class KEXICORE_EXPORT Info : public KPluginMetaData
{
public:
    ~Info();

    /**
     * @return internal name of the plugin, a shortcut of pluginId()
     *
     * Example: "org.kexi-project.table"
     */
    QString id() const;

    /**
     * @return a symbolic type name e.g. "table" for a plugin of "org.kexi-project.table" id.
     *
     * In theory there can be multiple plugins (with different IDs) with the same type name.
     * Defined by a X-Kexi-TypeName field in "kexi_*.desktop" information files.
     */
    QString typeName() const;

    /**
     * @return a i18n'ed group name e.g. "Tables" or "Queries".
     *
     * Defined by a X-Kexi-GroupName[language] field in "kexi_*.desktop" information files.
     */
    QString groupName() const;

    /**
     * @return an untranslated group name e.g. "Tables" or "Queries".
     *
     * Defined by a X-Kexi-GroupName field in "kexi_*.desktop" information files.
     * Like groupName() but always in English.
     */
    QString untranslatedGroupName() const;

    /**
     * @return the icon name for a item
     */
    QString createIconName() const;

    /**
     * @return supported modes for dialogs created by this part, i.e. a combination
     * of Kexi::ViewMode enum elements. Modes are declared in related .desktop files
     * in the X-KDE-ServiceTypes field by specifying a list of types, currently
     * possible values are: "Kexi/Viewer" (data view), "Kexi/Designer" (design view),
     * "Kexi/Editor" (text view).
     *
     * This information is used to set supported view modes for every KexiView-derived
     * object created by a KexiPart.
     */
    Kexi::ViewModes supportedViewModes() const;

    /**
     * @return supported modes for dialogs created by this part in "user mode", i.e. a combination
     * of Kexi::ViewMode enum elements.
     * Modes are declared in related .desktop files in the X-Kexi-ServiceTypesInUserMode field
     * by specifying a list of types. For the list of possible values see supportedViewModes().
     *
     * This information is used to set supported view modes for every KexiView-derived
     * object created by a KexiPart.
     */
    Kexi::ViewModes supportedUserViewModes() const;

    /**
     * @return true if the plugin should have a corresponding folder-like entry in
     * the Project Navigator (as a folder).
     *
     * Defined by a boolean X-Kexi-VisibleInProjectNavigator field in "kexi_*.desktop"
     * information files.
     */
    bool isVisibleInNavigator() const;

    /**
     * @return true if the part supports data exporting.
     *
     * Defined by a boolean X-Kexi-SupportsDataExport field in "kexi_*.desktop"
     * information files.
     */
    bool isDataExportSupported() const;

    /**
     * @return true if the part supports data printing.
     *
     * Defined by a boolean X-Kexi-SupportsPrinting field in "kexi_*.desktop"
     * information files.
     */
    bool isPrintingSupported() const;

    /**
     * @return true if the part supports execution.
     *
     * This is for example the case for the Macro and the Scripting plugins.
     * Defined by a boolean X-Kexi-SupportsExecution field in "kexi_*.desktop"
     * information files.
     */
    bool isExecuteSupported() const;

    /**
     * @return true if the property editing facilities should be displayed even
     * if the item's property set (KexiWindow::propertySet()) is 0.
     *
     * @c false by default. It is set to true e.g. for the "table" plugin.
     * Defined by a boolean X-Kexi-PropertyEditorAlwaysVisibleInDesignMode field
     * in "kexi_*.desktop" information files.
     */
    bool isPropertyEditorAlwaysVisibleInDesignMode() const;

    /**
     * @return major version of this part.
     *
     * 0 means invalid version.
     * @see version()
     */
    int majorVersion() const;

    /**
     * @return minor version of this part.
     *
     * @see version()
     */
    int minorVersion() const;

    /**
     * @return "New object" action for this part.
     */
    QAction* newObjectAction();

protected:
//! @todo KEXI3 fields of static plugins should be defined in .desktop files too (see KReport's static plugins)
//    /**
//     * Used in StaticPartInfo
//     */
//    Info(const QString &id, const QString &iconName, const QString &objectName);

    explicit Info(const QPluginLoader &loader);

    friend class Manager;
    friend class ::KexiProject;
    friend class ::KexiWindow;

    /**
     * Sets an error message useful to explain loading-related issues found with this plugin.
     * Most likely to be called by @ref KexiPart::Manager
     */
    void setErrorMessage(const QString& errorMessage);

    /**
     * @return i18n'd error message set by setErrorMessage().
     */
    QString errorMessage() const;

private:
    Q_DISABLE_COPY(Info)
    class Private;
    Private * const d;
};

}
#endif
