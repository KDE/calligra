/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2006-2007,2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef KASTEN_IF_DATASELECTABLE_H
#define KASTEN_IF_DATASELECTABLE_H

// Qt
#include <QtCore/QtPlugin>

class QMimeData;


namespace Kasten2
{

class AbstractModelSelection;


namespace If
{

// TODO: this interface is strongly related to the selecteddatawriteable interface
class DataSelectable
{
  public:
    virtual ~DataSelectable();

  public: // set
    virtual void selectAllData( bool selectAll ) = 0;

  public: // get
    virtual bool hasSelectedData() const = 0;
    virtual QMimeData *copySelectedData() const = 0; // TODO: move into AbstractModelSelection
    virtual const AbstractModelSelection* modelSelection() const = 0;

  public: // signal
    /// emitted if there is a change in whether selected data is available or not
    virtual void hasSelectedDataChanged( bool hasSelectedData ) = 0;
    /// emitted if the selection changes, to other data or none
    virtual void selectedDataChanged( const AbstractModelSelection* modelSelection ) = 0;
};

inline DataSelectable::~DataSelectable() {}

}
}

Q_DECLARE_INTERFACE( Kasten2::If::DataSelectable, "org.kde.kasten2.if.dataselectable/1.0" )

#endif
