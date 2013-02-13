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

#ifndef MODELENCODERFILESYSTEMEXPORTJOB_P_H
#define MODELENCODERFILESYSTEMEXPORTJOB_P_H

// lib
#include "abstractfilesystemexportjob_p.h"
#include "modelencoderfilesystemexportjob.h"


namespace Kasten2
{

class ModelEncoderFileSystemExportJobPrivate : public AbstractFileSystemExportJobPrivate
{
  public:
    ModelEncoderFileSystemExportJobPrivate( ModelEncoderFileSystemExportJob* parent,
                                            AbstractModel* model,
                                            const AbstractModelSelection* selection,
                                            const KUrl& url,
                                            AbstractModelStreamEncoder* encoder );

    virtual ~ModelEncoderFileSystemExportJobPrivate();

  public: // AbstractFileSystemExportJob API
    void startExportToFile();

  public:
    AbstractModelStreamEncoder* encoder() const;

  protected:
    Q_DECLARE_PUBLIC( ModelEncoderFileSystemExportJob )

  protected:
    AbstractModelStreamEncoder* const mEncoder;
};


inline ModelEncoderFileSystemExportJobPrivate::ModelEncoderFileSystemExportJobPrivate( ModelEncoderFileSystemExportJob* parent,
    AbstractModel* model,
    const AbstractModelSelection* selection,
    const KUrl& url,
    AbstractModelStreamEncoder* encoder )
  : AbstractFileSystemExportJobPrivate( parent, model, selection, url ),
    mEncoder( encoder )
{}

inline ModelEncoderFileSystemExportJobPrivate::~ModelEncoderFileSystemExportJobPrivate() {}

inline AbstractModelStreamEncoder* ModelEncoderFileSystemExportJobPrivate::encoder() const { return mEncoder; }

}

#endif
