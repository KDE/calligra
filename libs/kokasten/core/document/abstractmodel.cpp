/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2007-2008 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "abstractmodel.h"
#include "abstractmodel_p.h"


namespace Kasten2
{

AbstractModel::AbstractModel( AbstractModel* baseModel )
  : d_ptr( new AbstractModelPrivate(this,baseModel) )
{}

AbstractModel::AbstractModel( AbstractModelPrivate* d )
  : d_ptr( d )
{}

bool AbstractModel::isModifiable()        const { return false; }
bool AbstractModel::isReadOnly()          const { return true; }
AbstractModel* AbstractModel::baseModel() const
{
    Q_D( const AbstractModel );

    return d->baseModel();
}

void AbstractModel::setBaseModel( AbstractModel* baseModel )
{
    Q_D( AbstractModel );

    d->setBaseModel( baseModel );
}
void AbstractModel::setReadOnly( bool isReadOnly ) { Q_UNUSED(isReadOnly) }

AbstractModel::~AbstractModel()
{
    delete d_ptr;
}

}
