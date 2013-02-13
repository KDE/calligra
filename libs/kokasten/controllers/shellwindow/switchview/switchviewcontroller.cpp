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

#include "switchviewcontroller.h"

// lib
#include <abstractgroupedviews.h>
// KDE
#include <KXMLGUIClient>
#include <KActionCollection>
#include <KAction>
// Qt
#include <QtGui/QActionGroup>


namespace Kasten2
{

SwitchViewController::SwitchViewController( AbstractGroupedViews* groupedViews, KXMLGUIClient* guiClient )
   : mGroupedViews( groupedViews )
{
    KActionCollection* actionCollection = guiClient->actionCollection();

    mForwardAction = actionCollection->addAction( KStandardAction::Forward, QLatin1String("window_next"), this, SLOT(forward()) );
    mBackwardAction = actionCollection->addAction( KStandardAction::Back, QLatin1String("window_previous"), this, SLOT(backward()) );

    connect( groupedViews, SIGNAL(added(QList<Kasten2::AbstractView*>)),  SLOT(updateActions()) );
    connect( groupedViews, SIGNAL(removing(QList<Kasten2::AbstractView*>)), SLOT(updateActions()) );

    connect( groupedViews, SIGNAL(viewFocusChanged(Kasten2::AbstractView*)), SLOT(updateActions()) );

    updateActions();
}

void SwitchViewController::setTargetModel( AbstractModel* model )
{
Q_UNUSED(model)
}

// TODO: think about moving this properties/abilities (hasNext/Previous,switchToNext/Previous) into a interface for the groupedview
void SwitchViewController::updateActions()
{
    bool hasNext;
    bool hasPrevious;
    const QList<AbstractView*> viewList = mGroupedViews->viewList();
    if( viewList.isEmpty() )
    {
        hasNext = false;
        hasPrevious = false;
    }
    else
    {
        AbstractView* focussedView = mGroupedViews->viewFocus();
        const int indexOfFocussedView = viewList.indexOf( focussedView );

        hasNext = ( indexOfFocussedView+1 < viewList.count() );
        hasPrevious = ( indexOfFocussedView > 0 );
    }

    mForwardAction->setEnabled( hasNext );
    mBackwardAction->setEnabled( hasPrevious );
}


void SwitchViewController::forward()
{
    const QList<AbstractView*> viewList = mGroupedViews->viewList();
    AbstractView* focussedView = mGroupedViews->viewFocus();
    const int indexOfFocussedView = viewList.indexOf( focussedView );
    AbstractView* nextView = viewList.at( indexOfFocussedView + 1 );
    mGroupedViews->setViewFocus( nextView );
}


void SwitchViewController::backward()
{
    const QList<AbstractView*> viewList = mGroupedViews->viewList();
    AbstractView* focussedView = mGroupedViews->viewFocus();
    const int indexOfFocussedView = viewList.indexOf( focussedView );
    AbstractView* previousView = viewList.at( indexOfFocussedView - 1 );
    mGroupedViews->setViewFocus( previousView );
}

}
