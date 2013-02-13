/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2008 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef KDE_IF_VERSIONABLE_H
#define KDE_IF_VERSIONABLE_H

// lib
// #include "documentversionid.h"
#include "documentversiondata.h"
// Qt
#include <QtCore/QtPlugin>

/*
This interface for now just deals with the working copy of the model,
versions in the storing (like revision control systems) are not supported/integrated,
needs a more general approach to input, synchronizer, changesets and such.

A model/document can be versioned if the changes to it are traced and reversable.

Addressing:
While we don't do a distributed model and concurrent changes we use simple 
increasing indizes to address the different version, starting with 0.
The initial version as created/loaded in memory gets the index 0. All following
versions get a indexOfPrevious+1. This approach might be reusable for local
addressing later.

Wandering in the version "tree":
The model can be reset to a previous version, or be set
again to a newer version it had been advanced to before.
If the model was reset to a previous version and is changed, currently no branch is
created but all the versions that are newer are simply skipped.

Version
^ Changes
Version
^ Changes
Version
^ Changed
Version
^ Changes
InitialVersion : Index = 0
*/

namespace Kasten2
{
namespace If
{

class Versionable
{
  public:
    virtual ~Versionable();

  public: // get
//     virtual KDocumentVersionId versionId() const = 0;
    virtual int versionIndex() const = 0;
    virtual DocumentVersionData versionData( int versionIndex ) const = 0;
//     virtual KDocumentVersionIterator currentVersion() const = 0;
//     virtual KDocumentVersionIterator rootVersion() const = 0;
//     virtual KDocumentVersionIterator headVersion() const = 0;
// for those with multiple branches:
//     virtual KDocumentVersionIterator headVersion( const KDocumentVersionBranchId& id) const = 0;
//     virtual QList<KDocumentVersionBranchId> heads() const = 0;
    virtual int versionCount() const = 0;

  public: // set/action
//     virtual void setVersion( KDocumentVersionId id ) = 0;
    virtual void revertToVersionByIndex( int versionIndex ) = 0;

  public: // signal
//     virtual void versionChanged( KDocumentVersionId id ) = 0;
    // 
    virtual void revertedToVersionIndex( int versionIndex ) = 0;
    //
    virtual void headVersionDataChanged( const DocumentVersionData& versionData ) = 0;
    virtual void headVersionChanged( int newHeadVersionIndex ) = 0;
};

inline Versionable::~Versionable() {}

}
}

Q_DECLARE_INTERFACE( Kasten2::If::Versionable, "org.kde.kasten2.if.versionable/1.0" )

#endif
