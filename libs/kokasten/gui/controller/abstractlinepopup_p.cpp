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

#include "abstractlinepopup_p.h"

// KDE
#include <KIcon>
// Qt
#include <QtGui/QLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QApplication>


namespace Kasten2
{

void AbstractLinePopupPrivate::init()
{
    // TODO: what kind of border should there be? like a QMenu?

//     setAttribute( Qt::WA_DeleteOnClose );
    p->setMouseTracking( true );

    mBaseLayout = new QHBoxLayout( p );
    mBaseLayout->setMargin( 0 );
    mBaseLayout->setSpacing( 0 );

    mIconLabel = new QLabel( p );
    mBaseLayout->addWidget( mIconLabel );

    qApp->installEventFilter( p );
}


void AbstractLinePopupPrivate::setIcon( const KIcon& icon )
{
    mIconLabel->setPixmap( icon.pixmap(22) ); // TODO: correct call, using KDE size
}

void AbstractLinePopupPrivate::setWidget( QWidget* widget )
{
    mWidget = widget;
    mBaseLayout->addWidget( widget, 10 );
}

void AbstractLinePopupPrivate::setPosition( const QPoint& globalPosition )
{
    p->move( globalPosition.x(), globalPosition.y() - p->height() );
}

void AbstractLinePopupPrivate::setVisible( bool visible )
{
    p->QWidget::setVisible( visible );
    if( mEventLoop )
        mEventLoop->exit();
}

int AbstractLinePopupPrivate::exec()
{
    if( mWidget )
        mWidget->setFocus();
    p->show();

    QEventLoop eventLoop;
    mEventLoop = &eventLoop;
    eventLoop.exec();
    mEventLoop = 0;

    return mResult;
}

}
