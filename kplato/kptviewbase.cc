/* This file is part of the KDE project
  Copyright (C) 2006 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include "kptviewbase.h"

#include "kptproject.h"
#include "kptview.h"

#include <kparts/event.h>
#include <kxmlguifactory.h>


namespace KPlato
{
    
//--------------
ViewBase::ViewBase(View *mainview, QWidget *parent)
    : KoView( mainview->koDocument(), parent ), //QWidget(parent),
    m_mainview(mainview)
{
}

ViewBase::ViewBase(KoDocument *doc, QWidget *parent)
    : KoView( doc, parent ), //QWidget(parent),
    m_mainview(0)
{
}
    
View *ViewBase::mainView() const
{
    return m_mainview;
}

void ViewBase::updateReadWrite( bool /*readwrite*/ )
{
}

void ViewBase::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
    KoView *v = dynamic_cast<KoView*>( parentWidget() );
    KXMLGUIFactory *f = 0;
    if ( m_mainview ) {
        f = m_mainview->factory();
    }
    kDebug()<<k_funcinfo<<this<<" "<<ev->activated()<<", "<<f<<endl;
    setViewActive( ev->activated(), f );
}
void ViewBase::setViewActive( bool active, KXMLGUIFactory*) // slot
{
/*    if ( mainView() )
    mainView()->setTaskActionsEnabled( this, active );*/
}

void ViewBase::addActions( KXMLGUIFactory *factory )
{
    //kDebug()<<k_funcinfo<<this<<endl;
    if (factory ) {
        factory->addClient( this );
    }
}

void ViewBase::removeActions()
{
    //kDebug()<<k_funcinfo<<this<<endl;
    if ( factory() ) {
        factory()->removeClient( this );
    }
}

} // namespace KPlato

#include "kptviewbase.moc"
