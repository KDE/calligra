/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2007-2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef MODELCODECMANAGER_H
#define MODELCODECMANAGER_H

// lib
#include "kastencore_export.h"
// Qt
#include <QtCore/QObject>
#include <QtCore/QList>

class QString;


namespace Kasten2
{

class AbstractModel;
class AbstractModelSelection;
class AbstractModelStreamEncoder;
class AbstractModelStreamDecoder;
class AbstractModelDataGenerator;
class AbstractModelExporter;
class DocumentManager;
class AbstractOverwriteDialog;


class KASTENCORE_EXPORT ModelCodecManager : public QObject
{
  Q_OBJECT

  public:
    explicit ModelCodecManager( DocumentManager* manager );
    virtual ~ModelCodecManager();

  public:
    // or use the viewmodel here? on what should the export be based?
    void encodeToStream( AbstractModelStreamEncoder* encoder,
                         AbstractModel* model, const AbstractModelSelection* selection );

    void exportDocument( AbstractModelExporter* exporter,
                         AbstractModel* model, const AbstractModelSelection* selection );

  public:
    QList<AbstractModelStreamEncoder*> encoderList( AbstractModel* model, const AbstractModelSelection* selection ) const;
    QList<AbstractModelStreamDecoder*> decoderList() const;
    QList<AbstractModelDataGenerator*> generatorList() const;

    QList<AbstractModelExporter*> exporterList( AbstractModel* model, const AbstractModelSelection* selection ) const;

  public:
    void setEncoders( const QList<AbstractModelStreamEncoder*>& encoderList );
    void setDecoders( const QList<AbstractModelStreamDecoder*>& decoderList );
    void setGenerators( const QList<AbstractModelDataGenerator*>& generatorList );
    void setOverwriteDialog( AbstractOverwriteDialog* overwriteDialog );

  private:
    // unless there is a singleton
    DocumentManager* mManager;
    // used for dialogs, TODO: create (or use?) global instance for this
    AbstractOverwriteDialog* mOverwriteDialog;

    // temporary hack: hard coded codecs for byte arrays
    QList<AbstractModelStreamEncoder*> mEncoderList;
    QList<AbstractModelStreamDecoder*> mDecoderList;
    QList<AbstractModelDataGenerator*> mGeneratorList;
    QList<AbstractModelExporter*> mExporterList;
};

}

#endif
