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

#ifndef ABSTRACTFILESYSTEMLOADJOB_P_H
#define ABSTRACTFILESYSTEMLOADJOB_P_H

// lib
#include "abstractfilesystemloadjob.h"
#include <abstractloadjob_p.h>
// KDE
#include <KUrl>


namespace Kasten2
{

class AbstractFileSystemLoadJobPrivate : public AbstractLoadJobPrivate
{
  public:
    AbstractFileSystemLoadJobPrivate( AbstractFileSystemLoadJob* parent, AbstractModelFileSystemSynchronizer* synchronizer, const KUrl& url );

    virtual ~AbstractFileSystemLoadJobPrivate();

  public: // KJob API
    void start();

  public: // AbstractLoadJob API
    void setDocument( AbstractDocument* document );

  public:
    AbstractModelFileSystemSynchronizer* synchronizer() const;
    const KUrl& url() const;
    QFile* file() const;

  public: // slots
    void load();

  protected:
    Q_DECLARE_PUBLIC( AbstractFileSystemLoadJob )

  protected:
    AbstractModelFileSystemSynchronizer* const mSynchronizer;
    const KUrl mUrl;
    QFile* mFile;
    QString mWorkFilePath;
};


inline AbstractFileSystemLoadJobPrivate::AbstractFileSystemLoadJobPrivate( AbstractFileSystemLoadJob* parent,
    AbstractModelFileSystemSynchronizer* synchronizer, const KUrl& url )
  : AbstractLoadJobPrivate( parent ),
    mSynchronizer( synchronizer ),
    mUrl( url ),
    mFile( 0 )
{}

inline AbstractFileSystemLoadJobPrivate::~AbstractFileSystemLoadJobPrivate() {}

inline AbstractModelFileSystemSynchronizer* AbstractFileSystemLoadJobPrivate::synchronizer() const
{
    return mSynchronizer;
}
inline const KUrl& AbstractFileSystemLoadJobPrivate::url() const { return mUrl; }

inline QFile* AbstractFileSystemLoadJobPrivate::file()     const { return mFile; }
// TODO: setup a notification system

inline void AbstractFileSystemLoadJobPrivate::start()
{
    Q_Q( AbstractFileSystemLoadJob );

    QMetaObject::invokeMethod( q, "load", Qt::QueuedConnection );
}

}

#endif
