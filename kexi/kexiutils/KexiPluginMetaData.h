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

#ifndef KEXIPLUGINMETADATA_H
#define KEXIPLUGINMETADATA_H

#include <KPluginMetaData>

#include "kexiutils_export.h"

//! Extended version of KPluginMetaData
class KEXIUTILS_EXPORT KexiPluginMetaData : public KPluginMetaData
{
public:
    ~KexiPluginMetaData();

    /**
     * @return internal name of the plugin, a shortcut of pluginId()
     *
     * Example: "org.kexi-project.table"
     */
    QString id() const;

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
     * @return translated user-visible error message set by setErrorMessage().
     */
    QString errorMessage() const;

protected:
    explicit KexiPluginMetaData(const QPluginLoader &loader);

    /**
     * Sets a translated user-visible error message useful to explain loading-related
     * issues found with this plugin. Most likely to be called by a plugin manager.
     */
    void setErrorMessage(const QString& errorMessage);

    /**
     * @return root object for this plugin, useful to retrieve Kexi-specific fields using
     * readStringList(), readTranslatedValue() or readTranslatedString().
     */
    QJsonObject rootObject() const;

private:
    Q_DISABLE_COPY(KexiPluginMetaData)
    class Private;
    Private * const d;
};

#endif
