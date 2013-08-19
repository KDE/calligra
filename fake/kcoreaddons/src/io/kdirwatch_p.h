/* Private Header for class of KDirWatchPrivate
 *
 * this separate header file is needed for MOC processing
 * because KDirWatchPrivate has signals and slots
 *
 * This file is part of the KDE libraries
 * Copyright (C) 1998 Sven Radej <sven@lisa.exp.univie.ac.at>
 * Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2007 Flavio Castelli <flavio.castelli@gmail.com>
 * Copyright (C) 2008 Jarosław Staniek <staniek@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KDIRWATCH_P_H
#define KDIRWATCH_P_H

#include <io/config-kdirwatch.h>
#include "kdirwatch.h"

#ifndef QT_NO_FILESYSTEMWATCHER
#define HAVE_QFILESYSTEMWATCHER 1
#else
#define HAVE_QFILESYSTEMWATCHER 0
#endif

#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QTimer>
class QFileSystemWatcher;
class QSocketNotifier;

#if HAVE_FAM
#include <limits.h>
#include <fam.h>
#endif

#if HAVE_SYS_INOTIFY_H
#include <unistd.h>
#include <fcntl.h>
#include <sys/inotify.h>

#ifndef IN_DONT_FOLLOW
#define IN_DONT_FOLLOW 0x02000000
#endif

#ifndef IN_ONLYDIR
#define IN_ONLYDIR 0x01000000
#endif

#endif

#include <sys/time.h>
#include <sys/param.h> // ino_t
#include <ctime>


#define invalid_ctime ((time_t)-1)

#if HAVE_QFILESYSTEMWATCHER
#include <QtCore/QFileSystemWatcher>

#if defined Q_OS_WIN
/* Helper implemented as a workaround for limitation on Windows:
 * the maximum number of object handles is MAXIMUM_WAIT_OBJECTS (64) per thread.
 *
 * From http://msdn.microsoft.com/en-us/library/ms687025(VS.85).aspx
 * "To wait on more than MAXIMUM_WAIT_OBJECTS handles, create a thread to wait
 *  on MAXIMUM_WAIT_OBJECTS handles, then wait on that thread plus the other handles.
 *  Use this technique to break the handles into groups of MAXIMUM_WAIT_OBJECTS."
 *
 * QFileSystemWatcher is implemented as thread, so KFileSystemWatcher
 * allocates more QFileSystemWatcher instances on demand (and deallocates them later).
 */
class KFileSystemWatcher : public QObject
{
  Q_OBJECT
public:
  KFileSystemWatcher();
  ~KFileSystemWatcher();
  void addPath(const QString &file);
  void removePath(const QString &file);

Q_SIGNALS:
    void fileChanged(const QString &path);
    void directoryChanged(const QString &path);

private:
  QFileSystemWatcher* availableWatcher();
  QFileSystemWatcher* m_recentWatcher;
  QList<QFileSystemWatcher*> m_watchers;
  QHash<QFileSystemWatcher*, uint> m_usedObjects;
  QHash<QString,QFileSystemWatcher*> m_paths;
};
#else
typedef QFileSystemWatcher KFileSystemWatcher;
#endif
#endif

/* KDirWatchPrivate is a singleton and does the watching
 * for every KDirWatch instance in the application.
 */
class KDirWatchPrivate : public QObject
{
  Q_OBJECT
public:

  enum entryStatus { Normal = 0, NonExistent };
  enum entryMode { UnknownMode = 0, StatMode, DNotifyMode, INotifyMode, FAMMode, QFSWatchMode };
  enum { NoChange=0, Changed=1, Created=2, Deleted=4 };


  struct Client {
    KDirWatch* instance;
    int count;
    // did the instance stop watching
    bool watchingStopped;
    // events blocked when stopped
    int pending;
    KDirWatch::WatchModes m_watchModes;
  };

  class Entry
  {
  public:
    // the last observed modification time
    time_t m_ctime;
    // the last observed link count
    int m_nlink;
    // last observed inode
    ino_t m_ino;
    entryStatus m_status;
    entryMode m_mode;
    bool isDir;
    // instances interested in events
    QList<Client *> m_clients;
    // nonexistent entries of this directory
    QList<Entry *> m_entries;
    QString path;

    int msecLeft, freq;

    QString parentDirectory() const;
    void addClient(KDirWatch*, KDirWatch::WatchModes);
    void removeClient(KDirWatch*);
    int clientCount() const;
    bool isValid() { return m_clients.count() || m_entries.count(); }

    Entry* findSubEntry(const QString& path) const {
        Q_FOREACH(Entry* sub_entry, m_entries) {
            if (sub_entry->path == path)
                return sub_entry;
        }
        return 0;
    }

    bool dirty;
    void propagate_dirty();

    QList<Client *> clientsForFileOrDir(const QString& tpath, bool* isDir) const;

#if HAVE_FAM
    FAMRequest fr;
#endif

#if HAVE_SYS_INOTIFY_H
    int wd;
    // Creation and Deletion of files happens infrequently, so
    // can safely be reported as they occur.  File changes i.e. those that emity "dirty()" can
    // happen many times per second, though, so maintain a list of files in this directory
    // that can be emitted and flushed at the next slotRescan(...).
    // This will be unused if the Entry is not a directory.
    QList<QString> m_pendingFileChanges;
#endif
  };

  typedef QMap<QString,Entry> EntryMap;

  KDirWatchPrivate();
  ~KDirWatchPrivate();

  void resetList (KDirWatch*,bool);
  void useFreq(Entry* e, int newFreq);
  void addEntry(KDirWatch* instance,const QString& _path, Entry* sub_entry,
        bool isDir, KDirWatch::WatchModes watchModes = KDirWatch::WatchDirOnly);
  void removeEntry(KDirWatch*,const QString&, Entry* sub_entry);
  void removeEntry(KDirWatch*,Entry* e, Entry* sub_entry);
  bool stopEntryScan(KDirWatch*, Entry*);
  bool restartEntryScan(KDirWatch*, Entry*, bool );
  void stopScan(KDirWatch*);
  void startScan(KDirWatch*, bool, bool);

  void removeEntries(KDirWatch*);
  void statistics();

  void addWatch(Entry* entry);
  void removeWatch(Entry* entry);
  Entry* entry(const QString&);
  int scanEntry(Entry* e);
  void emitEvent(const Entry* e, int event, const QString &fileName = QString());

  // Memory management - delete when last KDirWatch gets deleted
  void ref() { m_ref++; }
  bool deref() { return ( --m_ref == 0 ); }

 static bool isNoisyFile( const char *filename );

public Q_SLOTS:
  void slotRescan();
  void famEventReceived(); // for FAM
  void inotifyEventReceived(); // for inotify
  void slotRemoveDelayed();
  void fswEventReceived(const QString &path);  // for QFileSystemWatcher

public:
  QTimer timer;
  EntryMap m_mapEntries;

  KDirWatch::Method m_preferredMethod, m_nfsPreferredMethod;
  int freq;
  int statEntries;
  int m_nfsPollInterval, m_PollInterval;
  int m_ref;
  bool useStat(Entry*);

  // removeList is allowed to contain any entry at most once
  QSet<Entry *> removeList;
  bool delayRemove;

  bool rescan_all;
  QTimer rescan_timer;

#if HAVE_FAM
  QSocketNotifier *sn;
  FAMConnection fc;
  bool use_fam;

  void checkFAMEvent(FAMEvent*);
  bool useFAM(Entry*);
#endif

#if HAVE_SYS_INOTIFY_H
  QSocketNotifier *mSn;
  bool supports_inotify;
  int m_inotify_fd;

  bool useINotify(Entry*);
#endif
#if HAVE_QFILESYSTEMWATCHER
  KFileSystemWatcher *fsWatcher;
  bool useQFSWatch(Entry* e);
#endif

  bool _isStopped;
};

QDebug operator<<(QDebug debug, const KDirWatchPrivate::Entry &entry);

#endif // KDIRWATCH_P_H

