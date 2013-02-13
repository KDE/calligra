/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2007-2008,2011 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "copyascontroller.h"

// lib
#include "copyasdialog.h"
// Kasten gui
#include <modelcodecviewmanager.h>
#include <dataselectable.h>
#include <abstractmodelstreamencoderconfigeditor.h>
// Kasten core
#include <modelstreamencodethread.h>
#include <modelcodecmanager.h>
#include <abstractmodelstreamencoder.h>
#include <abstractmodel.h>
// KDE
#include <KXMLGUIClient>
#include <KXMLGUIFactory>
#include <KActionCollection>
#include <KLocale>
#include <KSelectAction>
// Qt
#include <QtCore/QBuffer>
#include <QtCore/QMimeData>
#include <QtGui/QClipboard>
#include <QtGui/QApplication>


Q_DECLARE_METATYPE(Kasten2::AbstractModelStreamEncoder*)

namespace Kasten2
{

CopyAsController::CopyAsController( ModelCodecViewManager* modelCodecViewManager,
                                    ModelCodecManager* modelCodecManager,
                                    KXMLGUIClient* guiClient )
  : AbstractXmlGuiController(),
    mModelCodecViewManager( modelCodecViewManager ),
    mModelCodecManager( modelCodecManager ),
    mModel( 0 )
{
    KActionCollection* actionCollection = guiClient->actionCollection();

    mCopyAsSelectAction = actionCollection->add<KSelectAction>( QLatin1String("copy_as") );
    mCopyAsSelectAction->setText( i18nc("@title:menu","Copy As") );
    mCopyAsSelectAction->setIcon( KIcon( QLatin1String("edit-copy") ) );
    mCopyAsSelectAction->setToolBarMode( KSelectAction::MenuMode );
    connect( mCopyAsSelectAction, SIGNAL(triggered(QAction*)), SLOT(onActionTriggered(QAction*)) );

    setTargetModel( 0 );
}

void CopyAsController::setTargetModel( AbstractModel* model )
{
    if( mModel ) mModel->disconnect( this );

    mModel = model ? model->findBaseModelWithInterface<If::DataSelectable*>() : 0;
    mSelectionControl = mModel ? qobject_cast<If::DataSelectable*>( mModel ) : 0;

    if( mSelectionControl )
    {
        // TODO: only fill the list on menu call...
        connect( mModel, SIGNAL(hasSelectedDataChanged(bool)), SLOT(updateActions()) );
    }

    updateActions();
}


void CopyAsController::updateActions()
{
    mCopyAsSelectAction->removeAllActions();

    const AbstractModelSelection* selection = ( mSelectionControl != 0 ) ? mSelectionControl->modelSelection() : 0;

    const QList<AbstractModelStreamEncoder*> encoderList =
        mModelCodecManager->encoderList( mModel, selection );
    const bool hasEncoders = ( encoderList.size() > 0 );

    if( hasEncoders )
    {
        for( int c = 0; c < encoderList.size(); ++c )
        {
            AbstractModelStreamEncoder* encoder = encoderList.at( c );
            const QString title = encoder->remoteTypeName();
            QAction* action = new QAction( title, mCopyAsSelectAction );

            action->setData( QVariant::fromValue(encoder) );
            mCopyAsSelectAction->addAction( action );
        }
    }
    else
    {
        QAction* noneAction = new QAction( i18nc("@item There are no encoders.","Not available."), mCopyAsSelectAction );
        noneAction->setEnabled( false );
        mCopyAsSelectAction->addAction( noneAction );
    }

    // TODO: need a call AbstractModelSelection::isEmpty
    mCopyAsSelectAction->setEnabled( mSelectionControl && mSelectionControl->hasSelectedData() );
}

void CopyAsController::onActionTriggered( QAction *action )
{
    AbstractModelStreamEncoder* encoder = action->data().value<AbstractModelStreamEncoder* >();

    const AbstractModelSelection* selection = mSelectionControl->modelSelection();

    AbstractModelStreamEncoderConfigEditor* configEditor =
        mModelCodecViewManager->createConfigEditor( encoder );

    if( configEditor )
    {
        CopyAsDialog* dialog = new CopyAsDialog( encoder->remoteTypeName(), configEditor );
        dialog->setData( mModel, selection );
        if( !dialog->exec() )
            return;
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    QByteArray exportData;
    QBuffer exportDataBuffer( &exportData );
    exportDataBuffer.open( QIODevice::WriteOnly );

    ModelStreamEncodeThread *encodeThread =
        new ModelStreamEncodeThread( this, &exportDataBuffer, mModel, selection, encoder );
    encodeThread->start();
    while( !encodeThread->wait(100) )
        QApplication::processEvents( QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers, 100 );

    delete encodeThread;

    exportDataBuffer.close();

    QMimeData *mimeData = new QMimeData;
    mimeData->setData( encoder->remoteClipboardMimeType(), exportData );
    QApplication::clipboard()->setMimeData( mimeData, QClipboard::Clipboard );

    QApplication::restoreOverrideCursor();
}

}
