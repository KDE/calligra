/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2006-2007,2009,2011 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef DOCUMENTCREATEMANAGER_H
#define DOCUMENTCREATEMANAGER_H

// lib
#include "kastencore_export.h"
// Qt
#include <QtCore/QObject>

class QMimeData;


namespace Kasten2
{

class AbstractDocumentFactory;
class DocumentManager;


class KASTENCORE_EXPORT DocumentCreateManager : public QObject
{
  Q_OBJECT

  public:
    explicit DocumentCreateManager( DocumentManager* manager );
    virtual ~DocumentCreateManager();

  public:
    bool canCreateNewFromData( const QMimeData* mimeData ) const;

  public:
    void createNew();
    void createNewFromData( const QMimeData* mimeData, bool setModified );

  public:
    void setDocumentFactory( AbstractDocumentFactory* factory );

  private:
    // unless there is a singleton
    DocumentManager* mManager;

    // temporary hack: hard coded factory for byte arrays
    AbstractDocumentFactory* mFactory;
};

}

#endif
