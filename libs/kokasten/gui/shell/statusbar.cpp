/*
    This file is part of the Kasten Framework, made within the KDE community.

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

#include "statusbar.h"

// lib
#include "statusbarlayout.h"
// Qt
#include <QtCore/QEvent>


namespace Kasten2
{

StatusBar::StatusBar( QWidget* parent )
  : KStatusBar( parent )
{
//     setMinimumWidth( 1 );

    QWidget* baseWidget = new QWidget( this );

    mLayout = new StatusBarLayout( baseWidget );
    mLayout->setSpacing( 6 ); // hard coded in QStatusBar
    KStatusBar::addWidget( baseWidget, 1 );
}


void StatusBar::addWidget( QWidget* widget )
{
    mLayout->addWidget( widget );
}

void StatusBar::updateLayout()
{
    mLayout->invalidate();
}


void StatusBar::changeEvent( QEvent* event )
{
    switch( event->type() )
    {
    case QEvent::StyleChange:
        mLayout->invalidate();
//         mLayout->updateMarginAndSpacing();
        break;
    default:
        break;
    }

    KStatusBar::changeEvent( event );
}


StatusBar::~StatusBar()
{
}

}
