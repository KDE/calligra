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

#include "toolinlineviewwidget.h"

// lib
#include <abstracttoolinlineview.h>
// KDE
#include <KIcon>
// Qt
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>


namespace Kasten2
{

ToolInlineViewWidget::ToolInlineViewWidget( AbstractToolInlineView* view, QWidget* parent )
  : QWidget( parent ),
    mView( view )
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->addWidget( view->widget(), 10 ); //TODO: find out why this takes ownership of widget to this

    //TODO: use style buttons instead, like QDockWidget
    QToolButton* closeButton = new QToolButton( this );
    closeButton->setAutoRaise( true );
    closeButton->setIcon( KIcon(QLatin1String("dialog-close")) );
    connect( closeButton, SIGNAL(clicked()), SIGNAL(done()) );
    layout->addWidget( closeButton );
    layout->setAlignment( closeButton, (Qt::Alignment)(Qt::AlignLeft|Qt::AlignTop) );
}


AbstractToolInlineView* ToolInlineViewWidget::view() const { return mView; }


ToolInlineViewWidget::~ToolInlineViewWidget()
{
    // TODO: crashes on close of the program if view is still open, because the view is already deleted
    layout()->removeWidget( mView->widget() );
    mView->widget()->setParent( 0 );
}

}
