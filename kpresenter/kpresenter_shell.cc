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

#include "kpresenter_shell.h"
#include "kpresenter_doc.h"
#include "kpresenter_view.h"
#include "kpresenter_factory.h"
#include <kstddirs.h>

KPresenterShell::KPresenterShell( QWidget* parent, const char* name )
    : KoMainWindow( parent, name )
{
    setDoPartActivation( FALSE );
    resize( 800, 600 );
}

KPresenterShell::~KPresenterShell()
{
}

QString KPresenterShell::configFile() const
{
    return readConfigFile( locate( "data", "kpresenter/kpresenter_shell.rc",
				   KPresenterFactory::global() ) );
}

KoDocument* KPresenterShell::createDoc()
{
    return new KPresenterDoc;
}

void KPresenterShell::setRootPart( Part *part )
{
    KoMainWindow::setRootPart( part );
    ( (KPresenterView*)rootView() )->initGui();
}

void KPresenterShell::slotFilePrint()
{
    ( (KPresenterView*)rootView() )->printDlg();
}

#include "kpresenter_shell.moc"
