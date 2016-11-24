/* This file is part of the KDE project
 *   Copyright (C) 2016 Dag Andersen <danders@get2net.dk>
 * 
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 * 
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 * 
 *   You should have received a copy of the GNU Library General Public License
 *   along with this library; see the file COPYING.LIB.  If not, write to
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *   Boston, MA 02110-1301, USA.
 */

#ifndef Calligra2Migration_h
#define Calligra2Migration_h

#include "komain_export.h"

#include <QString>
#include <QStringList>

/**
 * class Calligra2Migration
 * 
 * Migrate application directories and files to new QStandardPaths locations
 * 
 * Calligra2Migration handles config files, ui files and data location.
 * Config- and ui files is handled by Kdelibs4Migrator.
 * If application has been renamed, rc files and data location is renamed accordingly.
 */

class KOMAIN_EXPORT Calligra2Migration
{
public:
    /// Create a migration instance
    /// @p appName Name of the application
    /// @p oldAppName If the application has been renamed the old name must be specified here
    explicit Calligra2Migration(const QString &appName, const QString &oldAppName = QString());

    void setConfigFiles(const QStringList &configFiles);
    void setUiFiles(const QStringList &uiFiles);
    void migrate();
    
private:
    QString m_newAppName;
    QString m_oldAppName;
    QStringList m_configFiles;
    QStringList m_uiFiles;
};

#endif
