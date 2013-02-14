/*
 *  Copyright (c) 2013 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_program.h"

// program
#include "kis_mainwindow.h"
#include "kis_document_factory.h"
#include "kis_application.h"

#include "data/splash/splash_screen.xpm"

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
#include <KApplication>

// Qt
#include <QtCore/QList>
#include <QSplashScreen>

// KO
#include <KoPluginLoader.h>


namespace Kasten2
{

// static const char OffsetOptionId[] = "offset";
// static const char OffsetOptionShortId[] = "o";

#define CmdLineOptionName(STRING) QByteArray::fromRawData( STRING, sizeof(STRING)-1 )

KritaProgram::KritaProgram( int argc, char* argv[] )
    : mDocumentManager( new DocumentManager() )
    , mViewManager( new ViewManager() )
    , mDocumentStrategy( new MultiDocumentStrategy(mDocumentManager, mViewManager) )
    , mDialogHandler( new DialogHandler() )
{
    KisApplication::createCommandLineOptions();

    // Create the pixmap from an xpm for the splash: we cannot get the
    // location of our datadir before we've started our components,
    // so use an xpm.
    QPixmap pm(splash_screen_xpm);
    m_splashScreen = new KSplashScreen(pm);
    m_splashScreen->show();
    m_splashScreen->showMessage(".");
}


int KritaProgram::execute()
{
    KApplication programCore;

    mDocumentManager->createManager()->setDocumentFactory( new KisDocumentFactory() );
//    mDocumentManager->syncManager()->setDocumentSynchronizerFactory( new ByteArrayRawFileSynchronizerFactory() );
    mDocumentManager->syncManager()->setOverwriteDialog( mDialogHandler );
    mDocumentManager->syncManager()->setSaveDiscardDialog( mDialogHandler );

    mViewManager->setViewFactory( new ByteArrayViewFactory(mByteArrayViewProfileManager) );

    KritaMainWindow* mainWindow = new KritaMainWindow( this );
    mDialogHandler->setWidget( mainWindow );

    // started by session management?
    if( programCore.isSessionRestored() && KMainWindow::canBeRestored(1) ) {
        mainWindow->restore( 1 );
    }
    else {
        // no session.. just start up normally
        KCmdLineArgs* arguments = KCmdLineArgs::parsedArgs();

        // take arguments
        if( arguments->count() > 0 )
        {
            for( int i=0; i < arguments->count(); ++i ) {
                mDocumentStrategy->load( arguments->url(i) );
            }

        }

        mainWindow->show();
        arguments->clear();
    }

    m_splashScreen->hide();
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
}

}
