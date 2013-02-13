/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2006,2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef ABSTRACTVIEWFACTORY_H
#define ABSTRACTVIEWFACTORY_H

// lib
#include "abstractview.h"


namespace Kasten2
{

class KASTENGUI_EXPORT AbstractViewFactory
{
  public:
    virtual ~AbstractViewFactory();

  public:
    // TODO: there can be views not only on documents
    virtual AbstractView* createViewFor( AbstractDocument* document ) = 0;
    // TODO: is alignment best done here? needs view to be stable on creation of view copy
    // doesn't work if the new view is not next to the old, but are there usecases for this?
    /**
    * @param alignment on which side the new view is placed to show a continuous whole view
    */
    virtual AbstractView* createCopyOfView( AbstractView* view, Qt::Alignment alignment = 0 );
};


// TODO: is this default implementation useful? Like, if the base is not a document, but a subdocument/model?
inline AbstractView* AbstractViewFactory::createCopyOfView( AbstractView* view, Qt::Alignment alignment )
{
    Q_UNUSED( alignment )

    AbstractView* viewCopy = createViewFor( view->findBaseModel<AbstractDocument*>() );
    if( viewCopy )
    {
        viewCopy->setReadOnly( view->isReadOnly() );
    }

    return viewCopy;
}

inline AbstractViewFactory::~AbstractViewFactory() {}

}

#endif
