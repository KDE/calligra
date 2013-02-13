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

#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

// lib
#include "kastencore_export.h"
// Qt
#include <QtCore/QList>
#include <QtCore/QObject>

class QStringList;


namespace Kasten2
{
class AbstractDocument;

class DocumentCreateManager; //TODO: temporary
class DocumentSyncManager; //TODO: temporary
class ModelCodecManager; //TODO: temporary


class KASTENCORE_EXPORT DocumentManager : public QObject
{
  Q_OBJECT

  friend class DocumentSyncManager;

  public:
    DocumentManager();
    virtual ~DocumentManager();

  public:
    void addDocument( AbstractDocument* document );

    void closeDocument( AbstractDocument* document );
    void closeDocuments( const QList<AbstractDocument*>& documents );
    void closeAll();
// TODO: think about if a more general close( documentList, theseOrOthers ) is better, same with canCloseAllOther()
    void closeAllOther( AbstractDocument* document );

    // TODO: what to do for documents not added?
    bool canClose( AbstractDocument* document );
    bool canClose( const QList<AbstractDocument*>& documents );
    bool canCloseAll();
    bool canCloseAllOther( AbstractDocument* document );

    void requestFocus( AbstractDocument* document );

  public:
    QList<AbstractDocument*> documents() const;
    bool isEmpty() const;

  public:
    DocumentCreateManager* createManager();
    DocumentSyncManager* syncManager();
    ModelCodecManager* codecManager();

  Q_SIGNALS:
    // documents got added
    void added( const QList<Kasten2::AbstractDocument*>& documents );
    /// documents are about to be closed, cannot be stopped
    void closing( const QList<Kasten2::AbstractDocument*>& documents );

//     void closing( KCloseEvent* event );
// TODO: other than QObject event gets modified by observers, take care of unsetting a close cancel
// problem with a signal is that all(!) observers get notified, even if event is already cancelled
// better a visitor pattern?

    // TODO: or should the document be able to emit this?
    void focusRequested( Kasten2::AbstractDocument* document );

//  protected:
//    virtual AbstractDocument* createDocument();

  private:
    QList<AbstractDocument*> mList;

    // TODO: remove into own singleton
    DocumentCreateManager* mCreateManager;
    DocumentSyncManager* mSyncManager;
    ModelCodecManager* mCodecManager;
};


inline DocumentCreateManager* DocumentManager::createManager() { return mCreateManager; }
inline DocumentSyncManager* DocumentManager::syncManager() { return mSyncManager; }
inline ModelCodecManager* DocumentManager::codecManager() { return mCodecManager; }

}

#endif
