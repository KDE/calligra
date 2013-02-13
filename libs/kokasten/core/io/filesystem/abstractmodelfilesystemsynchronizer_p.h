/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2007-2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef ABSTRACTMODELFILESYSTEMSYNCHRONIZER_P_H
#define ABSTRACTMODELFILESYSTEMSYNCHRONIZER_P_H

#include "abstractmodelfilesystemsynchronizer.h"

// lib
#include <abstractmodelsynchronizer_p.h>
// Qt
#include <QtCore/QDateTime>


namespace Kasten2
{

class AbstractModelFileSystemSynchronizerPrivate : public AbstractModelSynchronizerPrivate
{
  public:
    explicit AbstractModelFileSystemSynchronizerPrivate( AbstractModelFileSystemSynchronizer* parent );

    virtual ~AbstractModelFileSystemSynchronizerPrivate();

  public:
    RemoteSyncState remoteSyncState() const;

  public:
    void setRemoteState( RemoteSyncState remoteState );
    void setFileDateTimeOnSync( const QDateTime& fileDateTime );

    void startFileWatching();
    void stopFileWatching();
    void pauseFileWatching();
    void unpauseFileWatching();

    void startNetworkWatching();
    void stopNetworkWatching();

  public:
    void onFileDirty( const QString& fileName );
    void onFileCreated( const QString& fileName );
    void onFileDeleted( const QString& fileName );
    void onNetworkConnect();
    void onNetworkDisconnect();

  protected:
    QDateTime mFileDateTime;
    RemoteSyncState mRemoteState;
    mutable KDirWatch* mDirWatch;

  protected:
    Q_DECLARE_PUBLIC( AbstractModelFileSystemSynchronizer )
};


inline AbstractModelFileSystemSynchronizerPrivate::AbstractModelFileSystemSynchronizerPrivate( AbstractModelFileSystemSynchronizer* parent )
  : AbstractModelSynchronizerPrivate( parent ),
    mRemoteState( RemoteUnknown ),
    mDirWatch( 0 )
{
}
inline RemoteSyncState AbstractModelFileSystemSynchronizerPrivate::remoteSyncState() const { return mRemoteState; }

inline void AbstractModelFileSystemSynchronizerPrivate::setRemoteState( RemoteSyncState remoteState )
{
    Q_Q( AbstractModelFileSystemSynchronizer );

    if( mRemoteState == remoteState )
        return;

    mRemoteState = remoteState;
    emit q->remoteSyncStateChanged( remoteState );
}
inline void AbstractModelFileSystemSynchronizerPrivate::setFileDateTimeOnSync( const QDateTime& fileDateTime )
{
    mFileDateTime = fileDateTime;
}

}

#endif
