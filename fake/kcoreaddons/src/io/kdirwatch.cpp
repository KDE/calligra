/* This file is part of the KDE libraries
   Copyright (C) 1998 Sven Radej <sven@lisa.exp.univie.ac.at>
   Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
   Copyright (C) 2007 Flavio Castelli <flavio.castelli@gmail.com>
   Copyright (C) 2008 Rafal Rzepecki <divided.mind@gmail.com>
   Copyright (C) 2010 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


// CHANGES:
// Jul 30, 2008 - Don't follow symlinks when recursing to avoid loops (Rafal)
// Aug 6,  2007 - KDirWatch::WatchModes support complete, flags work fine also
// when using FAMD (Flavio Castelli)
// Aug 3,  2007 - Handled KDirWatch::WatchModes flags when using inotify, now
// recursive and file monitoring modes are implemented (Flavio Castelli)
// Jul 30, 2007 - Substituted addEntry boolean params with KDirWatch::WatchModes
// flag (Flavio Castelli)
// Oct 4,  2005 - Inotify support (Dirk Mueller)
// Februar 2002 - Add file watching and remote mount check for STAT
// Mar 30, 2001 - Native support for Linux dir change notification.
// Jan 28, 2000 - Usage of FAM service on IRIX (Josef.Weidendorfer@in.tum.de)
// May 24. 1998 - List of times introduced, and some bugs are fixed. (sven)
// May 23. 1998 - Removed static pointer - you can have more instances.
// It was Needed for KRegistry. KDirWatch now emits signals and doesn't
// call (or need) KFM. No more URL's - just plain paths. (sven)
// Mar 29. 1998 - added docs, stop/restart for particular Dirs and
// deep copies for list of dirs. (sven)
// Mar 28. 1998 - Created.  (sven)

#include "kdirwatch.h"
#include "kdirwatch_p.h"
#include "kfilesystemtype_p.h"

#include <io/config-kdirwatch.h>

#include <sys/stat.h>
#include <assert.h>
#include <errno.h>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QSocketNotifier>
#include <QtCore/QTimer>
#include <QtCore/QCoreApplication>

#include <qplatformdefs.h> // QT_LSTAT, QT_STAT, QT_STATBUF

#include <stdlib.h>
#include <string.h>

// debug
#include <sys/ioctl.h>


#include <sys/utsname.h>

// set this to true for much more verbose debug output
static const bool s_verboseDebug = false;

// The KDirWatchPrivate instance is refcounted, and deleted by the last KDirWatch instance
static KDirWatchPrivate* dwp_self = 0;
static KDirWatchPrivate* createPrivate() {
  if (!dwp_self)
    dwp_self = new KDirWatchPrivate;
  return dwp_self;
}

// Convert a string into a watch Method
static KDirWatch::Method methodFromString(const QString& method) {
  if (method == QLatin1String("Fam")) {
    return KDirWatch::FAM;
  } else if (method == QLatin1String("Stat")) {
    return KDirWatch::Stat;
  } else if (method == QLatin1String("QFSWatch")) {
    return KDirWatch::QFSWatch;
  } else {
#ifdef Q_OS_LINUX
    // inotify supports delete+recreate+modify, which QFSWatch doesn't support
    return KDirWatch::INotify;
#else
    return KDirWatch::QFSWatch;
#endif
  }
}

#ifndef NDEBUG
static const char* methodToString(KDirWatch::Method method)
{
    switch (method) {
    case KDirWatch::FAM:
        return "Fam";
    case KDirWatch::INotify:
        return "INotify";
    case KDirWatch::DNotify:
        return "DNotify";
    case KDirWatch::Stat:
        return "Stat";
    case KDirWatch::QFSWatch:
        return "QFSWatch";
    default:
        return "ERROR!";
    }
}
#endif

//
// Class KDirWatchPrivate (singleton)
//

/* All entries (files/directories) to be watched in the
 * application (coming from multiple KDirWatch instances)
 * are registered in a single KDirWatchPrivate instance.
 *
 * At the moment, the following methods for file watching
 * are supported:
 * - Polling: All files to be watched are polled regularly
 *   using stat (more precise: QFileInfo.lastModified()).
 *   The polling frequency is determined from global kconfig
 *   settings, defaulting to 500 ms for local directories
 *   and 5000 ms for remote mounts
 * - FAM (File Alternation Monitor): first used on IRIX, SGI
 *   has ported this method to LINUX. It uses a kernel part
 *   (IMON, sending change events to /dev/imon) and a user
 *   level damon (fam), to which applications connect for
 *   notification of file changes. For NFS, the fam damon
 *   on the NFS server machine is used; if IMON is not built
 *   into the kernel, fam uses polling for local files.
 * - INOTIFY: In LINUX 2.6.13, inode change notification was
 *   introduced. You're now able to watch arbitrary inode's
 *   for changes, and even get notification when they're
 *   unmounted.
 */

KDirWatchPrivate::KDirWatchPrivate()
  : timer(),
    freq( 3600000 ), // 1 hour as upper bound
    statEntries( 0 ),
    m_ref( 0 ),
    delayRemove( false ),
    rescan_all( false ),
    rescan_timer()
{
  timer.setObjectName(QLatin1String("KDirWatchPrivate::timer"));
  connect (&timer, SIGNAL(timeout()), this, SLOT(slotRescan()));

#pragma message("KDE5 TODO: Remove dependencies on Kconfig and KGlobal")
#if 0
  KConfigGroup config(KSharedConfig::openConfig(), "DirWatch");
  m_nfsPollInterval = config.readEntry("NFSPollInterval", 5000);
  m_PollInterval = config.readEntry("PollInterval", 500);

  QString method = config.readEntry("PreferredMethod", "inotify");
  m_preferredMethod = methodFromString(method);

  // The nfs method defaults to the normal (local) method
  m_nfsPreferredMethod = methodFromString(config.readEntry("nfsPreferredMethod", "Fam"));
#endif
#pragma message("KDE5 FIXME: We use the default values until the todo above is fixed")
  m_nfsPollInterval = 5000;
  m_PollInterval = 500;
  m_preferredMethod = methodFromString(QLatin1String("inotify"));
  m_nfsPreferredMethod = methodFromString(QLatin1String("Fam"));

  QList<QByteArray> availableMethods;

  availableMethods << "Stat";

  // used for FAM and inotify
  rescan_timer.setObjectName(QString::fromLatin1("KDirWatchPrivate::rescan_timer"));
  rescan_timer.setSingleShot( true );
  connect(&rescan_timer, SIGNAL(timeout()), this, SLOT(slotRescan()));

#if HAVE_FAM
  // It's possible that FAM server can't be started
  if (FAMOpen(&fc) ==0) {
    availableMethods << "FAM";
    use_fam=true;
    sn = new QSocketNotifier( FAMCONNECTION_GETFD(&fc),
			      QSocketNotifier::Read, this);
    connect( sn, SIGNAL(activated(int)),
 	     this, SLOT(famEventReceived()) );
  }
  else {
    qDebug() << "Can't use FAM (fam daemon not running?)";
    use_fam=false;
  }
#endif

#if HAVE_SYS_INOTIFY_H
  supports_inotify = true;

  m_inotify_fd = inotify_init();

  if ( m_inotify_fd <= 0 ) {
    qDebug() << "Can't use Inotify, kernel doesn't support it";
    supports_inotify = false;
  }

  {
    struct utsname uts;
    int major, minor, patch;
    if (uname(&uts) < 0) {
      supports_inotify = false;
      qDebug() << "Unable to get uname";
    } else if (sscanf(uts.release, "%d.%d", &major, &minor) != 2) {
      supports_inotify = false;
      qDebug() << "The version is malformed: " << uts.release;
    } else if(major == 2 && minor == 6) { // If it is 2.6 check further...
      if (sscanf(uts.release, "%d.%d.%d", &major, &minor, &patch) != 3) {
        supports_inotify = false;
        qDebug() << "Detected 2.6 kernel but can't know more: " << uts.release;
      } else if (major * 1000000 + minor * 1000 + patch < 2006014 ){
        supports_inotify = false;
        qDebug() << "Can't use INotify, Linux kernel too old " << uts.release;
      }
    }
  }

  //qDebug() << "INotify available: " << supports_inotify;
  if ( supports_inotify ) {
    availableMethods << "INotify";
    (void)fcntl(m_inotify_fd, F_SETFD, FD_CLOEXEC);

    mSn = new QSocketNotifier( m_inotify_fd, QSocketNotifier::Read, this );
    connect( mSn, SIGNAL(activated(int)),
             this, SLOT(inotifyEventReceived()) );
  }
#endif
#if HAVE_QFILESYSTEMWATCHER
  availableMethods << "QFileSystemWatcher";
  fsWatcher = 0;
#endif
#ifndef NDEBUG
  qDebug() << "Available methods: " << availableMethods << "preferred=" << methodToString(m_preferredMethod);
#endif
}

// This is called on app exit (when K_GLOBAL_STATIC deletes KDirWatch::self)
KDirWatchPrivate::~KDirWatchPrivate()
{
  timer.stop();

  /* remove all entries being watched */
  removeEntries(0);

#if HAVE_FAM
  if (use_fam) {
    FAMClose(&fc);
  }
#endif
#if HAVE_SYS_INOTIFY_H
  if ( supports_inotify )
    ::close( m_inotify_fd );
#endif
#if HAVE_QFILESYSTEMWATCHER
  delete fsWatcher;
#endif
}

void KDirWatchPrivate::inotifyEventReceived()
{
  //qDebug();
#if HAVE_SYS_INOTIFY_H
  if ( !supports_inotify )
    return;

  int pending = -1;
  int offsetStartRead = 0; // where we read into buffer
  char buf[8192];
  assert( m_inotify_fd > -1 );
  ioctl( m_inotify_fd, FIONREAD, &pending );

  while ( pending > 0 ) {

    const int bytesToRead = qMin( pending, (int)sizeof( buf ) - offsetStartRead );

    int bytesAvailable = read( m_inotify_fd, &buf[offsetStartRead], bytesToRead );
    pending -= bytesAvailable;
    bytesAvailable += offsetStartRead;
    offsetStartRead = 0;

    int offsetCurrent = 0;
    while ( bytesAvailable >= (int)sizeof( struct inotify_event ) ) {
      const struct inotify_event * const event = (struct inotify_event *) &buf[offsetCurrent];
      const int eventSize = sizeof( struct inotify_event ) + event->len;
      if ( bytesAvailable < eventSize ) {
          break;
      }

      bytesAvailable -= eventSize;
      offsetCurrent += eventSize;

      QString path;
      QByteArray cpath(event->name, event->len);
      if(event->len)
        path = QFile::decodeName ( cpath );

      if ( path.length() && isNoisyFile( cpath.data() ) )
        continue;

      // now we're in deep trouble of finding the
      // associated entries
      // for now, we suck and iterate
      for ( EntryMap::Iterator it = m_mapEntries.begin();
            it != m_mapEntries.end();  ) {
        Entry* e = &( *it );
        ++it;
        if ( e->wd == event->wd ) {
          const bool wasDirty = e->dirty;
          e->dirty = true;

          //if (s_verboseDebug) {
          //  qDebug() << "got event" << "0x"+QString::number(event->mask, 16) << "for" << e->path;
          //}

          if( event->mask & IN_DELETE_SELF) {
            if (s_verboseDebug) {
              qDebug() << "-->got deleteself signal for" << e->path;
            }
            e->m_status = NonExistent;
            e->wd = -1;
            e->m_ctime = invalid_ctime;
            emitEvent(e, Deleted, e->path);
            // If the parent dir was already watched, tell it something changed
            Entry* parentEntry = entry(e->parentDirectory());
            if (parentEntry)
                parentEntry->dirty = true;
            // Add entry to parent dir to notice if the entry gets recreated
            addEntry(0, e->parentDirectory(), e, true /*isDir*/);
          }
          if ( event->mask & IN_IGNORED ) {
            // Causes bug #207361 with kernels 2.6.31 and 2.6.32!
            //e->wd = -1;
          }
          if ( event->mask & (IN_CREATE|IN_MOVED_TO) ) {
            const QString tpath = e->path + QLatin1Char('/') + path;
            Entry* sub_entry = e->findSubEntry(tpath);

            if (s_verboseDebug) {
              qDebug() << "-->got CREATE signal for" << (tpath) << "sub_entry=" << sub_entry;
              qDebug() << *e;
            }

            // The code below is very similar to the one in checkFAMEvent...
            if (sub_entry) {
              // We were waiting for this new file/dir to be created
              sub_entry->dirty = true;
              rescan_timer.start(0); // process this asap, to start watching that dir
            } else if (e->isDir && !e->m_clients.empty()) {
              bool isDir = false;
              const QList<Client *> clients = e->clientsForFileOrDir(tpath, &isDir);
              Q_FOREACH(Client *client, clients) {
                // See discussion in addEntry for why we don't addEntry for individual
                // files in WatchFiles mode with inotify.
                if (isDir) {
                  addEntry(client->instance, tpath, 0, isDir,
                           isDir ? client->m_watchModes : KDirWatch::WatchDirOnly);
                }
              }
              if (!clients.isEmpty()) {
                emitEvent(e, Created, tpath);
                qDebug().nospace() << clients.count() << " instance(s) monitoring the new "
                                       << (isDir ? "dir " : "file ") << tpath;
              }
              e->m_pendingFileChanges.append(e->path);
              if (!rescan_timer.isActive())
                  rescan_timer.start(m_PollInterval); // singleshot
            }
          }
          if (event->mask & (IN_DELETE|IN_MOVED_FROM)) {
            const QString tpath = e->path + QLatin1Char('/') + path;
            if (s_verboseDebug) {
              qDebug() << "-->got DELETE signal for" << tpath;
            }
            if ((e->isDir) && (!e->m_clients.empty())) {
              Client* client = 0;
              // A file in this directory has been removed.  It wasn't an explicitly
              // watched file as it would have its own watch descriptor, so
              // no addEntry/ removeEntry bookkeeping should be required.  Emit
              // the event immediately if any clients are interested.
              QT_STATBUF stat_buf;
              // Unlike clientsForFileOrDir, the stat can fail here (item deleted),
              // so in that case we'll just take both kinds of clients and emit Deleted.
              KDirWatch::WatchModes flag = KDirWatch::WatchSubDirs | KDirWatch::WatchFiles;
              if (QT_STAT(QFile::encodeName(tpath).constData(), &stat_buf) == 0) {
                bool isDir = S_ISDIR(stat_buf.st_mode);
                flag = isDir ? KDirWatch::WatchSubDirs : KDirWatch::WatchFiles;
              }
              int counter = 0;
              Q_FOREACH(client, e->m_clients) { // krazy:exclude=Q_FOREACH
                  if (client->m_watchModes & flag) {
                        counter++;
                  }
              }
              if (counter != 0) {
                  emitEvent(e, Deleted, tpath);
              }
            }
          }
          if (event->mask & (IN_MODIFY|IN_ATTRIB)) {
            if ((e->isDir) && (!e->m_clients.empty())) {
              const QString tpath = e->path + QLatin1Char('/') + path;
              if (s_verboseDebug) {
                qDebug() << "-->got MODIFY signal for" << (tpath);
              }
              // A file in this directory has been changed.  No
              // addEntry/ removeEntry bookkeeping should be required.
              // Add the path to the list of pending file changes if
              // there are any interested clients.
              //QT_STATBUF stat_buf;
              //QByteArray tpath = QFile::encodeName(e->path+'/'+path);
              //QT_STAT(tpath, &stat_buf);
              //bool isDir = S_ISDIR(stat_buf.st_mode);

              // The API doc is somewhat vague as to whether we should emit
              // dirty() for implicitly watched files when WatchFiles has
              // not been specified - we'll assume they are always interested,
              // regardless.
              // Don't worry about duplicates for the time
              // being; this is handled in slotRescan.
              e->m_pendingFileChanges.append(tpath);
              // Avoid stat'ing the directory if only an entry inside it changed.
              e->dirty = (wasDirty || (path.isEmpty() && (event->mask & IN_ATTRIB)));
            }
          }

          if (!rescan_timer.isActive())
            rescan_timer.start(m_PollInterval); // singleshot

          break;
        }
      }
    }
    if (bytesAvailable > 0) {
        // copy partial event to beginning of buffer
        memmove(buf, &buf[offsetCurrent], bytesAvailable);
        offsetStartRead = bytesAvailable;
    }
  }
#endif
}

/* In FAM mode, only entries which are marked dirty are scanned.
 * We first need to mark all yet nonexistent, but possible created
 * entries as dirty...
 */
void KDirWatchPrivate::Entry::propagate_dirty()
{
  Q_FOREACH(Entry *sub_entry, m_entries)
  {
     if (!sub_entry->dirty)
     {
        sub_entry->dirty = true;
        sub_entry->propagate_dirty();
     }
  }
}


/* A KDirWatch instance is interested in getting events for
 * this file/Dir entry.
 */
void KDirWatchPrivate::Entry::addClient(KDirWatch* instance,
                                        KDirWatch::WatchModes watchModes)
{
  if (instance == 0)
    return;

  Q_FOREACH(Client* client, m_clients) {
    if (client->instance == instance) {
      client->count++;
      client->m_watchModes = watchModes;
      return;
    }
  }

  Client* client = new Client;
  client->instance = instance;
  client->count = 1;
  client->watchingStopped = instance->isStopped();
  client->pending = NoChange;
  client->m_watchModes = watchModes;

  m_clients.append(client);
}

void KDirWatchPrivate::Entry::removeClient(KDirWatch* instance)
{
  QList<Client *>::iterator it = m_clients.begin();
  const QList<Client *>::iterator end = m_clients.end();
  for ( ; it != end ; ++it ) {
    Client* client = *it;
    if (client->instance == instance) {
      client->count--;
      if (client->count == 0) {
        m_clients.erase(it);
        delete client;
      }
      return;
    }
  }
}

/* get number of clients */
int KDirWatchPrivate::Entry::clientCount() const
{
  int clients = 0;
  Q_FOREACH(Client* client, m_clients)
    clients += client->count;

  return clients;
}

QString KDirWatchPrivate::Entry::parentDirectory() const
{
  return QDir::cleanPath(path + QLatin1String("/.."));
}

QList<KDirWatchPrivate::Client *> KDirWatchPrivate::Entry::clientsForFileOrDir(const QString& tpath, bool* isDir) const
{
  QList<Client *> ret;
  QFileInfo fi(tpath);
  if (fi.exists()) {
    *isDir = fi.isDir();
    const KDirWatch::WatchModes flag =
      *isDir ? KDirWatch::WatchSubDirs : KDirWatch::WatchFiles;
    Q_FOREACH(Client *client, this->m_clients) {
      if (client->m_watchModes & flag) {
        ret.append(client);
      }
    }
  } else {
    // Happens frequently, e.g. ERROR: couldn't stat "/home/dfaure/.viminfo.tmp"
    //qDebug() << "ERROR: couldn't stat" << tpath;
    // In this case isDir is not set, but ret is empty anyway
    // so isDir won't be used.
  }
  return ret;
}

QDebug operator<<(QDebug debug, const KDirWatchPrivate::Entry &entry)
{
  debug.nospace() << "[ Entry for " << entry.path << ", " << (entry.isDir ? "dir" : "file");
  if (entry.m_status == KDirWatchPrivate::NonExistent)
    debug << ", non-existent";
  debug << ", using " << ((entry.m_mode == KDirWatchPrivate::FAMMode) ? "FAM" :
                       (entry.m_mode == KDirWatchPrivate::INotifyMode) ? "INotify" :
                       (entry.m_mode == KDirWatchPrivate::DNotifyMode) ? "DNotify" :
                       (entry.m_mode == KDirWatchPrivate::QFSWatchMode) ? "QFSWatch" :
                       (entry.m_mode == KDirWatchPrivate::StatMode) ? "Stat" : "Unknown Method");
#if HAVE_SYS_INOTIFY_H
  if (entry.m_mode == KDirWatchPrivate::INotifyMode)
    debug << " inotify_wd=" << entry.wd;
#endif
  debug << ", has " << entry.m_clients.count() << " clients";
  debug.space();
  if (!entry.m_entries.isEmpty()) {
    debug << ", nonexistent subentries:";
    Q_FOREACH(KDirWatchPrivate::Entry* subEntry, entry.m_entries)
      debug << subEntry << subEntry->path;
  }
  debug << ']';
  return debug;
}

KDirWatchPrivate::Entry* KDirWatchPrivate::entry(const QString& _path)
{
// we only support absolute paths
  if (_path.isEmpty() || QDir::isRelativePath(_path)) {
    return 0;
  }

  QString path (_path);

  if ( path.length() > 1 && path.endsWith( QLatin1Char( '/' ) ) )
    path.truncate( path.length() - 1 );

  EntryMap::Iterator it = m_mapEntries.find( path );
  if ( it == m_mapEntries.end() )
    return 0;
  else
    return &(*it);
}

// set polling frequency for a entry and adjust global freq if needed
void KDirWatchPrivate::useFreq(Entry* e, int newFreq)
{
  e->freq = newFreq;

  // a reasonable frequency for the global polling timer
  if (e->freq < freq) {
    freq = e->freq;
    if (timer.isActive()) timer.start(freq);
    qDebug() << "Global Poll Freq is now" << freq << "msec";
  }
}


#if HAVE_FAM
// setup FAM notification, returns false if not possible
bool KDirWatchPrivate::useFAM(Entry* e)
{
  if (!use_fam) return false;

  // handle FAM events to avoid deadlock
  // (FAM sends back all files in a directory when monitoring)
  famEventReceived();

  e->m_mode = FAMMode;
  e->dirty = false;

  if (e->isDir) {
    if (e->m_status == NonExistent) {
      // If the directory does not exist we watch the parent directory
      addEntry(0, e->parentDirectory(), e, true);
    }
    else {
      int res =FAMMonitorDirectory(&fc, QFile::encodeName(e->path).data(),
				   &(e->fr), e);
      if (res<0) {
	e->m_mode = UnknownMode;
	use_fam=false;
        delete sn; sn = 0;
	return false;
      }
      qDebug().nospace() << " Setup FAM (Req " << FAMREQUEST_GETREQNUM(&(e->fr))
                   << ") for " << e->path;
    }
  }
  else {
    if (e->m_status == NonExistent) {
      // If the file does not exist we watch the directory
      addEntry(0, QFileInfo(e->path).absolutePath(), e, true);
    }
    else {
      int res = FAMMonitorFile(&fc, QFile::encodeName(e->path).data(),
			       &(e->fr), e);
      if (res<0) {
	e->m_mode = UnknownMode;
	use_fam=false;
        delete sn; sn = 0;
	return false;
      }

      qDebug().nospace() << " Setup FAM (Req " << FAMREQUEST_GETREQNUM(&(e->fr))
                   << ") for " << e->path;
    }
  }

  // handle FAM events to avoid deadlock
  // (FAM sends back all files in a directory when monitoring)
  famEventReceived();

  return true;
}
#endif

#if HAVE_SYS_INOTIFY_H
// setup INotify notification, returns false if not possible
bool KDirWatchPrivate::useINotify( Entry* e )
{
  //qDebug() << "trying to use inotify for monitoring";

  e->wd = -1;
  e->dirty = false;

  if (!supports_inotify) return false;

  e->m_mode = INotifyMode;

  if ( e->m_status == NonExistent ) {
    addEntry(0, e->parentDirectory(), e, true);
    return true;
  }

  // May as well register for almost everything - it's free!
  int mask = IN_DELETE|IN_DELETE_SELF|IN_CREATE|IN_MOVE|IN_MOVE_SELF|IN_DONT_FOLLOW|IN_MOVED_FROM|IN_MODIFY|IN_ATTRIB;

  if ( ( e->wd = inotify_add_watch( m_inotify_fd,
                                    QFile::encodeName( e->path ).data(), mask) ) >= 0)
  {
    if (s_verboseDebug) {
      qDebug() << "inotify successfully used for monitoring" << e->path << "wd=" << e->wd;
    }
    return true;
  }

   qDebug() << "inotify failed for monitoring" << e->path << ":" << strerror(errno);
  return false;
}
#endif
#if HAVE_QFILESYSTEMWATCHER
bool KDirWatchPrivate::useQFSWatch(Entry* e)
{
  e->m_mode = QFSWatchMode;
  e->dirty = false;

  if ( e->m_status == NonExistent ) {
    addEntry(0, e->parentDirectory(), e, true /*isDir*/);
    return true;
  }

  qDebug() << "fsWatcher->addPath" << e->path;
  if (!fsWatcher) {
      fsWatcher = new KFileSystemWatcher();
      connect(fsWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(fswEventReceived(QString)));
      connect(fsWatcher, SIGNAL(fileChanged(QString)),      this, SLOT(fswEventReceived(QString)));
  }
  fsWatcher->addPath( e->path );
  return true;
}
#endif

bool KDirWatchPrivate::useStat(Entry* e)
{
  if (KFileSystemType::fileSystemType(e->path) == KFileSystemType::Nfs) // TODO: or Smbfs?
    useFreq(e, m_nfsPollInterval);
  else
    useFreq(e, m_PollInterval);

  if (e->m_mode != StatMode) {
    e->m_mode = StatMode;
    statEntries++;

    if ( statEntries == 1 ) {
      // if this was first STAT entry (=timer was stopped)
      timer.start(freq);      // then start the timer
      qDebug() << " Started Polling Timer, freq " << freq;
    }
  }

  qDebug() << " Setup Stat (freq " << e->freq << ") for " << e->path;

  return true;
}


/* If <instance> !=0, this KDirWatch instance wants to watch at <_path>,
 * providing in <isDir> the type of the entry to be watched.
 * Sometimes, entries are dependant on each other: if <sub_entry> !=0,
 * this entry needs another entry to watch himself (when notExistent).
 */
void KDirWatchPrivate::addEntry(KDirWatch* instance, const QString& _path,
                Entry* sub_entry, bool isDir, KDirWatch::WatchModes watchModes)
{
  QString path (_path);
  if (path.isEmpty()
#ifndef Q_OS_WIN
     || path == QLatin1String("/dev")
     || (path.startsWith(QLatin1String("/dev/")) && !path.startsWith(QLatin1String("/dev/.")))
#endif
  )
    return; // Don't even go there.

  if ( path.length() > 1 && path.endsWith( QLatin1Char( '/' ) ) )
    path.truncate( path.length() - 1 );

  EntryMap::Iterator it = m_mapEntries.find( path );
  if ( it != m_mapEntries.end() )
  {
    if (sub_entry) {
       (*it).m_entries.append(sub_entry);
       if (s_verboseDebug) {
         qDebug() << "Added already watched Entry" << path
                      << "(for" << sub_entry->path << ")";
       }
#if HAVE_SYS_INOTIFY_H
       Entry* e = &(*it);
       if( (e->m_mode == INotifyMode) && (e->wd >= 0) ) {
         int mask = IN_DELETE|IN_DELETE_SELF|IN_CREATE|IN_MOVE|IN_MOVE_SELF|IN_DONT_FOLLOW;
         if(!e->isDir)
           mask |= IN_MODIFY|IN_ATTRIB;
         else
           mask |= IN_ONLYDIR;

         inotify_rm_watch (m_inotify_fd, e->wd);
         e->wd = inotify_add_watch( m_inotify_fd, QFile::encodeName( e->path ).data(),
                                    mask);
         //Q_ASSERT(e->wd >= 0); // fails in KDirListerTest::testDeleteCurrentDir
       }
#endif
    }
    else {
       (*it).addClient(instance, watchModes);
       if (s_verboseDebug) {
         qDebug() << "Added already watched Entry" << path
                      << "(now" <<  (*it).clientCount() << "clients)"
                      << QString::fromLatin1("[%1]").arg(instance->objectName());
       }
    }
    return;
  }

  // we have a new path to watch

  QT_STATBUF stat_buf;
  bool exists = (QT_STAT(QFile::encodeName(path).constData(), &stat_buf) == 0);

  EntryMap::iterator newIt = m_mapEntries.insert( path, Entry() );
  // the insert does a copy, so we have to use <e> now
  Entry* e = &(*newIt);

  if (exists) {
    e->isDir = S_ISDIR(stat_buf.st_mode);

    if (e->isDir && !isDir) {
      if (QT_LSTAT(QFile::encodeName(path).constData(), &stat_buf) == 0) {
        if (S_ISLNK(stat_buf.st_mode))
          // if it's a symlink, don't follow it
          e->isDir = false;
        else
          qWarning() << "KDirWatch:" << path << "is a directory. Use addDir!";
      }
    } else if (!e->isDir && isDir)
      qWarning("KDirWatch: %s is a file. Use addFile!", qPrintable(path));

    if (!e->isDir && ( watchModes != KDirWatch::WatchDirOnly)) {
      qWarning() << "KDirWatch:" << path << "is a file. You can't use recursive or "
                    "watchFiles options";
      watchModes = KDirWatch::WatchDirOnly;
    }

#ifdef Q_OS_WIN
    // ctime is the 'creation time' on windows - use mtime instead
    e->m_ctime = stat_buf.st_mtime;
#else
    e->m_ctime = stat_buf.st_ctime;
#endif
    e->m_status = Normal;
    e->m_nlink = stat_buf.st_nlink;
    e->m_ino = stat_buf.st_ino;
  }
  else {
    e->isDir = isDir;
    e->m_ctime = invalid_ctime;
    e->m_status = NonExistent;
    e->m_nlink = 0;
    e->m_ino = 0;
  }

  e->path = path;
  if (sub_entry)
    e->m_entries.append(sub_entry);
  else
    e->addClient(instance, watchModes);

  qDebug().nospace() << "Added " << (e->isDir ? "Dir " : "File ") << path
    << (e->m_status == NonExistent ? " NotExisting" : "")
    << " for " << (sub_entry ? sub_entry->path : QString())
    << " [" << (instance ? instance->objectName() : QString()) << "]";

  // now setup the notification method
  e->m_mode = UnknownMode;
  e->msecLeft = 0;

  if ( isNoisyFile( QFile::encodeName( path ).data() ) )
    return;

  if (exists && e->isDir && (watchModes != KDirWatch::WatchDirOnly)) {
    QFlags<QDir::Filter> filters = QDir::NoDotAndDotDot;

    if ((watchModes & KDirWatch::WatchSubDirs) &&
        (watchModes & KDirWatch::WatchFiles)) {
      filters |= (QDir::Dirs|QDir::Files);
    } else if (watchModes & KDirWatch::WatchSubDirs) {
      filters |= QDir::Dirs;
    } else if (watchModes & KDirWatch::WatchFiles) {
      filters |= QDir::Files;
    }

#if HAVE_SYS_INOTIFY_H
    if (e->m_mode == INotifyMode || (e->m_mode == UnknownMode && m_preferredMethod == KDirWatch::INotify)  )
    {
        //qDebug() << "Ignoring WatchFiles directive - this is implicit with inotify";
        // Placing a watch on individual files is redundant with inotify
        // (inotify gives us WatchFiles functionality "for free") and indeed
        // actively harmful, so prevent it.  WatchSubDirs is necessary, though.
        filters &= ~QDir::Files;
    }
#endif

    QDir basedir (e->path);
    const QFileInfoList contents = basedir.entryInfoList(filters);
    for (QFileInfoList::const_iterator iter = contents.constBegin();
         iter != contents.constEnd(); ++iter)
    {
      const QFileInfo &fileInfo = *iter;
      // treat symlinks as files--don't follow them.
      bool isDir = fileInfo.isDir() && !fileInfo.isSymLink();

      addEntry (instance, fileInfo.absoluteFilePath(), 0, isDir,
                isDir ? watchModes : KDirWatch::WatchDirOnly);
    }
  }

  addWatch(e);
}

void KDirWatchPrivate::addWatch(Entry* e)
{
  // If the watch is on a network filesystem use the nfsPreferredMethod as the
  // default, otherwise use preferredMethod as the default, if the methods are
  // the same we can skip the mountpoint check

  // This allows to configure a different method for NFS mounts, since inotify
  // cannot detect changes made by other machines. However as a default inotify
  // is fine, since the most common case is a NFS-mounted home, where all changes
  // are made locally. #177892.
  KDirWatch::Method preferredMethod = m_preferredMethod;
  if (m_nfsPreferredMethod != m_preferredMethod) {
    if (KFileSystemType::fileSystemType(e->path) == KFileSystemType::Nfs) {
      preferredMethod = m_nfsPreferredMethod;
    }
  }

  // Try the appropriate preferred method from the config first
  bool entryAdded = false;
  switch (preferredMethod) {
#if HAVE_FAM
    case KDirWatch::FAM: entryAdded = useFAM(e); break;
#endif
#if HAVE_SYS_INOTIFY_H
    case KDirWatch::INotify: entryAdded = useINotify(e); break;
#endif
#if HAVE_QFILESYSTEMWATCHER
    case KDirWatch::QFSWatch: entryAdded = useQFSWatch(e); break;
#endif
    case KDirWatch::Stat: entryAdded = useStat(e); break;
    default: break;
  }

  // Failing that try in order INotify, FAM, QFSWatch, Stat
  if (!entryAdded) {
#if HAVE_SYS_INOTIFY_H
    if (useINotify(e)) return;
#endif
#if HAVE_FAM
    if (useFAM(e)) return;
#endif
#if HAVE_QFILESYSTEMWATCHER
    if (useQFSWatch(e)) return;
#endif
    useStat(e);
  }
}

void KDirWatchPrivate::removeWatch(Entry* e)
{
#if HAVE_FAM
    if (e->m_mode == FAMMode) {
        FAMCancelMonitor(&fc, &(e->fr) );
        qDebug().nospace()  << "Cancelled FAM (Req " << FAMREQUEST_GETREQNUM(&(e->fr))
                                << ") for " << e->path;
    }
#endif
#if HAVE_SYS_INOTIFY_H
    if (e->m_mode == INotifyMode) {
        (void) inotify_rm_watch( m_inotify_fd, e->wd );
        if (s_verboseDebug) {
            qDebug().nospace() << "Cancelled INotify (fd " << m_inotify_fd << ", "
                                   << e->wd << ") for " << e->path;
        }
    }
#endif
#if HAVE_QFILESYSTEMWATCHER
    if (e->m_mode == QFSWatchMode && fsWatcher) {
        if (s_verboseDebug)
            qDebug() << "fsWatcher->removePath" << e->path;
        fsWatcher->removePath(e->path);
    }
#endif
}

void KDirWatchPrivate::removeEntry(KDirWatch* instance,
                                   const QString& _path,
                                   Entry* sub_entry)
{
  if (s_verboseDebug) {
    qDebug() << "path=" << _path << "sub_entry:" << sub_entry;
  }
  Entry* e = entry(_path);
  if (!e) {
    qWarning() << "doesn't know" << _path;
    return;
  }

  removeEntry(instance, e, sub_entry);
}

void KDirWatchPrivate::removeEntry(KDirWatch* instance,
                                   Entry* e,
                                   Entry* sub_entry)
{
  removeList.remove(e);

  if (sub_entry)
    e->m_entries.removeAll(sub_entry);
  else
    e->removeClient(instance);

  if (e->m_clients.count() || e->m_entries.count())
    return;

  if (delayRemove) {
    removeList.insert(e);
    // now e->isValid() is false
    return;
  }

    if ( e->m_status == Normal) {
        removeWatch(e);
    } else {
        // Removed a NonExistent entry - we just remove it from the parent
        if (e->isDir)
            removeEntry(0, e->parentDirectory(), e);
        else
            removeEntry(0, QFileInfo(e->path).absolutePath(), e);
    }

  if (e->m_mode == StatMode) {
    statEntries--;
    if ( statEntries == 0 ) {
      timer.stop(); // stop timer if lists are empty
      qDebug() << " Stopped Polling Timer";
    }
  }

  if (s_verboseDebug) {
    qDebug().nospace() << "Removed " << (e->isDir ? "Dir ":"File ") << e->path
                           << " for " << (sub_entry ? sub_entry->path : QString())
                           << " [" << (instance ? instance->objectName() : QString()) << "]";
  }
  m_mapEntries.remove( e->path ); // <e> not valid any more
}


/* Called from KDirWatch destructor:
 * remove <instance> as client from all entries
 */
void KDirWatchPrivate::removeEntries( KDirWatch* instance )
{
  int minfreq = 3600000;

  QStringList pathList;
  // put all entries where instance is a client in list
  EntryMap::Iterator it = m_mapEntries.begin();
  for( ; it != m_mapEntries.end(); ++it ) {
    Client* c = 0;
    Q_FOREACH(Client* client, (*it).m_clients) {
      if (client->instance == instance) {
        c = client;
        break;
      }
    }
    if (c) {
      c->count = 1; // forces deletion of instance as client
      pathList.append((*it).path);
    }
    else if ( (*it).m_mode == StatMode && (*it).freq < minfreq )
      minfreq = (*it).freq;
  }

  Q_FOREACH(const QString &path, pathList)
    removeEntry(instance, path, 0);

  if (minfreq > freq) {
    // we can decrease the global polling frequency
    freq = minfreq;
    if (timer.isActive()) timer.start(freq);
    qDebug() << "Poll Freq now" << freq << "msec";
  }
}

// instance ==0: stop scanning for all instances
bool KDirWatchPrivate::stopEntryScan( KDirWatch* instance, Entry* e)
{
  int stillWatching = 0;
  Q_FOREACH(Client* client, e->m_clients) {
    if (!instance || instance == client->instance)
      client->watchingStopped = true;
    else if (!client->watchingStopped)
      stillWatching += client->count;
  }

  qDebug()  << (instance ? instance->objectName() : QString::fromLatin1("all"))
                << "stopped scanning" << e->path << "(now"
                << stillWatching << "watchers)";

  if (stillWatching == 0) {
    // if nobody is interested, we don't watch
    if ( e->m_mode != INotifyMode ) {
      e->m_ctime = invalid_ctime; // invalid
      e->m_status = NonExistent;
    }
    //    e->m_status = Normal;
  }
  return true;
}

// instance ==0: start scanning for all instances
bool KDirWatchPrivate::restartEntryScan( KDirWatch* instance, Entry* e,
					 bool notify)
{
  int wasWatching = 0, newWatching = 0;
  Q_FOREACH(Client* client, e->m_clients) {
    if (!client->watchingStopped)
      wasWatching += client->count;
    else if (!instance || instance == client->instance) {
      client->watchingStopped = false;
      newWatching += client->count;
    }
  }
  if (newWatching == 0)
    return false;

  qDebug()  << (instance ? instance->objectName() : QString::fromLatin1("all"))
                << "restarted scanning" << e->path
                << "(now" << wasWatching+newWatching << "watchers)";

  // restart watching and emit pending events

  int ev = NoChange;
  if (wasWatching == 0) {
    if (!notify) {
      QT_STATBUF stat_buf;
      bool exists = (QT_STAT(QFile::encodeName(e->path).constData(), &stat_buf) == 0);
      if (exists) {
#ifdef Q_OS_WIN
        // ctime is the 'creation time' on windows - use mtime instead
        e->m_ctime = stat_buf.st_mtime;
#else
        e->m_ctime = stat_buf.st_ctime;
#endif
        e->m_status = Normal;
        if (s_verboseDebug) {
          qDebug() << "Setting status to Normal for" << e << e->path;
        }
        e->m_nlink = stat_buf.st_nlink;
        e->m_ino = stat_buf.st_ino;

        // Same as in scanEntry: ensure no subentry in parent dir
        removeEntry(0, e->parentDirectory(), e);
      }
      else {
        e->m_ctime = invalid_ctime;
        e->m_status = NonExistent;
        e->m_nlink = 0;
        if (s_verboseDebug) {
          qDebug() << "Setting status to NonExistent for" << e << e->path;
        }
      }
    }
    e->msecLeft = 0;
    ev = scanEntry(e);
  }
  emitEvent(e,ev);

  return true;
}

// instance ==0: stop scanning for all instances
void KDirWatchPrivate::stopScan(KDirWatch* instance)
{
  EntryMap::Iterator it = m_mapEntries.begin();
  for( ; it != m_mapEntries.end(); ++it )
    stopEntryScan(instance, &(*it));
}


void KDirWatchPrivate::startScan(KDirWatch* instance,
                                 bool notify, bool skippedToo )
{
  if (!notify)
    resetList(instance,skippedToo);

  EntryMap::Iterator it = m_mapEntries.begin();
  for( ; it != m_mapEntries.end(); ++it )
    restartEntryScan(instance, &(*it), notify);

  // timer should still be running when in polling mode
}


// clear all pending events, also from stopped
void KDirWatchPrivate::resetList( KDirWatch* /*instance*/, bool skippedToo )
{
  EntryMap::Iterator it = m_mapEntries.begin();
  for( ; it != m_mapEntries.end(); ++it ) {

    Q_FOREACH(Client* client, (*it).m_clients) {
      if (!client->watchingStopped || skippedToo)
        client->pending = NoChange;
    }
  }
}

// Return event happened on <e>
//
int KDirWatchPrivate::scanEntry(Entry* e)
{
  // Shouldn't happen: Ignore "unknown" notification method
  if (e->m_mode == UnknownMode) return NoChange;

  if (e->m_mode == FAMMode || e->m_mode == INotifyMode) {
    // we know nothing has changed, no need to stat
    if(!e->dirty) return NoChange;
    e->dirty = false;
  }

  if (e->m_mode == StatMode) {
    // only scan if timeout on entry timer happens;
    // e.g. when using 500msec global timer, a entry
    // with freq=5000 is only watched every 10th time

    e->msecLeft -= freq;
    if (e->msecLeft>0) return NoChange;
    e->msecLeft += e->freq;
  }

  QT_STATBUF stat_buf;
  const bool exists = (QT_STAT(QFile::encodeName(e->path).constData(), &stat_buf) == 0);
  if (exists) {

    if (e->m_status == NonExistent) {
      // ctime is the 'creation time' on windows, but with qMax
      // we get the latest change of any kind, on any platform.
      e->m_ctime = qMax(stat_buf.st_ctime, stat_buf.st_mtime);
      e->m_status = Normal;
      e->m_ino = stat_buf.st_ino;
      if (s_verboseDebug) {
        qDebug() << "Setting status to Normal for just created" << e << e->path;
      }
      // We need to make sure the entry isn't listed in its parent's subentries... (#222974, testMoveTo)
      removeEntry(0, e->parentDirectory(), e);

      return Created;
    }

#if 1 // for debugging the if() below
    if (s_verboseDebug) {
      struct tm* tmp = localtime(&e->m_ctime);
      char outstr[200];
      strftime(outstr, sizeof(outstr), "%T", tmp);
      qDebug() << "e->m_ctime=" << e->m_ctime << outstr
                   << "stat_buf.st_ctime=" << stat_buf.st_ctime
                   << "e->m_nlink=" << e->m_nlink
                   << "stat_buf.st_nlink=" << stat_buf.st_nlink
                   << "e->m_ino=" << e->m_ino
                   << "stat_buf.st_ino=" << stat_buf.st_ino;
    }
#endif

    if ( ((e->m_ctime != invalid_ctime) &&
          (qMax(stat_buf.st_ctime, stat_buf.st_mtime) != e->m_ctime ||
           stat_buf.st_ino != e->m_ino ||
           stat_buf.st_nlink != nlink_t(e->m_nlink)))
#ifdef Q_OS_WIN
          // we trust QFSW to get it right, the ctime comparisons above
          // fail for example when adding files to directories on Windows
          // which doesn't change the mtime of the directory
        || e->m_mode == QFSWatchMode
#endif
    ) {
      e->m_ctime = qMax(stat_buf.st_ctime, stat_buf.st_mtime);
      e->m_nlink = stat_buf.st_nlink;
      if (e->m_ino != stat_buf.st_ino) {
          // The file got deleted and recreated. We need to watch it again.
          removeWatch(e);
          addWatch(e);
      }
      e->m_ino = stat_buf.st_ino;
      return Changed;
    }

    return NoChange;
  }

  // dir/file doesn't exist

  e->m_nlink = 0;
  e->m_ino = 0;
  e->m_status = NonExistent;

  if (e->m_ctime == invalid_ctime) {
    return NoChange;
  }

  e->m_ctime = invalid_ctime;
  return Deleted;
}

/* Notify all interested KDirWatch instances about a given event on an entry
 * and stored pending events. When watching is stopped, the event is
 * added to the pending events.
 */
void KDirWatchPrivate::emitEvent(const Entry* e, int event, const QString &fileName)
{
  QString path (e->path);
  if (!fileName.isEmpty()) {
    if (!QDir::isRelativePath(fileName))
      path = fileName;
    else {
#ifdef Q_OS_UNIX
      path += QLatin1Char('/') + fileName;
#elif defined(Q_OS_WIN)
      //current drive is passed instead of /
      path += QDir::currentPath().left(2) + QLatin1Char('/') + fileName;
#endif
    }
  }

  if (s_verboseDebug) {
    qDebug() << event << path << e->m_clients.count() << "clients";
  }

  Q_FOREACH(Client* c, e->m_clients)
  {
    if (c->instance==0 || c->count==0) continue;

    if (c->watchingStopped) {
      // add event to pending...
      if (event == Changed)
        c->pending |= event;
      else if (event == Created || event == Deleted)
        c->pending = event;
      continue;
    }
    // not stopped
    if (event == NoChange || event == Changed)
      event |= c->pending;
    c->pending = NoChange;
    if (event == NoChange) continue;

    // Emit the signals delayed, to avoid unexpected re-entrancy from the slots (#220153)

    if (event & Deleted) {
      QMetaObject::invokeMethod(c->instance, "setDeleted", Qt::QueuedConnection, Q_ARG(QString, path));
      // emit only Deleted event...
      continue;
    }

    if (event & Created) {
      QMetaObject::invokeMethod(c->instance, "setCreated", Qt::QueuedConnection, Q_ARG(QString, path));
      // possible emit Change event after creation
    }

    if (event & Changed) {
      QMetaObject::invokeMethod(c->instance, "setDirty", Qt::QueuedConnection, Q_ARG(QString, path));
    }
  }
}

// Remove entries which were marked to be removed
void KDirWatchPrivate::slotRemoveDelayed()
{
  delayRemove = false;
  // Removing an entry could also take care of removing its parent
  // (e.g. in FAM or inotify mode), which would remove other entries in removeList,
  // so don't use Q_FOREACH or iterators here...
  while (!removeList.isEmpty()) {
    Entry* entry = *removeList.begin();
    removeEntry(0, entry, 0); // this will remove entry from removeList
  }
}

/* Scan all entries to be watched for changes. This is done regularly
 * when polling. FAM and inotify use a single-shot timer to call this slot delayed.
 */
void KDirWatchPrivate::slotRescan()
{
  if (s_verboseDebug)
    qDebug();

  EntryMap::Iterator it;

  // People can do very long things in the slot connected to dirty(),
  // like showing a message box. We don't want to keep polling during
  // that time, otherwise the value of 'delayRemove' will be reset.
  // ### TODO: now the emitEvent delays emission, this can be cleaned up
  bool timerRunning = timer.isActive();
  if ( timerRunning )
    timer.stop();

  // We delay deletions of entries this way.
  // removeDir(), when called in slotDirty(), can cause a crash otherwise
  // ### TODO: now the emitEvent delays emission, this can be cleaned up
  delayRemove = true;

  if (rescan_all)
  {
    // mark all as dirty
    it = m_mapEntries.begin();
    for( ; it != m_mapEntries.end(); ++it )
      (*it).dirty = true;
    rescan_all = false;
  }
  else
  {
    // progate dirty flag to dependant entries (e.g. file watches)
    it = m_mapEntries.begin();
    for( ; it != m_mapEntries.end(); ++it )
      if (((*it).m_mode == INotifyMode || (*it).m_mode == QFSWatchMode) && (*it).dirty )
        (*it).propagate_dirty();
  }

#if HAVE_SYS_INOTIFY_H
  QList<Entry*> cList;
#endif

  it = m_mapEntries.begin();
  for( ; it != m_mapEntries.end(); ++it ) {
    // we don't check invalid entries (i.e. remove delayed)
    Entry* entry = &(*it);
    if (!entry->isValid()) continue;

    const int ev = scanEntry(entry);
    if (s_verboseDebug)
      qDebug() << "scanEntry for" << entry->path << "says" << ev;

    switch(entry->m_mode) {
#if HAVE_SYS_INOTIFY_H
    case INotifyMode:
      if ( ev == Deleted ) {
        if (s_verboseDebug)
          qDebug() << "scanEntry says" << entry->path << "was deleted";
        addEntry(0, entry->parentDirectory(), entry, true);
      } else if (ev == Created) {
        if (s_verboseDebug)
          qDebug() << "scanEntry says" << entry->path << "was created. wd=" << entry->wd;
        if (entry->wd < 0) {
          cList.append(entry);
          addWatch(entry);
        }
      }
      break;
#endif
    case FAMMode:
    case QFSWatchMode:
      if (ev == Created) {
        addWatch(entry);
      }
      break;
    default:
      // dunno about StatMode...
      break;
    }

#if HAVE_SYS_INOTIFY_H
    if (entry->isDir) {
      // Report and clear the the list of files that have changed in this directory.
      // Remove duplicates by changing to set and back again:
      // we don't really care about preserving the order of the
      // original changes.
      QStringList pendingFileChanges = entry->m_pendingFileChanges;
      pendingFileChanges.removeDuplicates();
      Q_FOREACH(const QString &changedFilename, pendingFileChanges) {
        if (s_verboseDebug) {
          qDebug() << "processing pending file change for" << changedFilename;
        }
        emitEvent(entry, Changed, changedFilename);
      }
      entry->m_pendingFileChanges.clear();
    }
#endif

    if ( ev != NoChange ) {
      emitEvent(entry, ev);
    }
  }

  if ( timerRunning )
    timer.start(freq);

#if HAVE_SYS_INOTIFY_H
  // Remove watch of parent of new created directories
  Q_FOREACH(Entry* e, cList)
    removeEntry(0, e->parentDirectory(), e);
#endif

  QTimer::singleShot(0, this, SLOT(slotRemoveDelayed()));
}

bool KDirWatchPrivate::isNoisyFile( const char * filename )
{
  // $HOME/.X.err grows with debug output, so don't notify change
  if ( *filename == '.') {
    if (strncmp(filename, ".X.err", 6) == 0) return true;
    if (strncmp(filename, ".xsession-errors", 16) == 0) return true;
    // fontconfig updates the cache on every KDE app start
    // (inclusive kio_thumbnail slaves)
    if (strncmp(filename, ".fonts.cache", 12) == 0) return true;
  }

  return false;
}

#if HAVE_FAM
void KDirWatchPrivate::famEventReceived()
{
  static FAMEvent fe;

  delayRemove = true;

  //qDebug() << "Fam event received";

  while(use_fam && FAMPending(&fc)) {
    if (FAMNextEvent(&fc, &fe) == -1) {
      qWarning() << "FAM connection problem, switching to polling.";
      use_fam = false;
      delete sn; sn = 0;

      // Replace all FAMMode entries with INotify/Stat
      EntryMap::Iterator it = m_mapEntries.begin();
      for( ; it != m_mapEntries.end(); ++it )
        if ((*it).m_mode == FAMMode && (*it).m_clients.count()>0) {
            Entry* e = &(*it);
            addWatch(e);
        }
    }
    else
      checkFAMEvent(&fe);
  }

  QTimer::singleShot(0, this, SLOT(slotRemoveDelayed()));
}

void KDirWatchPrivate::checkFAMEvent(FAMEvent* fe)
{
  //qDebug();

  // Don't be too verbose ;-)
  if ((fe->code == FAMExists) ||
      (fe->code == FAMEndExist) ||
      (fe->code == FAMAcknowledge)) return;

  if ( isNoisyFile( fe->filename ) )
    return;

  Entry* e = 0;
  EntryMap::Iterator it = m_mapEntries.begin();
  for( ; it != m_mapEntries.end(); ++it )
    if (FAMREQUEST_GETREQNUM(&( (*it).fr )) ==
       FAMREQUEST_GETREQNUM(&(fe->fr)) ) {
      e = &(*it);
      break;
    }

  // Entry* e = static_cast<Entry*>(fe->userdata);

  if (s_verboseDebug) { // don't enable this except when debugging, see #88538
    qDebug()  << "Processing FAM event ("
                << ((fe->code == FAMChanged) ? "FAMChanged" :
                    (fe->code == FAMDeleted) ? "FAMDeleted" :
                    (fe->code == FAMStartExecuting) ? "FAMStartExecuting" :
                    (fe->code == FAMStopExecuting) ? "FAMStopExecuting" :
                    (fe->code == FAMCreated) ? "FAMCreated" :
                    (fe->code == FAMMoved) ? "FAMMoved" :
                    (fe->code == FAMAcknowledge) ? "FAMAcknowledge" :
                    (fe->code == FAMExists) ? "FAMExists" :
                    (fe->code == FAMEndExist) ? "FAMEndExist" : "Unknown Code")
                  << ", " << fe->filename
                  << ", Req " << FAMREQUEST_GETREQNUM(&(fe->fr)) << ") e=" << e;
  }

  if (!e) {
    // this happens e.g. for FAMAcknowledge after deleting a dir...
    //    qDebug() << "No entry for FAM event ?!";
    return;
  }

  if (e->m_status == NonExistent) {
    qDebug() << "FAM event for nonExistent entry " << e->path;
    return;
  }

  // Delayed handling. This rechecks changes with own stat calls.
  e->dirty = true;
  if (!rescan_timer.isActive())
    rescan_timer.start(m_PollInterval); // singleshot

    // needed FAM control actions on FAM events
    switch (fe->code) {
    case FAMDeleted:
        // fe->filename is an absolute path when a watched file-or-dir is deleted
        if (!QDir::isRelativePath(QFile::decodeName(fe->filename))) {
          FAMCancelMonitor(&fc, &(e->fr) ); // needed ?
          qDebug()  << "Cancelled FAMReq"
                        << FAMREQUEST_GETREQNUM(&(e->fr))
                        << "for" << e->path;
          e->m_status = NonExistent;
          e->m_ctime = invalid_ctime;
          emitEvent(e, Deleted, e->path);
          // If the parent dir was already watched, tell it something changed
          Entry* parentEntry = entry(e->parentDirectory());
          if (parentEntry)
              parentEntry->dirty = true;
          // Add entry to parent dir to notice if the entry gets recreated
          addEntry(0, e->parentDirectory(), e, true /*isDir*/);
        } else {
            // A file in this directory has been removed, and wasn't explicitly watched.
            // We could still inform clients, like inotify does? But stat can't.
            // For now we just marked e dirty and slotRescan will emit the dir as dirty.
            //qDebug() << "Got FAMDeleted for" << QFile::decodeName(fe->filename) << "in" << e->path << ". Absolute path -> NOOP!";
        }
        break;

      case FAMCreated: {
          // check for creation of a directory we have to watch
        QString tpath(e->path + QLatin1Char('/') + QFile::decodeName(fe->filename));

        // This code is very similar to the one in inotifyEventReceived...
        Entry* sub_entry = e->findSubEntry(tpath);
        if (sub_entry /*&& sub_entry->isDir*/) {
          // We were waiting for this new file/dir to be created
          emitEvent(sub_entry, Created);
          sub_entry->dirty = true;
          rescan_timer.start(0); // process this asap, to start watching that dir
        } else if (e->isDir && !e->m_clients.empty()) {
          bool isDir = false;
          const QList<Client *> clients = e->clientsForFileOrDir(tpath, &isDir);
          Q_FOREACH(Client *client, clients) {
            addEntry (client->instance, tpath, 0, isDir,
                      isDir ? client->m_watchModes : KDirWatch::WatchDirOnly);
          }

          if (!clients.isEmpty()) {
            emitEvent(e, Created, tpath);

            qDebug().nospace() << clients.count() << " instance(s) monitoring the new "
                                   << (isDir ? "dir " : "file ") << tpath;
          }
        }
      }
        break;
      default:
        break;
    }
}
#else
void KDirWatchPrivate::famEventReceived()
{
    qWarning () << "Fam event received but FAM is not supported";
}
#endif


void KDirWatchPrivate::statistics()
{
  EntryMap::Iterator it;

  qDebug() << "Entries watched:";
  if (m_mapEntries.count()==0) {
    qDebug() << "  None.";
  }
  else {
    it = m_mapEntries.begin();
    for( ; it != m_mapEntries.end(); ++it ) {
      Entry* e = &(*it);
      qDebug() << "  " << *e;

      Q_FOREACH(Client* c, e->m_clients) {
        QByteArray pending;
        if (c->watchingStopped) {
          if (c->pending & Deleted) pending += "deleted ";
          if (c->pending & Created) pending += "created ";
          if (c->pending & Changed) pending += "changed ";
          if (!pending.isEmpty()) pending = " (pending: " + pending + ')';
          pending = ", stopped" + pending;
        }
        qDebug()  << "    by " << c->instance->objectName()
                  << " (" << c->count << " times)" << pending;
      }
      if (e->m_entries.count()>0) {
        qDebug() << "    dependent entries:";
        Q_FOREACH(Entry *d, e->m_entries) {
          qDebug() << "      " << d << d->path << (d->m_status == NonExistent ? "NonExistent" : "EXISTS!!! ERROR!");
          if (s_verboseDebug) {
            Q_ASSERT(d->m_status == NonExistent); // it doesn't belong here otherwise
          }
        }
      }
    }
  }
}

#if HAVE_QFILESYSTEMWATCHER
// Slot for QFileSystemWatcher
void KDirWatchPrivate::fswEventReceived(const QString &path)
{
  if (s_verboseDebug)
    qDebug() << path;
  EntryMap::Iterator it = m_mapEntries.find(path);
  if(it != m_mapEntries.end()) {
    Entry* e = &(*it);
    e->dirty = true;
    const int ev = scanEntry(e);
    if (s_verboseDebug)
      qDebug() << "scanEntry for" << e->path << "says" << ev;
    if (ev != NoChange)
      emitEvent(e, ev);
    if(ev == Deleted) {
      if (e->isDir)
        addEntry(0, e->parentDirectory(), e, true);
      else
        addEntry(0, QFileInfo(e->path).absolutePath(), e, true);
    } else if (ev == Created) {
      // We were waiting for it to appear; now watch it
      addWatch(e);
    } else if (e->isDir) {
      // Check if any file or dir was created under this directory, that we were waiting for
      Q_FOREACH(Entry* sub_entry, e->m_entries) {
          fswEventReceived(sub_entry->path); // recurse, to call scanEntry and see if something changed
      }
    }
  }
}
#else
void KDirWatchPrivate::fswEventReceived(const QString &path)
{
    Q_UNUSED(path);
    qWarning () << "QFileSystemWatcher event received but QFileSystemWatcher is not supported";
}
#endif    // HAVE_QFILESYSTEMWATCHER

//
// Class KDirWatch
//

Q_GLOBAL_STATIC(KDirWatch, s_pKDirWatchSelf)
KDirWatch* KDirWatch::self()
{
  return s_pKDirWatchSelf();
}

// <steve> is this used anywhere?
// <dfaure> yes, see kio/src/core/kcoredirlister_p.h:328
bool KDirWatch::exists()
{
  return s_pKDirWatchSelf.exists();
}

static void cleanupQFSWatcher()
{
  s_pKDirWatchSelf()->deleteQFSWatcher();
}

KDirWatch::KDirWatch (QObject* parent)
  : QObject(parent), d(createPrivate())
{
  static int nameCounter = 0;

  nameCounter++;
  setObjectName(QString::fromLatin1("KDirWatch-%1").arg(nameCounter) );

  d->ref();

  d->_isStopped = false;

    static bool cleanupRegistered = false;
    if (!cleanupRegistered) {
        cleanupRegistered = true;
        // Must delete QFileSystemWatcher before qApp is gone - bug 261541
        qAddPostRoutine(cleanupQFSWatcher);
    }
}

KDirWatch::~KDirWatch()
{
  d->removeEntries(this);
  if ( d->deref() )
  {
    // delete it if it's the last one
    delete d;
    dwp_self = 0;
  }
}

void KDirWatch::addDir( const QString& _path, WatchModes watchModes)
{
  if (d) d->addEntry(this, _path, 0, true, watchModes);
}

void KDirWatch::addFile( const QString& _path )
{
  if ( !d )
	return;

  d->addEntry(this, _path, 0, false);
}

QDateTime KDirWatch::ctime( const QString &_path ) const
{
  KDirWatchPrivate::Entry* e = d->entry(_path);

  if (!e)
    return QDateTime();

  return QDateTime::fromTime_t(e->m_ctime);
}

void KDirWatch::removeDir( const QString& _path )
{
  if (d) d->removeEntry(this, _path, 0);
}

void KDirWatch::removeFile( const QString& _path )
{
  if (d) d->removeEntry(this, _path, 0);
}

bool KDirWatch::stopDirScan( const QString& _path )
{
  if (d) {
    KDirWatchPrivate::Entry *e = d->entry(_path);
    if (e && e->isDir) return d->stopEntryScan(this, e);
  }
  return false;
}

bool KDirWatch::restartDirScan( const QString& _path )
{
  if (d) {
    KDirWatchPrivate::Entry *e = d->entry(_path);
    if (e && e->isDir)
      // restart without notifying pending events
      return d->restartEntryScan(this, e, false);
  }
  return false;
}

void KDirWatch::stopScan()
{
  if (d) {
    d->stopScan(this);
    d->_isStopped = true;
  }
}

bool KDirWatch::isStopped()
{
  return d->_isStopped;
}

void KDirWatch::startScan( bool notify, bool skippedToo )
{
  if (d) {
    d->_isStopped = false;
    d->startScan(this, notify, skippedToo);
  }
}


bool KDirWatch::contains( const QString& _path ) const
{
  KDirWatchPrivate::Entry* e = d->entry(_path);
  if (!e)
     return false;

  Q_FOREACH(KDirWatchPrivate::Client* client, e->m_clients) {
    if (client->instance == this)
      return true;
  }

  return false;
}

void KDirWatch::deleteQFSWatcher()
{
  delete d->fsWatcher;
  d->fsWatcher = 0;
}

void KDirWatch::statistics()
{
  if (!dwp_self) {
    qDebug() << "KDirWatch not used";
    return;
  }
  dwp_self->statistics();
}


void KDirWatch::setCreated( const QString & _file )
{
  qDebug() << objectName() << "emitting created" << _file;
  emit created( _file );
}

void KDirWatch::setDirty( const QString & _file )
{
  //qDebug() << objectName() << "emitting dirty" << _file;
  emit dirty( _file );
}

void KDirWatch::setDeleted( const QString & _file )
{
  qDebug() << objectName() << "emitting deleted" << _file;
  emit deleted( _file );
}

KDirWatch::Method KDirWatch::internalMethod() const
{
  return d->m_preferredMethod;
}


#include "moc_kdirwatch.cpp"
#include "moc_kdirwatch_p.cpp"

//sven
