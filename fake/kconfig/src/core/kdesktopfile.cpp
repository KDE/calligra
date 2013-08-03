/*
  This file is part of the KDE libraries
  Copyright (c) 1999 Pietro Iglio <iglio@kde.org>
  Copyright (c) 1999 Preston Brown <pbrown@kde.org>

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

// Qt5 TODO: re-enable. No point in doing it before, it breaks on QString::fromUtf8(QByteArray), which exists in qt5.
#undef QT_NO_CAST_FROM_BYTEARRAY

#include "kdesktopfile.h"

#include <unistd.h>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QUrl>
#include <qstandardpaths.h>

#include "kconfig_p.h"
#include "kconfiggroup.h"
#include "kcoreauthorized.h"
#include "kconfigini_p.h"

class KDesktopFilePrivate : public KConfigPrivate
{
 public:
    KDesktopFilePrivate(QStandardPaths::StandardLocation resourceType, const QString &fileName);
    KConfigGroup desktopGroup;
};

KDesktopFilePrivate::KDesktopFilePrivate(QStandardPaths::StandardLocation resourceType, const QString &fileName)
    : KConfigPrivate(KConfig::NoGlobals, resourceType)
{
    mBackend = new KConfigIniBackend();
    bDynamicBackend = false;
    changeFileName(fileName);
}

KDesktopFile::KDesktopFile(QStandardPaths::StandardLocation resourceType, const QString &fileName)
    : KConfig(*new KDesktopFilePrivate(resourceType, fileName))
{
    Q_D(KDesktopFile);
    reparseConfiguration();
    d->desktopGroup = KConfigGroup(this, "Desktop Entry");
}

KDesktopFile::KDesktopFile(const QString &fileName)
    : KConfig(*new KDesktopFilePrivate(QStandardPaths::ApplicationsLocation, fileName))
{
    Q_D(KDesktopFile);
    reparseConfiguration();
    d->desktopGroup = KConfigGroup(this, "Desktop Entry");
}

KDesktopFile::~KDesktopFile()
{
}

KConfigGroup KDesktopFile::desktopGroup() const
{
    Q_D(const KDesktopFile);
    return d->desktopGroup;
}

QString KDesktopFile::locateLocal(const QString &path)
{
    QString relativePath;
    // Relative to config? (e.g. for autostart)
    Q_FOREACH(const QString& dir, QStandardPaths::standardLocations(QStandardPaths::ConfigLocation)) {
        if (path.startsWith(dir) + '/') {
            relativePath = dir.mid(path.length() + 1);
            return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QLatin1Char('/') + relativePath;
        }
    }
    // Relative to xdg data dir? (much more common)
    Q_FOREACH(const QString& dir, QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation)) {
        if (path.startsWith(dir) + '/')
            relativePath = dir.mid(path.length() + 1);
    }
    if (relativePath.isEmpty()) {
        // What now? The desktop file doesn't come from XDG_DATA_DIRS. Use filename only and hope for the best.
        relativePath = path.mid(path.lastIndexOf(QLatin1Char('/'))+1);
    }
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1Char('/') + relativePath;
}

bool KDesktopFile::isDesktopFile(const QString& path)
{
  return (path.length() > 8
          && path.endsWith(QLatin1String(".desktop")));
}

bool KDesktopFile::isAuthorizedDesktopFile(const QString& path)
{
  if (path.isEmpty())
     return false; // Empty paths are not ok.

  if (QDir::isRelativePath(path))
     return true; // Relative paths are ok.

  const QString realPath = QFileInfo(path).canonicalFilePath();
  if (realPath.isEmpty())
      return false; // File doesn't exist.

  // Check if the .desktop file is installed as part of KDE or XDG.
  const QStringList appsDirs = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
  Q_FOREACH (const QString &prefix, appsDirs) {
      if (QDir(prefix).exists() && realPath.startsWith(QFileInfo(prefix).canonicalFilePath()))
          return true;
  }
  const QString servicesDir = QLatin1String("kde5/services/"); // KGlobal::dirs()->xdgDataRelativePath("services")
  Q_FOREACH (const QString &xdgDataPrefix, QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation)) {
      if (QDir(xdgDataPrefix).exists()) {
          const QString prefix = QFileInfo(xdgDataPrefix).canonicalFilePath();
          if (realPath.startsWith(prefix + QLatin1Char('/') + servicesDir))
              return true;
      }
  }
  const QString autostartDir = QLatin1String("autostart/");
  Q_FOREACH (const QString &xdgDataPrefix, QStandardPaths::standardLocations(QStandardPaths::ConfigLocation)) {
      if (QDir(xdgDataPrefix).exists()) {
          const QString prefix = QFileInfo(xdgDataPrefix).canonicalFilePath();
          if (realPath.startsWith(prefix + QLatin1Char('/') + autostartDir))
              return true;
      }
  }

  // Forbid desktop files outside of standard locations if kiosk is set so
  if (!KAuthorized::authorize(QLatin1String("run_desktop_files"))) {
     qWarning() << "Access to '" << path << "' denied because of 'run_desktop_files' restriction." << endl;
     return false;
  }

  // Not otherwise permitted, so only allow if the file is executable, or if
  // owned by root (uid == 0)
  QFileInfo entryInfo( path );
  if (entryInfo.isExecutable() || entryInfo.ownerId() == 0)
      return true;

  qWarning() << "Access to '" << path << "' denied, not owned by root, executable flag not set." << endl;
  return false;
}

QString KDesktopFile::readType() const
{
  Q_D(const KDesktopFile);
  return d->desktopGroup.readEntry("Type", QString());
}

QString KDesktopFile::readIcon() const
{
  Q_D(const KDesktopFile);
  return d->desktopGroup.readEntry("Icon", QString());
}

QString KDesktopFile::readName() const
{
  Q_D(const KDesktopFile);
  return d->desktopGroup.readEntry("Name", QString());
}

QString KDesktopFile::readComment() const
{
  Q_D(const KDesktopFile);
  return d->desktopGroup.readEntry("Comment", QString());
}

QString KDesktopFile::readGenericName() const
{
  Q_D(const KDesktopFile);
  return d->desktopGroup.readEntry("GenericName", QString());
}

QString KDesktopFile::readPath() const
{
  Q_D(const KDesktopFile);
  // NOT readPathEntry, it is not XDG-compliant. Path entries written by
  // KDE4 will be still treated as such, though.
  return d->desktopGroup.readEntry("Path", QString());
}

QString KDesktopFile::readDevice() const
{
  Q_D(const KDesktopFile);
  return d->desktopGroup.readEntry("Dev", QString());
}

QString KDesktopFile::readUrl() const
{
    Q_D(const KDesktopFile);
    if (hasDeviceType()) {
        return d->desktopGroup.readEntry("MountPoint", QString());
    } else {
        // NOT readPathEntry (see readPath())
        QString url = d->desktopGroup.readEntry("URL", QString());
        if ( !url.isEmpty() && !QDir::isRelativePath(url) )
        {
            // Handle absolute paths as such (i.e. we need to escape them)
            return QUrl::fromLocalFile(url).toString();
        }
        return url;
    }
}

QStringList KDesktopFile::readActions() const
{
    Q_D(const KDesktopFile);
    return d->desktopGroup.readXdgListEntry("Actions");
}

KConfigGroup KDesktopFile::actionGroup(const QString &group)
{
    return KConfigGroup(this, QLatin1String("Desktop Action ") + group);
}

const KConfigGroup KDesktopFile::actionGroup(const QString& group) const
{
    return const_cast<KDesktopFile*>(this)->actionGroup(group);
}

bool KDesktopFile::hasActionGroup(const QString &group) const
{
  return hasGroup(QString(QLatin1String("Desktop Action ") + group).toUtf8().constData());
}

bool KDesktopFile::hasLinkType() const
{
  return readType() == QLatin1String("Link");
}

bool KDesktopFile::hasApplicationType() const
{
  return readType() == QLatin1String("Application");
}

bool KDesktopFile::hasDeviceType() const
{
  return readType() == QLatin1String("FSDevice");
}

bool KDesktopFile::tryExec() const
{
  Q_D(const KDesktopFile);
  // Test for TryExec and "X-KDE-AuthorizeAction"
  // NOT readPathEntry (see readPath())
  QString te = d->desktopGroup.readEntry("TryExec", QString());

  if (!te.isEmpty()) {
    return !QStandardPaths::findExecutable(te).isEmpty();
  }
  const QStringList list = d->desktopGroup.readEntry("X-KDE-AuthorizeAction", QStringList());
  if (!list.isEmpty())
  {
     for(QStringList::ConstIterator it = list.begin();
         it != list.end();
         ++it)
     {
        if (!KAuthorized::authorize((*it).trimmed()))
           return false;
     }
  }

  // See also KService::username()
  bool su = d->desktopGroup.readEntry("X-KDE-SubstituteUID", false);
  if (su)
  {
      QString user = d->desktopGroup.readEntry("X-KDE-Username", QString());
      if (user.isEmpty())
        user = QString::fromLocal8Bit(qgetenv("ADMIN_ACCOUNT"));
      if (user.isEmpty())
        user = QString::fromLatin1("root");
      if (!KAuthorized::authorize(QString::fromLatin1("user/")+user))
        return false;
  }

  return true;
}

/**
 * @return the filename as passed to the constructor.
 */
//QString KDesktopFile::fileName() const { return backEnd->fileName(); }

/**
 * @return the resource type as passed to the constructor.
 */
//QString
//KDesktopFile::resource() const { return backEnd->resource(); }

QStringList
KDesktopFile::sortOrder() const
{
  Q_D(const KDesktopFile);
  return d->desktopGroup.readEntry("SortOrder", QStringList());
}

//void KDesktopFile::virtual_hook( int id, void* data )
//{ KConfig::virtual_hook( id, data ); }

QString KDesktopFile::readDocPath() const
{
  Q_D(const KDesktopFile);
  //legacy entry in kde3 apps
  if(d->desktopGroup.hasKey( "DocPath" ))
    return d->desktopGroup.readPathEntry( "DocPath", QString() );
  return d->desktopGroup.readPathEntry( "X-DocPath", QString() );
}

KDesktopFile* KDesktopFile::copyTo(const QString &file) const
{
  KDesktopFile *config = new KDesktopFile(QString());
  this->KConfig::copyTo(file, config);
//  config->setDesktopGroup();
  return config;
}

QStandardPaths::StandardLocation KDesktopFile::resource() const
{
    Q_D(const KDesktopFile);
    return d->resourceType;
}

QString KDesktopFile::fileName() const
{
    return name();
}

bool KDesktopFile::noDisplay() const
{
    Q_D(const KDesktopFile);
    if (d->desktopGroup.readEntry("NoDisplay", false)) {
        return true;
    }
    if (d->desktopGroup.hasKey("OnlyShowIn")) {
        if (!d->desktopGroup.readXdgListEntry("OnlyShowIn").contains(QLatin1String("KDE")))
            return true;
    }
    if (d->desktopGroup.hasKey("NotShowIn")) {
        if (d->desktopGroup.readXdgListEntry("NotShowIn").contains(QLatin1String("KDE")))
            return true;
    }
    return false;
}
