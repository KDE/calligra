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

#include "kpresenter_shell.h"
#include "kpresenter_doc.h"
#include "kpresenter_view.h"
#include "kpresenter_factory.h"
#include <kstddirs.h>
#include <klocale.h>
#include <kaboutdialog.h>
#include <kapp.h>

KPresenterShell::KPresenterShell( const char* name )
    : KoMainWindow( KPresenterFactory::global(), name )
{
// partManager()->setAllowDoubleClickActivation( false );
}

KPresenterShell::~KPresenterShell()
{
}

QString KPresenterShell::nativeFormatName() const
{
  return i18n("KPresenter");
}

void KPresenterShell::slotHelpAbout()
{
    KAboutDialog *dia = new KAboutDialog( KAboutDialog::AbtImageOnly | KAboutDialog::AbtProduct | KAboutDialog::AbtTitle,
					  kapp->caption(),
					  KDialogBase::Ok, KDialogBase::Ok, this, 0, true );
    dia->setTitle( "KPresenter" );
    dia->setProduct( "", KPRESENTER_VERSION, "Reginald Stadlbauer <reggie@kde.org>", "1998-1999" );
    dia->setImage( locate( "data", "koffice/pics/koffice-logo.png", KPresenterFactory::global() ) );
    dia->exec();
    delete dia;
}

#include "kpresenter_shell.moc"
