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

KImageShopShell::KImageShopShell( QWidget* parent, const char* name )
    : KoMainWindow( parent, name )
{
  resize(800, 600);
}

KImageShopShell::~KImageShopShell()
{
}

QString KImageShopShell::configFile() const
{
    return readConfigFile( locate("kis", "kimageshop_shell.rc", KImageShopFactory::global()));
}

KoDocument* KImageShopShell::createDoc()
{
    return new KImageShopDoc;
}

void KImageShopShell::slotFileNew()
{
  KImageShopDoc* doc = (KImageShopDoc*)document();

  if ( !doc )
    {
      doc = (KImageShopDoc*)createDoc();
      if ( !doc->initDoc() )
	{
	  delete doc;
	  return;
	}
      setRootPart( doc );
    }
    else
    {
      doc->slotNewImage();
    }
}

void KImageShopShell::slotFileOpen()
{
  QString filter = "*.kis|KImageShop picture\n" + KImageIO::pattern( KImageIO::Reading );

  QString file = KFileDialog::getOpenFileName( getenv( "HOME" ), filter );
  
  if ( file.isNull() )
    return;
  
  if( KImageIO::isSupported( KImageIO::mimeType( file ) ) )
    {
      QString tempfile = KoFilterManager::self()->import( file, nativeFormatMimeType() );
      if( tempfile.isNull() )
	return;
    }
  
  if( !openDocument( KURL( file ), tempfile != file ) )
    {
      QString tmp;
      tmp.sprintf( i18n( "Could not open\n%s" ), file.data() );
      QMessageBox::critical( 0L, i18n( "IO Error" ), tmp, i18n( "OK" ) );
    }
}

#include "kis_shell.moc"
