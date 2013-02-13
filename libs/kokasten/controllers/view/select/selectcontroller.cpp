/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2006-2008 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "selectcontroller.h"

// Kasten gui
#include <dataselectable.h>
#include <abstractview.h>
// KDE
#include <KXMLGUIClient>
#include <KLocale>
#include <KAction>
#include <KActionCollection>
#include <KStandardAction>


namespace Kasten2
{

SelectController::SelectController( KXMLGUIClient* guiClient )
 : mModel( 0 ), mSelectControl( 0 )
{
    KActionCollection* actionCollection = guiClient->actionCollection();

    mSelectAllAction = KStandardAction::selectAll( this, SLOT(selectAll()), actionCollection );
    mDeselectAction =  KStandardAction::deselect(  this, SLOT(unselect()),  actionCollection );

    setTargetModel( 0 );
}

void SelectController::setTargetModel( AbstractModel* model )
{
    if( mModel ) mModel->disconnect( this );

    mModel = model ? model->findBaseModelWithInterface<If::DataSelectable*>() : 0;
    mSelectControl = mModel ? qobject_cast<If::DataSelectable *>( mModel ) : 0;

    const bool hasSelectionControl = ( mSelectControl != 0 );
    if( hasSelectionControl )
    {
        connect( mModel, SIGNAL(hasSelectedDataChanged(bool)), SLOT(onHasSelectedDataChanged(bool)) );
    }

    mSelectAllAction->setEnabled( hasSelectionControl );
    mDeselectAction->setEnabled( hasSelectionControl ? mSelectControl->hasSelectedData() : false );
}


void SelectController::onHasSelectedDataChanged( bool hasSelectedData )
{
    mDeselectAction->setEnabled( hasSelectedData );
}


void SelectController::selectAll()
{
    mSelectControl->selectAllData( true );
}

void SelectController::unselect()
{
    mSelectControl->selectAllData( false );
}

}
