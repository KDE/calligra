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

#include "abstracttoolwidget_p.h"

// Qt
#include <QtGui/QPushButton>
#include <QtGui/QShortcut>


namespace Kasten2
{

AbstractToolWidgetPrivate::AbstractToolWidgetPrivate( AbstractToolWidget* parent )
  : p( parent ),
    mDefaultButton( 0 )

{
    mReturnShortcut = new QShortcut( Qt::Key_Return, p ); //TODO: what about Enter?
    mReturnShortcut->setContext( Qt::WidgetWithChildrenShortcut );
    p->connect( mReturnShortcut, SIGNAL(activated()), SLOT(onReturnPressed()) );
}


void AbstractToolWidgetPrivate::addButton( QPushButton* button, AbstractToolWidget::DefaultType defaultType )
{
    button->setAutoDefault( true );
    if( defaultType == AbstractToolWidget::Default )
    {
        if( mDefaultButton )
            mDefaultButton->setDefault( false );

        // TODO: catch button deletion
        mDefaultButton = button;
        button->setDefault( true );
    }
}


void AbstractToolWidgetPrivate::onReturnPressed()
{
    if( mDefaultButton )
        mDefaultButton->click();
}

}
