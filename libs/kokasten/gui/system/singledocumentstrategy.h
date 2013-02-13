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

#ifndef SINGLEDOCUMENTSTRATEGY_H
#define SINGLEDOCUMENTSTRATEGY_H

// lib
#include "abstractdocumentstrategy.h"


namespace Kasten2
{
class ViewManager;
class DocumentManager;
class SingleDocumentStrategyPrivate;


class KASTENGUI_EXPORT SingleDocumentStrategy : public AbstractDocumentStrategy
{
  Q_OBJECT

  public:
    SingleDocumentStrategy( DocumentManager* documentManager,
                            ViewManager* viewManager );
    virtual ~SingleDocumentStrategy();

  public: // AbstractDocumentStrategy API
    virtual void createNew();
    virtual void createNewFromClipboard();
    virtual void createNewWithGenerator( AbstractModelDataGenerator* generator );

    virtual void load( const KUrl& url );

    virtual void closeDocument( AbstractDocument* document );
    virtual void closeAll();
    virtual void closeAllOther( AbstractDocument* document );

  public: // const AbstractDocumentStrategy API
    virtual QList<AbstractDocument*> documents() const;
    virtual QStringList supportedRemoteTypes() const;

    virtual bool canClose( AbstractDocument* document ) const;
    virtual bool canCloseAll() const;
    virtual bool canCloseAllOther( AbstractDocument* document ) const;

  protected:
    Q_DECLARE_PRIVATE( SingleDocumentStrategy )
};

}

#endif
