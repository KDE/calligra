/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef ABSTRACTMODELDATAGENERATOR_P_H
#define ABSTRACTMODELDATAGENERATOR_P_H

#include "abstractmodeldatagenerator.h"


namespace Kasten2
{

class AbstractModelDataGeneratorPrivate
{
  public:
    explicit AbstractModelDataGeneratorPrivate( AbstractModelDataGenerator* parent,
                                                const QString& typeName, const QString& mimeType,
                                                AbstractModelDataGenerator::Flags flags );

    virtual ~AbstractModelDataGeneratorPrivate();

  public:
    const QString& typeName() const;
    const QString& mimeType() const;
    AbstractModelDataGenerator::Flags flags() const;

  protected:
    AbstractModelDataGenerator* const q_ptr;

    const QString mTypeName;
    const QString mMimeType;
    AbstractModelDataGenerator::Flags mFlags;
};


inline AbstractModelDataGeneratorPrivate::AbstractModelDataGeneratorPrivate( AbstractModelDataGenerator* parent,
    const QString& typeName, const QString& mimeType, AbstractModelDataGenerator::Flags flags )
  : q_ptr( parent ),
    mTypeName( typeName ),
    mMimeType( mimeType ),
    mFlags( flags )
{}

inline AbstractModelDataGeneratorPrivate::~AbstractModelDataGeneratorPrivate()
{}

inline const QString& AbstractModelDataGeneratorPrivate::typeName() const { return mTypeName; }
inline const QString& AbstractModelDataGeneratorPrivate::mimeType() const { return mMimeType; }
inline AbstractModelDataGenerator::Flags AbstractModelDataGeneratorPrivate::flags() const { return mFlags; }

}

#endif
