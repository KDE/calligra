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

#include "viewbox.h"

// lib
#include "abstractview.h"
// Qt
#include <QtGui/QVBoxLayout>


namespace Kasten2
{

ViewBox::ViewBox( AbstractView* view, QWidget* parent )
  : QWidget( parent ),
    mView( view )
{
    QWidget* widget = view->widget();
    setFocusProxy( widget );

    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setMargin (0);
    layout->setSpacing (0);
    layout->addWidget( widget );
}


AbstractView* ViewBox::view() const { return mView; }


void ViewBox::add( ViewBox::Area area )
{
    Q_UNUSED( area );
}

ViewBox::~ViewBox()
{
    mView->widget()->setParent( 0 );
}

}
