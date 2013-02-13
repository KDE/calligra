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

#include "abstractfilesystemexportjob.h"
#include "abstractfilesystemexportjob_p.h"
#include "abstractfilesystemexportjob.moc"


namespace Kasten2
{

AbstractFileSystemExportJob::AbstractFileSystemExportJob( AbstractModel* model, const AbstractModelSelection* selection,
                                 const KUrl& url )
  : AbstractExportJob( new AbstractFileSystemExportJobPrivate(this,model,selection,url) )
{
}

AbstractFileSystemExportJob::AbstractFileSystemExportJob( AbstractFileSystemExportJobPrivate* d )
  : AbstractExportJob( d )
{
}

AbstractModel* AbstractFileSystemExportJob::model() const
{
    Q_D( const AbstractFileSystemExportJob );

    return d->model();
}
const AbstractModelSelection* AbstractFileSystemExportJob::selection() const
{
    Q_D( const AbstractFileSystemExportJob );

    return d->selection();
}
QFile* AbstractFileSystemExportJob::file() const
{
    Q_D( const AbstractFileSystemExportJob );

    return d->file();
}

void AbstractFileSystemExportJob::start()
{
    Q_D( AbstractFileSystemExportJob );

    d->start();
}

void AbstractFileSystemExportJob::completeExport( bool success )
{
    Q_D( AbstractFileSystemExportJob );

    d->completeExport( success );
}


AbstractFileSystemExportJob::~AbstractFileSystemExportJob()
{
}

}
