/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2010-2011 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef TERMINALTOOL_H
#define TERMINALTOOL_H

// lib
#include "kastencontrollers_export.h"
// Kasten core
#include <abstracttool.h>

class KUrl;


namespace Kasten2
{

class DocumentSyncManager;
class AbstractDocument;


class KASTENCONTROLLERS_EXPORT TerminalTool : public AbstractTool
{
  Q_OBJECT

  public:
    explicit TerminalTool( DocumentSyncManager* documentSyncManager );
    virtual ~TerminalTool();

  public:
    KUrl currentUrl() const;

  public: // AbstractTool API
//     virtual AbstractModel* targetModel() const;
    virtual QString title() const;

    virtual void setTargetModel( AbstractModel* model );

  Q_SIGNALS:
    void currentUrlChanged( const KUrl& currentUrl );

  private: // sources
    DocumentSyncManager* mDocumentSyncManager;

    AbstractDocument* mDocument;
};

}

#endif
