/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qmessagebox.h>

#include <kimageio.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kfiledialog.h>
#include <kio/netaccess.h>

#include <koFilterManager.h>
#include <koApplication.h>

#include "kimage_shell.h"
#include "kimage_factory.h"
#include "kimage_doc.h"
#include "kimage_view.h"

KImageShell::KImageShell( const char* name )
  : KoMainWindow( KImageFactory::global(), name )
{
  resize( 640, 480 );
}

KImageShell::~KImageShell()
{
}

KoDocument* KImageShell::createDoc()
{
  return new KImageDocument;
}

void KImageShell::slotFileOpen()
{
  KURL url = KFileDialog::getOpenURL( getenv( "HOME" ), KImageIO::pattern( KImageIO::Reading ) );

  // TODO: use file preview dialog
  //KURL url = KFilePreviewDialog::getOpenURL( getenv( "HOME" ), KImageIO::pattern( KImageIO::Reading ), 0 );

  if ( url.isEmpty() )
    return;

  QString tempFile;

  KIO::NetAccess::download( url, tempFile );

  if( !KImageIO::isSupported( KImageIO::mimeType( tempFile ) ) )
  {
    if( !openDocument( tempFile ) )
    {
      QString tmp;
      tmp.sprintf( i18n( "Could not open\n%s" ), url.path().data() );
      QMessageBox::critical( 0L, i18n( "IO Error" ), tmp, i18n( "OK" ) );
    }

    KIO::NetAccess::removeTempFile( tempFile );
  }
}

#include "kimage_shell.moc"
