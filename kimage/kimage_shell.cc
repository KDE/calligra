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

#include <klocale.h>
#include <kfiledialog.h>
#include <kapp.h>
#include <klocale.h>
#include <kstddirs.h>

#include <koAboutDia.h>

#include "kimage_shell.h"
#include "kimage_factory.h"
#include "kimage_doc.h"
#include "kimage_view.h"

KImageShell::KImageShell( QWidget* parent, const char* name )
  : KoMainWindow( parent, name )
{
  setDoPartActivation( FALSE );
  resize( 640, 480 );
}

KImageShell::~KImageShell()
{
}

QString KImageShell::configFile() const
{
  return readConfigFile( locate( "kim", "kimage_shell.rc", KImageFactory::global() ) );
}

KoDocument* KImageShell::createDoc()
{
  return new KImageDocument;
}

#include "kimage_shell.moc"
