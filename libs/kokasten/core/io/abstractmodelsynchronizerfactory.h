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

#ifndef ABSTRACTMODELSYNCHRONIZERFACTORY_H
#define ABSTRACTMODELSYNCHRONIZERFACTORY_H

// lib
#include "abstractmodelsynchronizer.h"
// Qt
#include <QtCore/QObject>


namespace Kasten2
{

// not really a classic factory, functions do some more, instead it is what?
// TODO: improve the whole vodoo, especially linking document and synchronizer
// make it foolproof
// make a simple tutorial with an example program
// TODO: should the load/connect/export jobs be created here or from the synchronizer?
// putting jobs to synchronizer keeps factory simple, but forces synchronizer to be
// in invalid states, like with simple constructor
class KASTENCORE_EXPORT AbstractModelSynchronizerFactory : public QObject
{
  Q_OBJECT

  public:
    virtual ~AbstractModelSynchronizerFactory();

  public: // API to be implemented
    virtual AbstractModelSynchronizer* createSynchronizer() const = 0;

//     virtual AbstractDocument* loadNewDocument( const KUrl& originUrl ) const = 0;
//     virtual AbstractLoadJob* startLoad( const KUrl& url ) = 0;
    // TODO: better name than connect: bind?

//     virtual bool connectDocument( AbstractDocument* document, const KUrl& originUrl,
//                                   AbstractDocumentSynchronizer::ConnectOption option ) const = 0;
//     virtual AbstractConnectJob* startConnect( AbstractDocument* document,
//                                               const KUrl& url, AbstractDocumentSynchronizer::ConnectOption option ) = 0;

//     virtual bool exportDocument( AbstractDocument* document, const KUrl& originUrl ) const = 0;
//     virtual AbstractExportJob* startExport( AbstractDocument* document, const KUrl& originUrl ) const = 0;

    /** returns the id of the work model type */
    // TODO: is QByteArray enough?
    virtual QString supportedWorkType() const = 0;
    /** returns the id of the remote model type */
    virtual QString supportedRemoteType() const = 0;
};

}

#endif
