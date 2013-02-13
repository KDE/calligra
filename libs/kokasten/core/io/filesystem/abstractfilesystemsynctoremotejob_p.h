/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2008-2009,2011 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ABSTRACTFILESYSTEMSYNCTOREMOTEJOB_P_H
#define ABSTRACTFILESYSTEMSYNCTOREMOTEJOB_P_H

#include "abstractfilesystemsynctoremotejob.h"
#include <abstractsynctoremotejob_p.h>

// library
#include "abstractmodelfilesystemsynchronizer.h"
// KDE
#include <KTemporaryFile>
#include <KUrl>


namespace Kasten2
{
class AbstractFileSystemSyncToRemoteJobPrivate : public AbstractSyncToRemoteJobPrivate
{
  public:
    AbstractFileSystemSyncToRemoteJobPrivate( AbstractFileSystemSyncToRemoteJob* parent, AbstractModelFileSystemSynchronizer* synchronizer );

    virtual ~AbstractFileSystemSyncToRemoteJobPrivate();

  public: // KJob API
    void start();

  public:
    AbstractModelFileSystemSynchronizer* synchronizer() const;
    QFile* file() const;

  public:
    void completeWrite( bool success );

  public: // slots
    void syncToRemote();

  protected:
    Q_DECLARE_PUBLIC( AbstractFileSystemSyncToRemoteJob )

  protected:
    AbstractModelFileSystemSynchronizer* const mSynchronizer;
    QFile* mFile;
    QString mWorkFilePath;
};


inline AbstractFileSystemSyncToRemoteJobPrivate::AbstractFileSystemSyncToRemoteJobPrivate( AbstractFileSystemSyncToRemoteJob* parent,
    AbstractModelFileSystemSynchronizer* synchronizer )
  : AbstractSyncToRemoteJobPrivate( parent ),
    mSynchronizer( synchronizer ),
    mFile( 0 )
{}

inline QFile* AbstractFileSystemSyncToRemoteJobPrivate::file()     const { return mFile; }
// TODO: setup a notification system
inline AbstractModelFileSystemSynchronizer* AbstractFileSystemSyncToRemoteJobPrivate::synchronizer() const
{
    return mSynchronizer;
}

inline void AbstractFileSystemSyncToRemoteJobPrivate::start()
{
    Q_Q( AbstractFileSystemSyncToRemoteJob );

    QMetaObject::invokeMethod( q, "syncToRemote", Qt::QueuedConnection );
}


}

#endif
