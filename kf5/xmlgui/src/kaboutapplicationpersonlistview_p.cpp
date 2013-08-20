/* This file is part of the KDE libraries
   Copyright (C) 2010 Teo Mrnjavac <teo@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kaboutapplicationpersonlistview_p.h"

#include <QScrollBar>

namespace KDEPrivate
{

KAboutApplicationPersonListView::KAboutApplicationPersonListView( QWidget *parent )
    : QListView( parent )
{
    setVerticalScrollMode( ScrollPerPixel );
    setFrameShape( QFrame::NoFrame );

    QPalette p = palette();
    QColor c = p.color( QPalette::Base );
    c.setAlpha( 0 );
    p.setColor( QPalette::Base, c );
    setBackgroundRole( QPalette::Base );
    setPalette( p );
    setSelectionMode( NoSelection );
    setEditTriggers( NoEditTriggers );
}

void KAboutApplicationPersonListView::wheelEvent( QWheelEvent *e )
{
    //HACK: Workaround for Qt bug 7232: Smooth scrolling (scroll per pixel) in ItemViews
    //      does not work as expected.
    verticalScrollBar()->setSingleStep( 3 );
    QListView::wheelEvent( e );
}

} //namespace KDEPrivate
