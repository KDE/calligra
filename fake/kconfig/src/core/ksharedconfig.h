/*
  This file is part of the KDE libraries
  Copyright (c) 1999 Preston Brown <pbrown@kde.org>
  Copyright (C) 1997-1999 Matthias Kalle Dalheimer (kalle@kde.org)

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

#ifndef KSHAREDCONFIG_H
#define KSHAREDCONFIG_H

#include <kconfig.h>
#include <ksharedptr.h>

/**
 * \class KSharedConfig ksharedconfig.h <KSharedConfig>
 *
 * KConfig variant using shared memory
 *
 * KSharedConfig provides a reference counted, shared memory variant
 * of KConfig.  This allows you to use manipulate the same configuration
 * files from different places in your code without worrying about
 * accidentally overwriting changes.
 *
 * Note that, as with most of kdelibs, this is @b NOT threadsafe.
 */
class KCONFIGCORE_EXPORT KSharedConfig : public KConfig, public QSharedData //krazy:exclude=dpointer (only for refcounting)
{
public:
  typedef KSharedPtr<KSharedConfig> Ptr;

public:
    /**
     * Creates a KSharedConfig object to manipulate a configuration file
     *
     * If an absolute path is specified for @p fileName, that file will be used
     * as the store for the configuration settings.  If a non-absolute path
     * is provided, the file will be looked for in the standard directory
     * specified by resourceType.  If no path is provided, a default
     * configuration file will be used based on the name of the main
     * application component.
     *
     * @p mode determines whether the user or global settings will be allowed
     * to influence the values returned by this object.  See KConfig::OpenFlags for
     * more details.
     *
     * @param fileName     the configuration file to open. If empty, it will be determined
     *                     automatically (from --config on the command line, otherwise
     *                     from the application name + "rc")
     * @param mode         how global settings should affect the configuration
     *                     options exposed by this KConfig object
     * @param resourceType The standard directory to look for the configuration
     *                     file in (see KStandardDirs)
     *
     * @sa KConfig
     */
    static KSharedConfig::Ptr openConfig(const QString& fileName = QString(),
                                         OpenFlags mode = FullConfig,
                                         QStandardPaths::StandardLocation type = QStandardPaths::ConfigLocation);

    virtual ~KSharedConfig();

private:
    virtual KConfigGroup groupImpl(const QByteArray& aGroup);
    virtual const KConfigGroup groupImpl(const QByteArray& aGroup) const;

    KSharedConfig(const QString& file, OpenFlags mode,
                  QStandardPaths::StandardLocation resourceType);

};

typedef KSharedConfig::Ptr KSharedConfigPtr;

#endif // multiple inclusion guard
