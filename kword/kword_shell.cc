/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "kword_shell.h"
#include "kword_doc.h"
#include "kword_view.h"
#include "kword_factory.h"
#include <kstddirs.h>
#include <klocale.h>
#include <kaboutdialog.h>

KWordShell::KWordShell( const char* name )
    : KoMainWindow( KWordFactory::global(), name )
{
//    partManager()->setAllowDoubleClickActivation( false );
}

KWordShell::~KWordShell()
{
}

QString KWordShell::nativeFormatName() const
{
  return i18n("KWord");
}

void KWordShell::slotHelpAbout()
{
    KAboutDialog *dia = new KAboutDialog( KAboutDialog::AbtImageOnly | KAboutDialog::AbtProduct | KAboutDialog::AbtTitle,
					  kapp->caption(),
					  KDialogBase::Ok, KDialogBase::Ok, this, 0, TRUE );
    dia->setTitle( "KWord" );
    dia->setProduct( "", KWORD_VERSION, "Reginald Stadlbauer <reggie@kde.org>", "1998-1999" );
    dia->setImage( locate( "data", "koffice/pics/koffice-logo.png", KWordFactory::global() ) );
    dia->exec();
    delete dia;
}

#include "kword_shell.moc"
