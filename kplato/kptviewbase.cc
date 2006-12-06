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

#include "kptpart.h"
#include "kptproject.h"
#include "kptview.h"

#include <kparts/event.h>
#include <kxmlguifactory.h>

#include <KoDocument.h>


namespace KPlato
{
    
//--------------
ViewBase::ViewBase(Part *doc, QWidget *parent)
    : KoView( doc, parent )
{
}
    
Part *ViewBase::part() const
{
     return static_cast<Part*>( koDocument() );
}

void ViewBase::updateReadWrite( bool /*readwrite*/ )
{
}

void ViewBase::setGuiActive( bool active ) // virtual slot
{
    kDebug()<<k_funcinfo<<active<<endl;
    emit guiActivated( this, active );
}

} // namespace KPlato

#include "kptviewbase.moc"
