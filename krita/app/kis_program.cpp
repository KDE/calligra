/*
    This file is part of the Krita program, made within the KDE community.

    Copyright 2006-2009,2011 Friedrich W. H. Kossebau <kossebau@kde.org>
    Copyright 2013 Boudewijn Rempt <boud@kde.org>
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy 
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "program.h"

// program
#include "mainwindow.h"
// Krita Kasten
#include <bytearraydocumentfactory.h>
#include <bytearrayviewfactory.h>
#include <filesystem/bytearrayrawfilesynchronizerfactory.h>
#include <bytearraystreamencoderconfigeditorfactoryfactory.h>
#include <bytearraydatageneratorconfigeditorfactoryfactory.h>
#include <bytearraystreamencoderfactory.h>
#include <bytearraydatageneratorfactory.h>
// tmp
#include <bytearrayviewprofilemanager.h>
// Kasten gui
#include <multidocumentstrategy.h>
#include <dialoghandler.h>
#include <viewmanager.h>
#include <modelcodecviewmanager.h>
// Kasten core
#include <documentmanager.h>
#include <documentcreatemanager.h>
#include <documentsyncmanager.h>
#include <modelcodecmanager.h>
// KDE
#include <KUrl>
#include <KCmdLineArgs>
#include <KApplication>
// Qt
#include <QtCore/QList>


namespace Kasten2
{

// static const char OffsetOptionId[] = "offset";
// static const char OffsetOptionShortId[] = "o";

#define CmdLineOptionName(STRING) QByteArray::fromRawData( STRING, sizeof(STRING)-1 )

KritaProgram::KritaProgram( int argc, char* argv[] )
  : mDocumentManager( new DocumentManager() ),
    mViewManager( new ViewManager() ),
    mDocumentStrategy( new MultiDocumentStrategy(mDocumentManager, mViewManager) ),
    mDialogHandler( new DialogHandler() )
{
    KCmdLineOptions programOptions;
//     programOptions.add( OffsetOptionShortId );
//     programOptions.add( OffsetOptionId, ki18n("Offset to set the cursor to"), 0 );
    programOptions.add( CmdLineOptionName("+[URL(s)]"), ki18n("File(s) to load") );

    KCmdLineArgs::init( argc, argv, &mAboutData );
    KCmdLineArgs::addCmdLineOptions( programOptions );
}


int KritaProgram::execute()
{
    KApplication programCore;

    // TODO:
    mByteArrayViewProfileManager = new ByteArrayViewProfileManager();
    //mModelManagerManager->addModelManager( byteArrayViewProfileManager );

    const QList<AbstractModelStreamEncoder*> encoderList =
        ByteArrayStreamEncoderFactory::createStreamEncoders();

    const QList<AbstractModelDataGenerator*> generatorList =
        ByteArrayDataGeneratorFactory::createDataGenerators();

    const QList<AbstractModelStreamEncoderConfigEditorFactory*> encoderConfigEditorFactoryList =
        ByteArrayStreamEncoderConfigEditorFactoryFactory::createFactorys();

    const QList<AbstractModelDataGeneratorConfigEditorFactory*> generatorConfigEditorFactoryList =
        ByteArrayDataGeneratorConfigEditorFactoryFactory::createFactorys();

    mDocumentManager->codecManager()->setEncoders( encoderList );
    mDocumentManager->codecManager()->setGenerators( generatorList );
    mDocumentManager->codecManager()->setOverwriteDialog( mDialogHandler );
    mDocumentManager->createManager()->setDocumentFactory( new ByteArrayDocumentFactory() );
    mDocumentManager->syncManager()->setDocumentSynchronizerFactory( new ByteArrayRawFileSynchronizerFactory() );
    mDocumentManager->syncManager()->setOverwriteDialog( mDialogHandler );
    mDocumentManager->syncManager()->setSaveDiscardDialog( mDialogHandler );

    mViewManager->setViewFactory( new ByteArrayViewFactory(mByteArrayViewProfileManager) );
    mViewManager->codecViewManager()->setEncoderConfigEditorFactories( encoderConfigEditorFactoryList );
    mViewManager->codecViewManager()->setGeneratorConfigEditorFactories( generatorConfigEditorFactoryList );

    KritaMainWindow* mainWindow = new KritaMainWindow( this );
    mDialogHandler->setWidget( mainWindow );

    // started by session management?
    if( programCore.isSessionRestored() && KMainWindow::canBeRestored(1) )
    {
        mainWindow->restore( 1 );
    }
    else
    {
        // no session.. just start up normally
        KCmdLineArgs* arguments = KCmdLineArgs::parsedArgs();

        // take arguments
        if( arguments->count() > 0 )
        {
            for( int i=0; i<arguments->count(); ++i )
                mDocumentStrategy->load( arguments->url(i) );
        }

        mainWindow->show();

        arguments->clear();
    }

    return programCore.exec();
}


void KritaProgram::quit()
{
    kapp->quit();
}


KritaProgram::~KritaProgram()
{
    delete mDocumentStrategy;
    delete mDocumentManager;
    delete mViewManager;
    delete mDialogHandler;
    delete mByteArrayViewProfileManager;
}

}
