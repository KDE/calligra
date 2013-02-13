/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2007,2009-2010 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef ABSTRACTMODELSTREAMENCODER_P_H
#define ABSTRACTMODELSTREAMENCODER_P_H

#include "abstractmodelstreamencoder.h"


namespace Kasten2
{

class AbstractModelStreamEncoderPrivate
{
  public:
    explicit AbstractModelStreamEncoderPrivate( AbstractModelStreamEncoder* parent,
                                                const QString& remoteTypeName, const QString& remoteMimeType,
                                                const QString& remoteClipboardMimeType );

    virtual ~AbstractModelStreamEncoderPrivate();

  public:
    const QString& remoteTypeName() const;
    const QString& remoteMimeType() const;
    const QString& remoteClipboardMimeType() const;

  protected:
    AbstractModelStreamEncoder* const q_ptr;

    const QString mRemoteTypeName;
    const QString mRemoteMimeType;
    const QString mRemoteClipboardMimeType;
};


inline AbstractModelStreamEncoderPrivate::AbstractModelStreamEncoderPrivate( AbstractModelStreamEncoder* parent,
    const QString& remoteTypeName, const QString& remoteMimeType, const QString& remoteClipboardMimeType )
  : q_ptr( parent ),
    mRemoteTypeName( remoteTypeName ),
    mRemoteMimeType( remoteMimeType ),
    mRemoteClipboardMimeType( remoteClipboardMimeType.isEmpty() ? remoteMimeType : remoteClipboardMimeType )
{}

inline AbstractModelStreamEncoderPrivate::~AbstractModelStreamEncoderPrivate()
{}

inline const QString& AbstractModelStreamEncoderPrivate::remoteTypeName() const { return mRemoteTypeName; }
inline const QString& AbstractModelStreamEncoderPrivate::remoteMimeType() const { return mRemoteMimeType; }
inline const QString& AbstractModelStreamEncoderPrivate::remoteClipboardMimeType() const
{ return mRemoteClipboardMimeType; }

}

#endif
