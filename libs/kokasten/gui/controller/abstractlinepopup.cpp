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

#include "abstractlinepopup.h"
#include "abstractlinepopup_p.h"

// Qt
#include <QtCore/QEvent>


namespace Kasten2
{

AbstractLinePopup::AbstractLinePopup( QWidget* parent )
  : QWidget( parent, Qt::Popup ),
    d( new AbstractLinePopupPrivate(this) )
{
    d->init();
}


void AbstractLinePopup::setPosition( const QPoint& globalPosition )
{
    d->setPosition( globalPosition );
}
void AbstractLinePopup::setIcon( const KIcon& icon )
{
    d->setIcon( icon );
}
void AbstractLinePopup::setWidget( QWidget* widget )
{
    d->setWidget( widget );
}
void AbstractLinePopup::setResult( int result )
{
    d->setResult( result );
}

void AbstractLinePopup::setVisible( bool visible )
{
    d->setVisible( visible );
}

int AbstractLinePopup::exec()
{
    return d->exec();
}

bool AbstractLinePopup::eventFilter( QObject* object, QEvent* event )
{
    switch( event->type() )
    {
    case QEvent::WindowDeactivate:
        close();
        break;
    case QEvent::MouseButtonPress:
    case QEvent::Wheel:
        if( !d->isOrContainsObject(object) )
            close();
        break;
    default:
        break;
    }

    return false;
}

AbstractLinePopup::~AbstractLinePopup()
{
    delete d;
}

}
