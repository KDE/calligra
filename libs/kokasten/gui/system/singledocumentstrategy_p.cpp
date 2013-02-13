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

#include "singledocumentstrategy_p.h"

// lib
#include "createdialog.h"
// Kasten gui
#include <abstractmodeldatageneratorconfigeditor.h>
#include <modelcodecviewmanager.h>
#include <viewmanager.h>
// Kasten core
#include <modeldatageneratethread.h>
#include <documentsyncmanager.h>
#include <documentcreatemanager.h>
#include <abstractmodeldatagenerator.h>
#include <abstractmodel.h>
// KDE
#include <KApplication>
#include <KUrl>
#include <KProcess>
// QtGui
#include <QtGui/QClipboard>
#include <QtGui/QApplication>
// QtCore
#include <QtCore/QMimeData>


namespace Kasten2
{

void SingleDocumentStrategyPrivate::init()
{
    Q_Q( SingleDocumentStrategy );
    // setup
    QObject::connect( mDocumentManager, SIGNAL(added(QList<Kasten2::AbstractDocument*>)),
                      mViewManager, SLOT(createViewsFor(QList<Kasten2::AbstractDocument*>)) );
    QObject::connect( mDocumentManager, SIGNAL(closing(QList<Kasten2::AbstractDocument*>)),
                      mViewManager, SLOT(removeViewsFor(QList<Kasten2::AbstractDocument*>)) );
    QObject::connect( mDocumentManager->syncManager(), SIGNAL(urlUsed(KUrl)),
                      q, SIGNAL(urlUsed(KUrl)) );
}

void SingleDocumentStrategyPrivate::createNew()
{
    if( mDocumentManager->isEmpty() )
        mDocumentManager->createManager()->createNew();
    else
    {
        const QString executable = QCoreApplication::applicationFilePath();
        // TODO: get parameters from common place with KCmdLineOptions
        // TODO: forward also interesting parameters passed to this program
        const QStringList parameters = QStringList() << QLatin1String( "-c" );
        KProcess::startDetached( executable, parameters );
    }
}

void SingleDocumentStrategyPrivate::createNewFromClipboard()
{
    if( mDocumentManager->isEmpty() )
    {
        const QMimeData* mimeData =
            QApplication::clipboard()->mimeData( QClipboard::Clipboard );

        mDocumentManager->createManager()->createNewFromData( mimeData, true );
    }
    else
    {
        const QString executable = QCoreApplication::applicationFilePath();
        // TODO: get parameters from common place with KCmdLineOptions
        // TODO: forward also interesting parameters passed to this program
        const QStringList parameters = QStringList()
            << QLatin1String( "-c" )
            << QLatin1String( "-g" )
            << QLatin1String( "FromClipboard" );
        KProcess::startDetached( executable, parameters );
    }
}

void SingleDocumentStrategyPrivate::createNewWithGenerator( AbstractModelDataGenerator* generator )
{
    Q_Q( SingleDocumentStrategy );

    // TODO: show dialog in this process, meanwhile start other process, but hidden,
    // on result of dialog pass on the parameters
    if( ! mDocumentManager->isEmpty() )
    {
        const QString executable = QCoreApplication::applicationFilePath();
        // TODO: get parameters from common place with KCmdLineOptions
        // TODO: forward also interesting parameters passed to this program
        // TODO: add id to AbstractModelDataGenerator, to use instead of className
        const QStringList parameters = QStringList()
            << QLatin1String( "-c" )
            << QLatin1String( "-g" )
            << QLatin1String(generator->metaObject()->className());
        KProcess::startDetached( executable, parameters );
        return;
    }

    AbstractModelDataGeneratorConfigEditor* configEditor =
        mViewManager->codecViewManager()->createConfigEditor( generator );

    if( configEditor )
    {
        // TODO: make dialog abstract for different UIs
        CreateDialog* dialog = new CreateDialog( configEditor );
//         dialog->setData( mModel, selection ); TODO
        if( ! dialog->exec() )
            return;
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    ModelDataGenerateThread* generateThread =
        new ModelDataGenerateThread( q, generator );
    generateThread->start();
    while( !generateThread->wait(100) )
        QApplication::processEvents( QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers, 100 );

    QMimeData* mimeData = generateThread->data();

    delete generateThread;

    const bool setModified = ( generator->flags() & AbstractModelDataGenerator::DynamicGeneration );
    mDocumentManager->createManager()->createNewFromData( mimeData, setModified );

    QApplication::restoreOverrideCursor();
}

void SingleDocumentStrategyPrivate::load( const KUrl& url )
{
    if( mDocumentManager->isEmpty() )
        mDocumentManager->syncManager()->load( url );
    else
    {
        const QString executable = QCoreApplication::applicationFilePath();
        // TODO: get parameters from common place with KCmdLineOptions
        // TODO: forward also interesting parameters passed to this program
        const QStringList parameters = QStringList() << url.url();
        KProcess::startDetached( executable, parameters );
    }
}

}
