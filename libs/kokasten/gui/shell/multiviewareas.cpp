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

#include "multiviewareas.h"
#include "multiviewareas_p.h"
#include "multiviewareas.moc"


namespace Kasten2
{
// TODO: catch area focues change!
MultiViewAreas::MultiViewAreas()
  : AbstractGroupedViews( new MultiViewAreasPrivate(this) )
{
    Q_D( MultiViewAreas );

    d->init();
}

QList<AbstractView*> MultiViewAreas::viewList() const
{
    Q_D( const MultiViewAreas );

    return d->viewList();
}

int MultiViewAreas::viewCount() const
{
    Q_D( const MultiViewAreas );

    return d->viewCount();
}

int MultiViewAreas::indexOf( AbstractView* view ) const
{
    Q_D( const MultiViewAreas );

    return d->indexOf( view );
}

QWidget* MultiViewAreas::widget() const
{
    Q_D( const MultiViewAreas );

    return d->widget();
}

bool MultiViewAreas::hasFocus() const
{
    Q_D( const MultiViewAreas );

    return d->hasFocus();
}

AbstractView* MultiViewAreas::viewFocus() const
{
    Q_D( const MultiViewAreas );

    return d->viewFocus();
}

AbstractViewArea* MultiViewAreas::viewAreaFocus() const
{
    Q_D( const MultiViewAreas );

    return d->viewAreaFocus();
}

int MultiViewAreas::viewAreasCount() const
{
    Q_D( const MultiViewAreas );

    return d->viewAreasCount(); }


void MultiViewAreas::setFocus()
{
    Q_D( MultiViewAreas );

    d->setFocus();
}

void MultiViewAreas::addViews( const QList<AbstractView*>& views )
{
    Q_D( MultiViewAreas );

    d->addViews( views );
}

void MultiViewAreas::removeViews( const QList<AbstractView*>& views )
{
    Q_D( MultiViewAreas );

    d->removeViews( views );
}

void MultiViewAreas::setCurrentToolInlineView( AbstractToolInlineView* view )
{
    Q_D( MultiViewAreas );

    d->setCurrentToolInlineView( view );
}

void MultiViewAreas::setViewFocus( AbstractView* view )
{
    Q_D( MultiViewAreas );

    d->setViewFocus( view );
}

 //TODO: this method could be removed, as it is the same as viewArea->setFocus(), or?
void MultiViewAreas::setViewAreaFocus( AbstractViewArea* viewArea )
{
    Q_D( MultiViewAreas );

    d->setViewAreaFocus( viewArea );
}

AbstractViewArea* MultiViewAreas::splitViewArea( AbstractViewArea* viewArea, Qt::Orientation orientation )
{
    Q_D( MultiViewAreas );

    return d->splitViewArea( viewArea, orientation );
}

void MultiViewAreas::closeViewArea( AbstractViewArea* viewArea )
{
    Q_D( MultiViewAreas );

    d->closeViewArea( viewArea );
}

MultiViewAreas::~MultiViewAreas()
{
}

}
