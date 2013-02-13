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

#include "modelcodecmanager.h"

// lib
#include "modelencoderfilesystemexporter.h"
#include "abstractmodelstreamencoder.h"
// #include "abstractmodelstreamdecoder.h"
#include "abstractmodeldatagenerator.h"
#include "abstractoverwritedialog.h"
#include "jobmanager.h"
#include "documentmanager.h"
#include "abstractexportjob.h"
// KDE
#include <KIO/NetAccess>
#include <KFileDialog>
#include <KPushButton>
#include <KLocale>


namespace Kasten2
{

ModelCodecManager::ModelCodecManager( DocumentManager* manager )
  : mManager( manager ),
    mOverwriteDialog( 0 )
{
}

QList<AbstractModelStreamEncoder*>
ModelCodecManager::encoderList( AbstractModel* model, const AbstractModelSelection* selection ) const
{
Q_UNUSED( selection )
    return model ? mEncoderList : QList<AbstractModelStreamEncoder*>();
}

QList<AbstractModelStreamDecoder*>
ModelCodecManager::decoderList() const { return mDecoderList; }

QList<AbstractModelDataGenerator*>
ModelCodecManager::generatorList() const { return mGeneratorList; }

QList<AbstractModelExporter*>
ModelCodecManager::exporterList( AbstractModel* model, const AbstractModelSelection* selection ) const
{
Q_UNUSED( selection )
    return model ? mExporterList : QList<AbstractModelExporter*>();
}

void ModelCodecManager::setOverwriteDialog( AbstractOverwriteDialog* overwriteDialog )
{
    mOverwriteDialog = overwriteDialog;
}

void ModelCodecManager::setEncoders( const QList<AbstractModelStreamEncoder*>& encoderList )
{
    mEncoderList = encoderList;

    qDeleteAll( mExporterList );
    mExporterList.clear();

    foreach( AbstractModelStreamEncoder* encoder, mEncoderList )
        mExporterList << new ModelEncoderFileSystemExporter( encoder );
}

void ModelCodecManager::setDecoders( const QList<AbstractModelStreamDecoder*>& decoderList )
{
    mDecoderList = decoderList;
}

void ModelCodecManager::setGenerators( const QList<AbstractModelDataGenerator*>& generatorList )
{
    mGeneratorList = generatorList;
}

void ModelCodecManager::encodeToStream( AbstractModelStreamEncoder* encoder,
                                        AbstractModel* model, const AbstractModelSelection* selection )
{
   Q_UNUSED( selection )
   Q_UNUSED( model )
   Q_UNUSED( encoder )
//    AbstractDocument* model = mFactory->create();
//    mManager->addDocument( model );
}


void ModelCodecManager::exportDocument( AbstractModelExporter* exporter,
                                        AbstractModel* model, const AbstractModelSelection* selection )
{
    bool exportDone = false;

    const QString dialogTitle =
        i18nc( "@title:window", "Export" );
    do
    {
        KFileDialog exportFileDialog( /*mWorkingUrl.url()*/KUrl(), QString(), /*mWidget*/0 );

        exportFileDialog.setOperationMode( KFileDialog::Saving );
        exportFileDialog.setMode( KFile::File );
        const QStringList mimeTypes = QStringList() << exporter->remoteMimeType();
        exportFileDialog.setMimeFilter( mimeTypes );
        exportFileDialog.setCaption( dialogTitle );
        const KGuiItem exportGuiItem( i18nc("@action:button",
                                            "&Export"),
                                      QLatin1String("document-export"),
                                      i18nc("@info:tooltip",
                                            "Export the data into the file with the entered name.") );
        exportFileDialog.okButton()->setGuiItem( exportGuiItem );

        exportFileDialog.exec();

        const KUrl exportUrl = exportFileDialog.selectedUrl();

        if( !exportUrl.isEmpty() )
        {
            const bool isUrlInUse = KIO::NetAccess::exists( exportUrl, KIO::NetAccess::DestinationSide, /*mWidget*/0 );

            if( isUrlInUse )
            {
                // TODO: care for case that file from url is already loaded by (only?) this program
//                     const bool otherFileLoaded = mManager->documentByUrl( exportUrl );
                // TODO: replace "file" with synchronizer->storageTypeName() or such
                // TODO: offer "Synchronize" as alternative, if supported, see below
                const Answer answer =
                    mOverwriteDialog ? mOverwriteDialog->queryOverwrite( exportUrl, dialogTitle ) : Cancel;
                if( answer == Cancel )
                    break;
                if( answer == PreviousQuestion )
                    continue;
            }

            AbstractExportJob* exportJob = exporter->startExport( model, selection, exportUrl );
            exportDone = JobManager::executeJob( exportJob );

//                 if( exportDone )
//                     emit urlUsed( exportUrl );
        }
        else
            break;
    }
    while( !exportDone );
}

ModelCodecManager::~ModelCodecManager()
{
    qDeleteAll( mExporterList );
    qDeleteAll( mEncoderList );
//     qDeleteAll( mDecoderList );
    qDeleteAll( mGeneratorList );
}

}
