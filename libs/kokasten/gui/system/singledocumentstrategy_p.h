/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2011 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef SINGLEDOCUMENTSTRATEGY_P_H
#define SINGLEDOCUMENTSTRATEGY_P_H

// lib
#include "abstractdocumentstrategy_p.h"
#include "singledocumentstrategy.h"
// Kasten core
#include <documentmanager.h>
#include <documentsyncmanager.h>
// Qt Core
#include <QtCore/QStringList>


namespace Kasten2
{
class ViewManager;


class SingleDocumentStrategyPrivate : public AbstractDocumentStrategyPrivate
{
  public:
    SingleDocumentStrategyPrivate( SingleDocumentStrategy* parent,
                                   DocumentManager* documentManager,
                                   ViewManager* viewManager );
    virtual ~SingleDocumentStrategyPrivate();

  public:
    void init();

  public: // AbstractDocumentStrategy API
    void createNew();
    void createNewFromClipboard();
    void createNewWithGenerator( AbstractModelDataGenerator* generator );

    void load( const KUrl& url );

    void closeDocument( AbstractDocument* document );
    void closeAll();
    void closeAllOther( AbstractDocument* document );

  public: // const AbstractDocumentStrategy API
    QList<AbstractDocument*> documents() const;
    QStringList supportedRemoteTypes() const;

    bool canClose( AbstractDocument* document ) const;
    bool canCloseAll() const;
    bool canCloseAllOther( AbstractDocument* document ) const;

  protected:
    Q_DECLARE_PUBLIC( SingleDocumentStrategy )

  protected:
    DocumentManager* mDocumentManager;
    ViewManager* mViewManager;
};


inline
SingleDocumentStrategyPrivate::SingleDocumentStrategyPrivate( SingleDocumentStrategy* parent,
                                                              DocumentManager* documentManager,
                                                              ViewManager* viewManager )
  : AbstractDocumentStrategyPrivate( parent ),
    mDocumentManager( documentManager ),
    mViewManager( viewManager )
{
}

inline QList<AbstractDocument*> SingleDocumentStrategyPrivate::documents() const
{
    return mDocumentManager->documents();
}

inline QStringList SingleDocumentStrategyPrivate::supportedRemoteTypes() const
{
    return mDocumentManager->syncManager()->supportedRemoteTypes();
}

inline bool SingleDocumentStrategyPrivate::canClose( AbstractDocument* document ) const
{
    return mDocumentManager->canClose( document );
}

inline bool SingleDocumentStrategyPrivate::canCloseAllOther( AbstractDocument* document ) const
{
    return mDocumentManager->canCloseAllOther( document );
}

inline bool SingleDocumentStrategyPrivate::canCloseAll() const
{
    return mDocumentManager->canCloseAll();
}

inline void SingleDocumentStrategyPrivate::closeAll()
{
    mDocumentManager->closeAll();
}

inline void SingleDocumentStrategyPrivate::closeAllOther( AbstractDocument* document )
{
    mDocumentManager->closeAllOther( document );
}

inline void SingleDocumentStrategyPrivate::closeDocument( AbstractDocument* document )
{
    mDocumentManager->closeDocument( document );
}

inline SingleDocumentStrategyPrivate::~SingleDocumentStrategyPrivate() {}

}

#endif
