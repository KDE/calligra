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

#ifndef KASTEN_IF_VIEWAREASPLITABLE_H
#define KASTEN_IF_VIEWAREASPLITABLE_H

// Qt
#include <QtCore/QtPlugin>
#include <QtCore/QList>


namespace Kasten2
{

class AbstractViewArea;


namespace If
{

// TODO: should split(Qt::Orientation) be a method of AbstractViewArea?
// TODO: split or add? difference in semantics?
// TODO: areas could be tabbed/stacked, too, also recursively. Support that?
// TODO: should views to part/sub-models be restricted to overview of document? No, or?
// TODO: where to decide what to do with the contained views of a area that is closed?
// TODO: where to decide what to do on a split, e.g. add a new view copy of the current one?
class ViewAreaSplitable
{
  public:
    virtual ~ViewAreaSplitable();

  public: // set/action
    /// returns the new view area
    virtual AbstractViewArea* splitViewArea( AbstractViewArea* viewArea, Qt::Orientation orientation ) = 0;
    virtual void closeViewArea( AbstractViewArea* viewArea ) = 0; // TODO: or report success with bool?
    virtual void setViewAreaFocus( AbstractViewArea* viewArea ) = 0;

  public: // get
    virtual AbstractViewArea* viewAreaFocus() const = 0;
//     virtual QList<Kasten2::AbstractViewArea*> viewAreas() const = 0;
    virtual int viewAreasCount() const = 0;

  public: // signal
    virtual void viewAreasAdded( const QList<Kasten2::AbstractViewArea*>& viewAreas ) = 0;
    virtual void viewAreasRemoved( const QList<Kasten2::AbstractViewArea*>& viewAreas ) = 0;
    virtual void viewAreaFocusChanged( Kasten2::AbstractViewArea* viewArea ) = 0;
};


inline ViewAreaSplitable::~ViewAreaSplitable() {}

}
}

Q_DECLARE_INTERFACE( Kasten2::If::ViewAreaSplitable, "org.kde.kasten2.if.viewareasplitable/1.0" )

#endif
