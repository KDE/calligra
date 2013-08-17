/*  This file is part of the KDE libraries
 *  Copyright (C) 1999 Waldo Bastian <bastian@kde.org>
 *  Copyright (C) 2005-2008 David Faure <faure@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation;
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 **/

#ifndef KSYCOCA_H
#define KSYCOCA_H

#include <kservice_export.h>
#include <ksycocatype.h>

#include <QtCore/QObject>
#include <QtCore/QStringList>

class QDataStream;
class KSycocaFactory;
class KSycocaFactoryList;
class KSycocaPrivate;

/**
 * Executable name of the kbuildsycoca program
 */
#define KBUILDSYCOCA_EXENAME "kbuildsycoca5"

/**
 * @internal
 * Read-only SYstem COnfiguration CAche
 */
class KSERVICE_EXPORT KSycoca : public QObject
{
  Q_OBJECT
  //Q_CLASSINFO("D-Bus Interface", "org.kde.KSycoca")

protected:
   /**
    * @internal
    * Building database
    */
   explicit KSycoca( bool /* buildDatabase */ );

public:
   /**
    * type of database
    * @see absoluteFilePath()
    */
   typedef enum { LocalDatabase, GlobalDatabase } DatabaseType;

   /**
    * Read-only database
    */
   KSycoca();

   /**
    * Get or create the only instance of KSycoca (read-only)
    */
   static KSycoca *self();

   virtual ~KSycoca();

   /**
    * @return the compiled-in version, i.e. the one used when writing a new ksycoca
    */
   static int version();

   /**
    * @return true if the ksycoca database is available
    * This is usually the case, except if KDE isn't installed yet,
    * or before kded is started.
    */
   static bool isAvailable();

   /**
    * @internal - called by factories in read-only mode
    * This is how factories get a stream to an entry
    */
   QDataStream *findEntry(int offset, KSycocaType &type);
   /**
    * @internal - called by factories in read-only mode
    * Returns stream(), but positioned for reading this factory, 0 on error.
    */
   QDataStream *findFactory(KSycocaFactoryId id);
   /**
    * @internal - returns absolute file path of the database
    *
    * for global database type the database is searched under
    * the 'services' install path.
    * Otherwise, the value from the environment variable KDESYCOCA
    * is returned if set. If not set the path is build based on
    * KStandardDirs cache save location.
    */
   static QString absoluteFilePath(DatabaseType type=LocalDatabase);
   /**
    * @internal - returns language stored inside database
    */
   QString language();

   /**
    * @internal - returns timestamp of database
    *
    * The database contains all changes made _before_ this time and
    * _might_ contain changes made after that.
    */
   quint32 timeStamp();

   /**
    * @internal - returns update signature of database
    *
    * Signature that keeps track of changes to
    * $KDEDIR/share/services/update_ksycoca
    *
    * Touching this file causes the database to be recreated
    * from scratch.
    */
   quint32 updateSignature();

   /**
    * @internal - returns all directories with information
    * stored inside sycoca.
    */
   QStringList allResourceDirs();

   /**
    * @internal - add a factory
    */
   void addFactory( KSycocaFactory * );

   /**
    * @internal
    * @return true if building (i.e. if a KBuildSycoca);
    */
   virtual bool isBuilding();

   /**
    * @internal - disables launching of kbuildsycoca
    */
   static void disableAutoRebuild();

   /**
    * When you receive a "databaseChanged" signal, you can query here if
    * a change has occurred in a specific resource type.
    * @see KStandardDirs for the various resource types.
    *
    * This method is meant to be called from the GUI thread only.
    * @deprecated use the signal databaseChanged(QStringList) instead.
    */
#ifndef KDE_NO_DEPRECATED
   static KSERVICE_DEPRECATED bool isChanged(const char *type);
#endif

   /**
    * A read error occurs.
    * @internal
    */
   static void flagError();

Q_SIGNALS:
    /**
     * Connect to this to get notified when the database changes
     * @deprecated use the databaseChanged(QStringList) signal
     */
#ifndef KDE_NO_DEPRECATED
    QT_MOC_COMPAT void databaseChanged(); // KDE5 TODO: remove
#endif

    /**
     * Connect to this to get notified when the database changes
     * Example: when mimetype definitions have changed, applications showing
     * files as icons refresh icons to take into account the new mimetypes.
     * Another example: after creating a .desktop file in KOpenWithDialog,
     * it must wait until kbuildsycoca5 finishes until the KService::Ptr is available.
     *
     * @param changedResources List of resources where changes were detected.
     * This can include the following resources (as defined in KStandardDirs) :
     * apps, xdgdata-apps, services, servicetypes, xdgdata-mime.
     */
    void databaseChanged(const QStringList& changedResources);

protected:
    // @internal used by kbuildsycoca
    KSycocaFactoryList* factories();

    // @internal used by factories and kbuildsycoca
    QDataStream*& stream();
    friend class KSycocaFactory;
    friend class KSycocaDict;

private Q_SLOTS:
    /**
     * internal function for receiving kbuildsycoca's signal, when the sycoca file changes
     */
    void notifyDatabaseChanged(const QStringList &);

private:
    /**
     * Clear all caches related to ksycoca contents.
     * @internal only used by kded and kbuildsycoca.
     */
    static void clearCaches();
    /**
     * @internal - returns kfsstnd stored inside database
     */
    QString kfsstnd_prefixes();
    friend class KBuildSycoca;
    friend class Kded;

    Q_DISABLE_COPY(KSycoca)
    friend class KSycocaPrivate;
    KSycocaPrivate * const d;
};

#endif

