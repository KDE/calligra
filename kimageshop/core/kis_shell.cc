/*
 *  kis_shell.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter  <me@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <kglobal.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kstatusbar.h>
#include <kfiledialog.h>
#include <kmessagebox.h>

#include <koFilterManager.h>

#include "kis_doc.h"
#include "kis_shell.h"
#include "kis_factory.h"
#include "kis_pluginserver.h"
#include <kdebug.h>


KisShell::KisShell( const char* name )
    : KoMainWindow( KisFactory::global(), name )
{
    m_pStatusBar = new KStatusBar( this, "shell_statusbar" );

    m_pStatusBar->insertItem( i18n( "a little test for the statusbar" ), 
        ID_STATUSBAR_INFOTEXT );

    statusBarLabel()->setText(i18n("Krayon Status Bar"));    
}


KisShell::~KisShell()
{
}


QString KisShell::nativeFormatName() const 
{
    return i18n("Krayon");
}



void KisShell::slotFileNew()
{
    KisDoc* doc = (KisDoc*)rootDocument();

    if( !doc )
    {
        kdDebug(0) << "KisShell::slotFileNew: no rootDocument()" << endl;
        
        doc = (KisDoc*)createDoc();
        if (!doc->initDoc())
	    {
	        delete doc;
	        return;
	    }
        setRootDocument( doc );
    }
    else
    {
        if(!doc->slotNewImage())
kdDebug(0) << "KisShell::slotFileNew() can't create new image" << endl;        
    }
}


void KisShell::slotFileNewDocument()
{
    KisDoc* doc = (KisDoc*)rootDocument();
    if(doc)
    {
        if(!doc->slotNewImage())
kdDebug(0) << "KisShell::slotFileNewDocument() can't create new image" << endl;                
    }
}


void KisShell::slotFileAddNewImage()
{
    KisDoc* doc = (KisDoc*)rootDocument();

    if(doc)
    {
        if(!doc->slotNewImage())
kdDebug(0) << "KisShell::slotFileNewDocument() can't create new image" << endl;                        
    }
}


void KisShell::slotFileRemoveCurrentImage()
{
    KisDoc* doc = (KisDoc*)rootDocument();
    
    if(doc)
    {
        doc->slotRemoveImage(doc->currentImage());
    }
}

bool KisShell::openDocument( const KURL & url )
{
    KoDocument* doc = rootDocument();

    if (!doc)
    {
	    KoDocument* newdoc = createDoc();
	    if (newdoc->openURL( url ))
	    {
	        setRootDocument( newdoc );
	        return true;
	    }
	    return false;
    }
    
    return doc->openURL( url );
}


void KisShell::slotFilePrint()
{
  // TODO
}


void KisShell::slotFileClose()
{
    KisDoc* doc = (KisDoc*)rootDocument();
    doc->slotRemoveImage( doc->currentImage() );
}


void KisShell::statusMsg( const QString& text )
{
    m_pStatusBar->changeItem( text, ID_STATUSBAR_INFOTEXT );
}

#include "kis_shell.moc"
