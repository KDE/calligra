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

#include "viewareabox.h"

// Qt
#include <QtGui/QVBoxLayout>
#include <QtGui/QShortcut>


namespace Kasten2
{

ViewAreaBox::ViewAreaBox( QWidget* centralWidget, QWidget* parent )
  : QWidget( parent ),
    mCentralWidget( centralWidget ),
    mBottomWidget( 0 )
{
    setFocusProxy( mCentralWidget );

    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    if( mCentralWidget )
        layout->addWidget( mCentralWidget );

    mEscapeShortcut = new QShortcut( Qt::Key_Escape, this );
    mEscapeShortcut->setEnabled( false );
    connect( mEscapeShortcut, SIGNAL(activated()), SLOT(onDone()) ); // TODO: better use onCancelled
}


QWidget* ViewAreaBox::centralWidget() const { return mCentralWidget; }
QWidget* ViewAreaBox::bottomWidget()  const { return mBottomWidget; }

void ViewAreaBox::setCentralWidget( QWidget* centralWidget )
{
    if( mCentralWidget == centralWidget )
        return;

    QVBoxLayout* layout = static_cast<QVBoxLayout*>( this->layout() );
    const bool centralWidgetIsFocusProxy =
        mCentralWidget ? ( focusProxy() == mCentralWidget ) : false;
    // TODO: works if focus is on childwidget?
    const bool centralWidgetHasFocus =
        mCentralWidget ? mCentralWidget->hasFocus() : false;
    if( mCentralWidget )
        layout->removeWidget( mCentralWidget );

    mCentralWidget = centralWidget;

    if( mCentralWidget )
    {
        layout->insertWidget( 0, mCentralWidget );
        mCentralWidget->show(); // TODO: needed?
        if( centralWidgetHasFocus )
            mCentralWidget->setFocus();
        if( centralWidgetIsFocusProxy )
            setFocusProxy( mCentralWidget );
    }
}

void ViewAreaBox::setBottomWidget( QWidget* bottomWidget )
{
    QVBoxLayout* layout = static_cast<QVBoxLayout*>( this->layout() );

    if( mBottomWidget )
    {
        mBottomWidget->disconnect( this );
        layout->removeWidget( mBottomWidget );
        delete mBottomWidget;
    }

    mBottomWidget = bottomWidget;
    if( bottomWidget )
    {
        setFocusProxy( bottomWidget );
        connect( bottomWidget, SIGNAL(done()), SLOT(onDone()) );
        layout->addWidget( bottomWidget );
        bottomWidget->show();
        bottomWidget->setFocus();
    }
    else
        setFocusProxy( mCentralWidget );

    mEscapeShortcut->setEnabled( (bottomWidget != 0) );
}

void ViewAreaBox::onDone()
{
    setBottomWidget( 0 );
}

ViewAreaBox::~ViewAreaBox()
{
    delete mBottomWidget;
    if( mCentralWidget )
        mCentralWidget->setParent( 0 );
}

}
