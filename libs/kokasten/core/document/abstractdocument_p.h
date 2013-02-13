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

#ifndef ABSTRACTDOCUMENT_P_H
#define ABSTRACTDOCUMENT_P_H

#include "abstractmodel_p.h"
#include "abstractdocument.h"

// lib
#include "abstractmodelsynchronizer.h"


namespace Kasten2
{

class AbstractDocumentPrivate : public AbstractModelPrivate
{
  public:
      explicit AbstractDocumentPrivate( AbstractDocument* parent );

      ~AbstractDocumentPrivate();

  public:
    const QString& id() const;
    AbstractModelSynchronizer* synchronizer() const;

  public:
    void setId( const QString& id );
    void setSynchronizer( AbstractModelSynchronizer* synchronizer );

  protected:
    Q_DECLARE_PUBLIC( AbstractDocument )

  protected:
    QString mId;
    AbstractModelSynchronizer* mSynchronizer; // TODO: should this be here, with public setters and getters?
};


inline AbstractDocumentPrivate::AbstractDocumentPrivate( AbstractDocument* parent )
  : AbstractModelPrivate( parent ),
    mSynchronizer( 0 )
{}

inline const QString& AbstractDocumentPrivate::id() const { return mId; }
inline void AbstractDocumentPrivate::setId( const QString& id ) { mId = id; }

inline AbstractModelSynchronizer* AbstractDocumentPrivate::synchronizer() const { return mSynchronizer; }
inline void AbstractDocumentPrivate::setSynchronizer( AbstractModelSynchronizer* synchronizer )
{
    Q_Q( AbstractDocument );

    // plugging the same more than once?
    if( mSynchronizer == synchronizer )
        return;

    delete mSynchronizer;
    mSynchronizer = synchronizer;

    emit q->synchronizerChanged( synchronizer );
}

inline AbstractDocumentPrivate::~AbstractDocumentPrivate()
{
    delete mSynchronizer;
}

}

#endif
