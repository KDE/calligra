/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2010 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef ABSTRACTTOOLVIEWFACTORY_H
#define ABSTRACTTOOLVIEWFACTORY_H

#include "kastengui.h"

namespace Kasten2
{
class AbstractToolView;
class AbstractTool;


class AbstractToolViewFactory
{
  public:
    virtual ~AbstractToolViewFactory();

  public: // API to be implemented
    virtual AbstractToolView* create( AbstractTool* tool ) = 0;

    // TODO: find out of iconName an id can be QByteArray
    virtual QString iconName() const = 0;
    virtual QString title() const = 0;
    virtual QString id() const = 0;
    virtual SidePosition defaultPosition() const = 0;
};


inline AbstractToolViewFactory::~AbstractToolViewFactory() {}

}

#endif
