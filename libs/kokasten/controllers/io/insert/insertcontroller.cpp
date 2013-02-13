/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2009,2011 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "insertcontroller.h"

// lib
#include "insertdialog.h"
// Kasten gui
#include <modelcodecviewmanager.h>
#include <selecteddatawriteable.h>
#include <abstractmodeldatageneratorconfigeditor.h>
// Kasten core
#include <modeldatageneratethread.h>
#include <modelcodecmanager.h>
#include <abstractmodeldatagenerator.h>
#include <abstractmodel.h>
// KDE
#include <KXMLGUIClient>
#include <KXMLGUIFactory>
#include <KActionCollection>
#include <KLocale>
#include <KSelectAction>
// Qt
#include <QtCore/QMimeData>
#include <QtGui/QApplication>


#ifndef ABSTRACTMODELDATAGENERATOR_METATYPE
#define ABSTRACTMODELDATAGENERATOR_METATYPE
Q_DECLARE_METATYPE(Kasten2::AbstractModelDataGenerator*)
#endif

namespace Kasten2
{

InsertController::InsertController( ModelCodecViewManager* modelCodecViewManager,
                                    ModelCodecManager* modelCodecManager,
                                    KXMLGUIClient* guiClient )
  : AbstractXmlGuiController(),
    mModelCodecViewManager( modelCodecViewManager ),
    mModelCodecManager( modelCodecManager ),
    mModel( 0 )
{
    KActionCollection* actionCollection = guiClient->actionCollection();

    mInsertSelectAction = actionCollection->add<KSelectAction>( QLatin1String("insert") ); //TODO: find better id
    mInsertSelectAction->setText( i18nc("@title:menu","Insert") );
//     mInsertSelectAction->setIcon( KIcon("insert-text") );
    mInsertSelectAction->setToolBarMode( KSelectAction::MenuMode );
    connect( mInsertSelectAction, SIGNAL(triggered(QAction*)), SLOT(onActionTriggered(QAction*)) );

    setTargetModel( 0 );
}

void InsertController::setTargetModel( AbstractModel* model )
{
    if( mModel ) mModel->disconnect( this );

    mModel = model ? model->findBaseModelWithInterface<If::SelectedDataWriteable*>() : 0;
    mSelectedDataWriteableControl = mModel ? qobject_cast<If::SelectedDataWriteable*>( mModel ) : 0;

    if( mSelectedDataWriteableControl )
    {
        // TODO: only fill the list on menu call...
        connect( mModel, SIGNAL(readOnlyChanged(bool)), SLOT(onReadOnlyChanged(bool)) );
    }

    updateActions();
}


void InsertController::updateActions()
{
    mInsertSelectAction->removeAllActions();

    // TODO: pass model to find which mimetypes it can read
    // mSelectedDataWriteableControl->canReadData( QMimeData() ) needs already data
    // TODO: it this depend on the current selection/focus? So it needs to be updated on every change?
    const QList<AbstractModelDataGenerator*> generatorList =
        mModelCodecManager->generatorList();
    const bool hasGenerators = ( generatorList.size() > 0 );

    if( hasGenerators )
    {
        foreach( AbstractModelDataGenerator* generator, generatorList )
        {
            const QString title = generator->typeName();
            QAction* action = new QAction( title, mInsertSelectAction );

            action->setData( QVariant::fromValue(generator) );
            mInsertSelectAction->addAction( action );
        }
    }
    else
    {
        QAction* noneAction = new QAction( i18nc("@item There are no generators.","Not available."), mInsertSelectAction );
        noneAction->setEnabled( false );
        mInsertSelectAction->addAction( noneAction );
    }

    // TODO: need a call AbstractModelSelection::isEmpty
    const bool isWriteable = ( mSelectedDataWriteableControl && ! mModel->isReadOnly() );
    mInsertSelectAction->setEnabled( isWriteable );
}

void InsertController::onActionTriggered( QAction *action )
{
    AbstractModelDataGenerator* generator = action->data().value<AbstractModelDataGenerator* >();

    AbstractModelDataGeneratorConfigEditor* configEditor =
        mModelCodecViewManager->createConfigEditor( generator );

    if( configEditor )
    {
        InsertDialog* dialog = new InsertDialog( configEditor );
//         dialog->setData( mModel, selection ); TODO
        if( ! dialog->exec() )
            return;
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    ModelDataGenerateThread* generateThread =
        new ModelDataGenerateThread( this, generator );
    generateThread->start();
    while( !generateThread->wait(100) )
        QApplication::processEvents( QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers, 100 );

    QMimeData* mimeData = generateThread->data();

    delete generateThread;

    mSelectedDataWriteableControl->insertData( mimeData );

    QApplication::restoreOverrideCursor();
}


void InsertController::onReadOnlyChanged( bool isReadOnly )
{
    const bool isWriteable = ( ! isReadOnly );

    mInsertSelectAction->setEnabled( isWriteable );
}

}
