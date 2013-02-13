/*
    This file is part of the Okteta Kasten module, made within the KDE community.

    Copyright 2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "readonlybarcontroller.h"

// lib
#include <abstractdocument.h>
// Kasten ui
#include <togglebutton.h>
#include <statusbar.h>
// KDE
#include <KLocale>
#include <KIcon>


namespace Kasten2
{

ReadOnlyBarController::ReadOnlyBarController( StatusBar* statusBar )
 : mDocument( 0 )
{
    const QString readWriteText = i18nc( "@option:check the document is read-write", "Read-write" );
    const QString readOnlyText = i18nc( "@option:check the document is read-only", "Read-only" );
    mReadOnlyButton = new ToggleButton( KIcon( QLatin1String("object-unlocked") ), QString(), readWriteText, statusBar );
    mReadOnlyButton->setCheckedState( KIcon( QLatin1String("object-locked") ), QString(), readOnlyText );
    statusBar->addWidget( mReadOnlyButton );
    connect( mReadOnlyButton, SIGNAL(clicked(bool)), SLOT(setReadOnly(bool)) );

    setTargetModel( 0 );
}


void ReadOnlyBarController::setTargetModel( AbstractModel* model )
{
    if( mDocument ) mDocument->disconnect( mReadOnlyButton );

    mDocument = model ? model->findBaseModel<AbstractDocument*>() : 0;

    if( mDocument )
    {
        mReadOnlyButton->setChecked( mDocument->isReadOnly() );

        connect( mDocument, SIGNAL(readOnlyChanged(bool)),
                 mReadOnlyButton, SLOT(setChecked(bool)) );
        connect( mDocument, SIGNAL(modifiableChanged(bool)),
                 mReadOnlyButton, SLOT(setEnabled(bool)) );
    }
    else
    {
        mReadOnlyButton->setChecked( false );
    }

    mReadOnlyButton->setEnabled( mDocument ? mDocument->isModifiable() : false );
}


void ReadOnlyBarController::setReadOnly( bool isReadOnly )
{
    mDocument->setReadOnly( isReadOnly );
}

}
