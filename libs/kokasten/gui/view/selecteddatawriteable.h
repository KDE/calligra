/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2006-2007 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef KASTEN_IF_SELECTEDDATAWRITEABLE_H
#define KASTEN_IF_SELECTEDDATAWRITEABLE_H

// Qt
#include <QtCore/QtPlugin>

class QMimeData;


namespace Kasten2
{

namespace If
{
// TODO: this interface is strongly related to the selection interface
// needs further thinking
// for now the mimedata is the write extension to the selection interface
// as "createCopyOfSelectedData()" got moved to selection
// rename both interfaces to Selection and SelectionWriteable?
// TODO: cut does not work on overwrite modus (for me), how to generalize this?
class SelectedDataWriteable
{
  public:
    virtual ~SelectedDataWriteable();

  public: // operate
    // TODO: inserting works on selection if available
    virtual void insertData( const QMimeData *data ) = 0;
    virtual QMimeData *cutSelectedData() = 0;
    virtual void deleteSelectedData() = 0;

  public: // 
    virtual bool canReadData( const QMimeData *data ) const = 0;
};

inline SelectedDataWriteable::~SelectedDataWriteable() {}

}
}

Q_DECLARE_INTERFACE( Kasten2::If::SelectedDataWriteable, "org.kde.kasten2.if.selecteddatawriteable/1.0" )

#endif
