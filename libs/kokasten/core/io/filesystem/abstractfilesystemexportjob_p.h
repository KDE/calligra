/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2008-2009,2011 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef ABSTRACTFILESYSTEMEXPORTJOB_P_H
#define ABSTRACTFILESYSTEMEXPORTJOB_P_H

// lib
#include "abstractfilesystemexportjob.h"
#include <abstractexportjob_p.h>
// KDE
#include <KUrl>


namespace Kasten2
{

class AbstractFileSystemExportJobPrivate : public AbstractExportJobPrivate
{
  public:
    AbstractFileSystemExportJobPrivate( AbstractFileSystemExportJob* parent,
                                        AbstractModel* model, const AbstractModelSelection* selection, const KUrl& url );

    virtual ~AbstractFileSystemExportJobPrivate();

  public: // KJob API
    virtual void start();

  public:
    void completeExport( bool success );

  public:
    AbstractModel* model() const;
    const AbstractModelSelection* selection() const;
    QFile* file() const;

  public: // slot
    void exportToFile();

  protected:
    Q_DECLARE_PUBLIC( AbstractFileSystemExportJob )

  protected:
    AbstractModel* const mModel;
    const AbstractModelSelection* const mSelection;
    const KUrl mUrl;
    QFile* mFile;
    QString mWorkFilePath;
};


inline AbstractFileSystemExportJobPrivate::AbstractFileSystemExportJobPrivate( AbstractFileSystemExportJob* parent,
                                        AbstractModel* model, const AbstractModelSelection* selection,
                                        const KUrl& url)
  : AbstractExportJobPrivate( parent ),
    mModel( model ),
    mSelection( selection ),
    mUrl( url ),
    mFile( 0 )
{}

inline AbstractFileSystemExportJobPrivate::~AbstractFileSystemExportJobPrivate() {}

inline AbstractModel* AbstractFileSystemExportJobPrivate::model()                    const { return mModel; }
inline const AbstractModelSelection* AbstractFileSystemExportJobPrivate::selection() const { return mSelection; }
inline QFile* AbstractFileSystemExportJobPrivate::file()                             const { return mFile; }


inline void AbstractFileSystemExportJobPrivate::start()
{
    Q_Q( AbstractFileSystemExportJob );

    QMetaObject::invokeMethod( q, "exportToFile", Qt::QueuedConnection );
}

}

#endif
