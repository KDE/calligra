/* This file is part of the KDE project
   Copyright (C) 2005 Inge Wallin <inge@lysator.liu>

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
 * Boston, MA 02110-1301, USA
*/


#include "WUFactory.h"
#include "WUDocument.h"
#include "WUView.h"

#include <koView.h>
#include <koMainWindow.h>

#include <kaction.h>
#include <kactioncollection.h>
#include <kmenubar.h>
#include <kparts/event.h>


WUView::WUView( KWViewMode* viewMode, QWidget *_parent, const char *_name, KWDocument* _doc )
    : KWView( viewMode, _parent, _name, _doc )
{
    setInstance( WUFactory::instance() );
    if ( !kWordDocument()->isReadWrite() )
        setXMLFile( "writeup_readonly.rc" );
    else
        setXMLFile( "writeup.rc" );
}


WUView::~WUView()
{
}


// Reimplemented from KWView

void WUView::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
    KWView::guiActivateEvent( ev );

    //mainWindow()->menuBar()->hide();

    KToggleAction* actionShowDocStruct = dynamic_cast<KToggleAction *> (actionCollection()->action("show_docstruct"));
    KToggleAction* actionShowRuler     = dynamic_cast<KToggleAction *> (actionCollection()->action("show_ruler"));

    if (actionShowDocStruct) actionShowDocStruct->setEnabled(false);
    if (actionShowRuler)     actionShowRuler    ->setEnabled(false);
}





#include "WUView.moc"
