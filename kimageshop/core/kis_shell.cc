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

#include <qmessagebox.h>

#include <kstddirs.h>
#include <kimgio.h>
#include <kglobal.h>
#include <klocale.h>
#include <kfiledialog.h>

#include <koFilterManager.h>

#include "kis_shell.h"
#include "kis_doc.h"
#include "kis_factory.h"
#include "kis_pluginserver.h"

KisShell::KisShell( const char* name )
    : KoMainWindow( name )
{
  resize(800, 600);
}

KisShell::~KisShell()
{
}

KoDocument* KisShell::createDoc()
{
    return new KisDoc;
}

void KisShell::slotFileNew()
{
  KisDoc* doc = (KisDoc*)rootDocument();

  if ( !doc )
    {
      doc = (KisDoc*)createDoc();
      if ( !doc->initDoc() )
	{
	  delete doc;
	  return;
	}
      setRootDocument( doc );
    }
    else
    {
      doc->slotNewImage();
    }
}

void KisShell::slotFileOpen()
{
  QString filter =  KImageIO::pattern( KImageIO::Reading ) + "*.kis|KImageShop picture\n";

  QString file = KFileDialog::getOpenFileName( getenv( "HOME" ), filter );

  if ( file.isNull() )
    return;

  KisDoc* doc = (KisDoc*)rootDocument();

  if (!doc->loadImage(file))
    {
      QString tmp;
      tmp.sprintf( i18n( "Could not open\n%s" ), file.data() );
      QMessageBox::critical( 0L, i18n( "IO Error" ), tmp, i18n( "OK" ) );
    }
}

void KisShell::slotFileSave()
{
  // ##### FIXME
  slotFileSaveAs();
}

void KisShell::slotFileSaveAs()
{
  QString filter =  KImageIO::pattern( KImageIO::Reading ) + "*.kis|KImageShop picture\n";

  QString file = KFileDialog::getSaveFileName( getenv( "HOME" ), filter );

  if ( file.isNull() )
    return;

  KisDoc* doc = (KisDoc*)rootDocument();

  if (!doc->saveCurrentImage( file ))
    {
      QString tmp;
      tmp.sprintf( i18n( "Could not save\n%s" ), file.data() );
      QMessageBox::critical( 0L, i18n( "IO Error" ), tmp, i18n( "OK" ) );
    }
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

#include "kis_shell.moc"
