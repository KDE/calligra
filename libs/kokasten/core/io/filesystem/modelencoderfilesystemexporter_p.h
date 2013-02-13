/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2008-2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef MODELENCODERFILESYSTEMEXPORTER_P_H
#define MODELENCODERFILESYSTEMEXPORTER_P_H

// lib
#include "modelencoderfilesystemexporter.h"
#include "modelencoderfilesystemexportjob.h"
#include <abstractmodelstreamencoder.h>
#include <abstractmodelexporter_p.h>


namespace Kasten2
{

class ModelEncoderFileSystemExporterPrivate : public AbstractModelExporterPrivate
{
  public:
    ModelEncoderFileSystemExporterPrivate( ModelEncoderFileSystemExporter* parent,
                                           const QString& remoteTypeName, const QString& remoteMimeType,
                                           AbstractModelStreamEncoder* encoder );

    virtual ~ModelEncoderFileSystemExporterPrivate();

  public: // AbstractModelExporter API
    AbstractExportJob* startExport( AbstractModel* model, const AbstractModelSelection* selection,
                                    const KUrl& url );
    QString modelTypeName( AbstractModel* model, const AbstractModelSelection* selection ) const;

  public:
    AbstractModelStreamEncoder* encoder() const;

  protected:
    AbstractModelStreamEncoder* const mEncoder;
};


inline ModelEncoderFileSystemExporterPrivate::ModelEncoderFileSystemExporterPrivate( ModelEncoderFileSystemExporter* parent,
    const QString& remoteTypeName, const QString& remoteMimeType,
    AbstractModelStreamEncoder* encoder )
  : AbstractModelExporterPrivate( parent, remoteTypeName, remoteMimeType ),
    mEncoder( encoder )
{
}

inline ModelEncoderFileSystemExporterPrivate::~ModelEncoderFileSystemExporterPrivate() {}

inline AbstractModelStreamEncoder* ModelEncoderFileSystemExporterPrivate::encoder() const { return mEncoder; }

inline AbstractExportJob* ModelEncoderFileSystemExporterPrivate::startExport( AbstractModel* model,
    const AbstractModelSelection* selection,
    const KUrl& url )
{
    return new ModelEncoderFileSystemExportJob( model, selection, url, mEncoder );
}

inline QString ModelEncoderFileSystemExporterPrivate::modelTypeName( AbstractModel* model, const AbstractModelSelection* selection ) const
{
    return mEncoder->modelTypeName( model, selection );
}

}

#endif
