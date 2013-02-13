/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2008 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef ABSTRACTMODEL_H
#define ABSTRACTMODEL_H

// lib
#include "kastencore_export.h"
// Qt
#include <QtCore/QObject>

class QString;


namespace Kasten2
{
class AbstractModelPrivate;


// TODO: reasons not to name it AbstractObjectModel, but just as it is
class KASTENCORE_EXPORT AbstractModel : public QObject
{
    Q_OBJECT

  protected:
    explicit AbstractModel( AbstractModel* baseModel = 0 );
    explicit AbstractModel( AbstractModelPrivate* d );

  public:
    virtual ~AbstractModel();

  public:
// TODO: just one baseModel, or can there be multiple? Better name?
    AbstractModel* baseModel() const;
    /**
     * returns the first baseModel which is of type T, or null if none is found.
     * The search is started with the model itself
     */
    template <typename T>
    T findBaseModel() const;
    /**
     * returns the first baseModel which is of type T, or null if none is found.
     * The search is started with the model itself
     */
    template <typename T>
    AbstractModel* findBaseModelWithInterface() const;

  public: // API to be implemented
    virtual QString title() const = 0;

    /** Default returns false */
    virtual bool isModifiable() const;
    /** default returns true */
    virtual bool isReadOnly() const;
    /** default does nothing */
    virtual void setReadOnly( bool isReadOnly );

  Q_SIGNALS:
    // TODO: readonly and modifiable should be turned into flags, also get/set methods
    void readOnlyChanged( bool isReadOnly );
    void modifiableChanged( bool isModifiable );
    void titleChanged( const QString& newTitle );

  protected:
    void setBaseModel( AbstractModel* baseModel );

  protected:
    AbstractModelPrivate* const d_ptr;
    Q_DECLARE_PRIVATE( AbstractModel )
};


template <typename T>
T AbstractModel::findBaseModel() const
{
    AbstractModel* model = const_cast<AbstractModel*>( this );
    do
    {
        T castedModel = qobject_cast<T>(model);
        if( castedModel )
            return castedModel;
        model = model->baseModel();
    }
    while( model );

    return 0;
}

template <typename T>
AbstractModel* AbstractModel::findBaseModelWithInterface() const
{
    AbstractModel* model = const_cast<AbstractModel*>( this );
    do
    {
        T interface = qobject_cast<T>(model);
        if( interface )
            return model;
        model = model->baseModel();
    }
    while( model );

    return 0;
}

}

#endif
