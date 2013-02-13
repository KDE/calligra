/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2007,2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "tabbedviews.h"
#include "tabbedviews_p.h"
#include "tabbedviews.moc"


namespace Kasten2
{

TabbedViews::TabbedViews( TabbedViewsPrivate* _d )
  : AbstractGroupedViews( _d )
{
    Q_D( TabbedViews );

    d->init();
}

TabbedViews::TabbedViews()
  : AbstractGroupedViews( new TabbedViewsPrivate(this) )
{
    Q_D( TabbedViews );

    d->init();
}

QList<AbstractView*> TabbedViews::viewList() const
{
    Q_D( const TabbedViews );

    return d->viewList();
}

int TabbedViews::viewCount() const
{
    Q_D( const TabbedViews );

    return d->viewCount();
}

int TabbedViews::indexOf( AbstractView* view ) const
{
    Q_D( const TabbedViews );

    return d->indexOf( view );
}

QWidget* TabbedViews::widget() const
{
    Q_D( const TabbedViews );

    return d->widget();
}

AbstractView* TabbedViews::viewFocus() const
{
    Q_D( const TabbedViews );

    return d->viewFocus();
}

bool TabbedViews::hasFocus() const
{
    Q_D( const TabbedViews );

    return d->hasFocus();
}


void TabbedViews::addViews( const QList<AbstractView*>& views )
{
    Q_D( TabbedViews );

    d->addViews( views );
}

void TabbedViews::removeViews( const QList<AbstractView*>& views )
{
    Q_D( TabbedViews );

    d->removeViews( views );
}

void TabbedViews::setCurrentToolInlineView( AbstractToolInlineView* view )
{
    Q_D( TabbedViews );

    d->setCurrentToolInlineView( view );
}

void TabbedViews::setViewFocus( AbstractView* view )
{
    Q_D( TabbedViews );

    d->setViewFocus( view );
}

void TabbedViews::setFocus()
{
    Q_D( TabbedViews );

    d->setFocus();
}


TabbedViews::~TabbedViews()
{
}

}
