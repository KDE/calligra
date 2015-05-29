/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2014 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIPARTBASE_H
#define KEXIPARTBASE_H

#include <QObject>
#include <KLocalizedString>
#include <kexiutils/InternalPropertyMap.h>
#include "kexi.h"

class KTabWidget;
class KexiWindow;

namespace KexiPart
{
class Info;

//! @short The base class for Kexi frontend parts (plugins)
//! @see KexiPart::Part KexiInternalPart 
class KEXICORE_EXPORT PartBase : public QObject, protected KexiUtils::InternalPropertyMap
{
    Q_OBJECT

public:
    virtual ~PartBase();

    /*! @return Info structure for this part. */
    Info *info() const;

    /*! \return i18n'd message translated from \a englishMessage.
     This method is useful for messages like:
     "<p>Table \"%1\" has been modified.</p>",
     -- such messages can be accurately translated,
     while this could not: "<p>%1 \"%2\" has been modified.</p>".
     See implementation of this method in KexiTablePart to see
     what strings are needed for translation.

     Default implementation returns generic \a englishMessage.
     In special cases, \a englishMessage can start with ":",
     to indicate that empty string will be generated if
     a part does not offer a message for such \a englishMessage.
     This is used e.g. in KexiMainWindow::closeWindow().

     @note As number of %n parameters is unspecified,
     you should add appropriate number of parameters using .subs().
     to result of i18nMessage().
     In your your implementation, you should use ki18n(I18N_NOOP())
     or ki18nc(I18N_NOOP2()) instead of i18n() or i18nc().
     Example:
     @code
      QString tableName = "Employees";
      QString translated
       = part->i18nMessage("Design of object <resource>%1</resource> has been modified.")
        .subs(tableName).toString();
     @endcode */
    virtual KLocalizedString i18nMessage(const QString& englishMessage,
                                         KexiWindow *window) const;

    /*! @internal
     This method can be reimplemented to setup additional tabs
     in the property editor panel. Default implementation does nothing.
     This method is called whenever current window (KexiWindow) is switched and
     type (mime type) of its contents differs from previous one.
     For example, if a user switched from Table Designer to Form Designer,
     additional tab containing Form Designer's object tree should be shown. */
    virtual void setupCustomPropertyPanelTabs(KTabWidget *tab);

protected:
    /*!
     Creates new Plugin
     @param parent parent of this plugin
     @param list extra arguments passed to the plugin
    */
    PartBase(QObject *parent, 
        const QVariantList& list);

    /*! Sets Info structure for this part. */
    void setInfo(Info *info);

    Q_DISABLE_COPY(PartBase)

    class Private;
    Private * const d;

    friend class Manager;
    //friend class ::KexiWindow;
    //friend class GUIClient;
};

} // namespace KexiPart

//! Implementation of plugin's entry point
#define K_EXPORT_KEXIPART_PLUGIN( class_name, internal_name ) \
    KEXI_EXPORT_PLUGIN( "kexihandler", class_name, internal_name, KEXI_PART_VERSION, 0, 0 )

#endif
