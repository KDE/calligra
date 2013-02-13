/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2008-2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef ABSTRACTMODELEXPORTER_P_H
#define ABSTRACTMODELEXPORTER_P_H

#include "abstractmodelexporter.h"


namespace Kasten2
{

class AbstractModelExporterPrivate
{
  public:
    AbstractModelExporterPrivate( AbstractModelExporter* parent,
                                  const QString& remoteTypeName, const QString& remoteMimeType );

    virtual ~AbstractModelExporterPrivate();

  public:
    const QString& remoteTypeName() const;
    const QString& remoteMimeType() const;

  protected:
    AbstractModelExporter* const q_ptr;

    const QString mRemoteTypeName;
    const QString mRemoteMimeType;
};


inline AbstractModelExporterPrivate::AbstractModelExporterPrivate( AbstractModelExporter* parent,
    const QString& remoteTypeName, const QString& remoteMimeType )
  : q_ptr( parent ),
    mRemoteTypeName( remoteTypeName ),
    mRemoteMimeType( remoteMimeType )
{}

inline AbstractModelExporterPrivate::~AbstractModelExporterPrivate()
{}

inline const QString& AbstractModelExporterPrivate::remoteTypeName() const { return mRemoteTypeName; }
inline const QString& AbstractModelExporterPrivate::remoteMimeType() const { return mRemoteMimeType; }

}

#endif
