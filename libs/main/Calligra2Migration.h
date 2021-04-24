/* This file is part of the KDE project
 *   SPDX-FileCopyrightText: 2016 Dag Andersen <danders@get2net.dk>
 * 
 *   SPDX-License-Identifier: LGPL-2.0-or-later
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
