/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2011 Jarosław Staniek <staniek@kde.org>

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

#include "kexi.h"
#include "kexipartmanager.h"

class KexiProject;
class KexiWindow;

namespace KexiPart
{
class Manager;
class Item;
class Part;

/**
 * @short Information about a Kexi Part (plugin).
 */
class KEXICORE_EXPORT Info
{
public:
    ~Info();

    /*! i18n'd instance name usable for displaying in gui as object's caption,
     e.g. "Table".
     It is taken from the corresponding plugin (service) .desktop file
     (Name property). */
    QString instanceCaption() const;

    /*! @return a i18n'ed group name e.g. "Tables".
     It is taken from the corresponding plugin (service) .desktop file
     (GenericName property). */
    QString groupName() const;

//  /**
//   * @return the icon for groups
//   */
//  inline QString groupIcon() const { return m_groupIcon; }

    /**
     * @return the icon name for a item
     */
    QString itemIconName() const;

    /**
     * @return the icon name for a item
     */
    QString createItemIconName() const;

    /**
     * @return the object name associated with this part (e.g. "table")
     */
    QString objectName() const;

    /**
     * @return the globally-unique class id associated with this part (e.g. "org.kexi-project.table")
     */
    QString partClass() const;

    /**
     * @return the KService::Ptr associated with this part
     */
    KService::Ptr ptr() const;

    /*! @return supported modes for dialogs created by this part, i.e. a combination
     of Kexi::ViewMode enum elements. Modes are declared in related .desktop file
     by setting boolean values for properties X-Kexi-SupportsDataView,
     X-Kexi-SupportsDesignView, X-Kexi-SupportsTextView.

     The default value is Kexi::DataViewMode | Kexi::DesignViewMode.
     This information is used to set supported view modes for every
     KexiView-derived object created by this KexiPart. */
    Kexi::ViewModes supportedViewModes() const;

    /*! @return supported modes for dialogs created by this part in "user mode", i.e. a combination
     of Kexi::ViewMode enum elements.
     Modes are declared in related .desktop file by setting boolean values for properties
     X-Kexi-SupportsDataViewInUserMode, X-Kexi-SupportsDesignViewInUserMode,
     X-Kexi-SupportsTextViewInUserMode.
     The default value is Kexi::DataViewMode.
     This information is used to set supported view modes for every
     KexiView-derived object created by this KexiPart. */
    Kexi::ViewModes supportedUserViewModes() const;

    /**
     * @return true if loading was tried but failed
     */
    bool isBroken() const;

    /**
     * \return true if the part should be visible in the Project Navigator (as a folder).
     */
    bool isVisibleInNavigator() const;

    /**
     * \return true if the part supports data exporting.
     */
    bool isDataExportSupported() const;

    /**
     * \return true if the part supports data printing.
     */
    bool isPrintingSupported() const;

    /**
     * \return true if the part supports execution. This is as
     * example the case for the Macro and the Scripting plugins.
     */
    bool isExecuteSupported() const;

    /**
     * \return true if the property editing facilities should 
     * be displayed even if the item's property set 
     * (KexiWindow::propertySet()) is 0.
     * False by default. It is set to true e.g. for table part.
     */
    bool isPropertyEditorAlwaysVisibleInDesignMode() const;

protected:
    /**
     * Used in StaticPartInfo
     */
    Info(const QString &partClass, const QString &itemIconName,
         const QString &objectName);

    explicit Info(KService::Ptr service);

    friend class Manager;
    friend class ::KexiProject;
    friend class ::KexiWindow;

#if 0 //moved as internal to KexiProject
    /**
     * @return the project-part-id
     */
    int projectPartID() const;

    /**
     * Sets the project-part-id.
     */
    void setProjectPartID(int id);
#endif
    /**
     * Sets the broken flag and error message.
     * Most likely to be called by @ref KexiPart::Manager
     */
    void setBroken(bool broken, const QString& errorMessage);

    /**
     * \return i18n'd error message set by setBroken().
     */
    QString errorMessage() const;

    void setIdStoredInPartDatabase(bool set);

    /**
     * \return true if ID of the part is stored in project's database
     * false by default. This flag is updated in Manager::checkProject()
     * and set to true on first successful execution of KexiWindow::storeNewData()
     * @internal
     */
    bool isIdStoredInPartDatabase() const;

private:
    Q_DISABLE_COPY(Info)
    class Private;
    Private * const d;
};

/*! \return "create" KAction's name for part defined by \a info.
 The result is like "tablepart_create". Used in Part::createGUIClients()
 and KexiBrowser. */
KEXICORE_EXPORT QString nameForCreateAction(const Info& info);

}

#endif
