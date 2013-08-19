/* This file is part of the KDE libraries
   Copyright (C) 2008 Jarosław Staniek <staniek@kde.org>

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

#include "kdirwatch_p.h"

#include <windows.h>

KFileSystemWatcher::KFileSystemWatcher()
 : QObject(), m_recentWatcher(0)
{
}

KFileSystemWatcher::~KFileSystemWatcher()
{
  qDeleteAll(m_watchers);
}

QFileSystemWatcher* KFileSystemWatcher::availableWatcher()
{
  QFileSystemWatcher* watcher = m_recentWatcher;
  if (!watcher || m_usedObjects.value(watcher) >= MAXIMUM_WAIT_OBJECTS) {
    uint i = 0;
    watcher = 0;
    for (QList<QFileSystemWatcher*>::ConstIterator watchersIt(m_watchers.constBegin());
      watchersIt!=m_watchers.constEnd(); ++watchersIt, i++)
    {
      if (m_usedObjects.value(*watchersIt) < MAXIMUM_WAIT_OBJECTS) {
        watcher = *watchersIt;
        m_recentWatcher = watcher;
        return watcher;
      }
    }
  }
  if (!watcher) { //new one needed
    watcher = new QFileSystemWatcher();
    connect(watcher, SIGNAL(directoryChanged(QString)), this, SIGNAL(directoryChanged(QString)));
    connect(watcher, SIGNAL(fileChanged(QString)), this, SIGNAL(fileChanged(QString)));
    m_watchers.append( watcher );
    m_usedObjects.insert(watcher, 0);
    m_recentWatcher = watcher;
  }
  return watcher;
}

void KFileSystemWatcher::addPath(const QString &file)
{
  QFileSystemWatcher* watcher = availableWatcher();
  watcher->addPath(file);
  m_usedObjects[watcher]++;
  m_paths.insert(file, watcher);
}

void KFileSystemWatcher::removePath(const QString &file)
{
  QFileSystemWatcher* watcher = m_paths.value(file);
  if (!watcher)
    return;
  watcher->removePath(file);
  m_usedObjects[watcher]--;
  if (m_recentWatcher != watcher)
    m_recentWatcher = 0;
}
