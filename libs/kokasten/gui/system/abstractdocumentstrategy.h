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

#ifndef ABSTRACTDOCUMENTSTRATEGY_H
#define ABSTRACTDOCUMENTSTRATEGY_H

// lib
#include "kastengui_export.h"
// Qt
#include <QtCore/QObject>

template<typename C> class QList;
class KUrl;


namespace Kasten2
{
class AbstractDocument;
class AbstractModelDataGenerator;
class AbstractDocumentStrategyPrivate;


class KASTENGUI_EXPORT AbstractDocumentStrategy : public QObject
{
  Q_OBJECT

  protected:
    AbstractDocumentStrategy();
    explicit AbstractDocumentStrategy( AbstractDocumentStrategyPrivate* d );
  public:
    virtual ~AbstractDocumentStrategy();

  public: // API to be implemented
    virtual void createNew() = 0;
    virtual void createNewFromClipboard() = 0;
    virtual void createNewWithGenerator( AbstractModelDataGenerator* generator ) = 0;

    virtual void load( const KUrl& url ) = 0;

    virtual void closeDocument( AbstractDocument* document ) = 0;
//     virtual void closeDocuments( const QList<AbstractDocument*>& documents ) = 0;
    virtual void closeAll() = 0;
// TODO: think about if a more general close( documentList, theseOrOthers ) is better, same with canCloseAllOther()
    virtual void closeAllOther( AbstractDocument* document ) = 0;

//     virtual void requestFocus( AbstractDocument* document ) = 0;

  public: // const API to be implemented
    /// Returns \c true if there 
//     virtual bool allowsMultipleDocuments() const = 0;
    virtual QList<AbstractDocument*> documents() const = 0;
    virtual QStringList supportedRemoteTypes() const = 0;

    // TODO: what to do for documents not added?
    virtual bool canClose( AbstractDocument* document ) const = 0;
//     virtual bool canClose( const QList<AbstractDocument*>& documents ) const = 0;
    virtual bool canCloseAll() const = 0;
    virtual bool canCloseAllOther( AbstractDocument* document ) const = 0;

  Q_SIGNALS:
    // documents got added
    void added( const QList<Kasten2::AbstractDocument*>& documents );
    /// documents are about to be closed, cannot be stopped
    void closing( const QList<Kasten2::AbstractDocument*>& documents );

    // TODO: or should the document be able to emit this?
    void focusRequested( Kasten2::AbstractDocument* document );

    void urlUsed( const KUrl& url );

  protected:
    AbstractDocumentStrategyPrivate* const d_ptr;
    Q_DECLARE_PRIVATE( AbstractDocumentStrategy )
};

}

#endif
