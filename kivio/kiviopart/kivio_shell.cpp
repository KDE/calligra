/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "kivio_shell.h"
#include "kivio_doc.h"
#include "kivio_view.h"
#include "kivio_factory.h"

#include <klocale.h>
#include <kaboutdialog.h>
#include <kapp.h>
#include <kstddirs.h>
#include <kiconloader.h>

KivioShell::KivioShell(KInstance* instance, const char* name)
: KoMainWindow(instance,name)
{
  setInstance( KivioFactory::global(), false );
}

KivioShell::~KivioShell()
{
}

QString KivioShell::nativeFormatName() const
{
  return i18n("theKompany.com - Kivio");
}

void KivioShell::slotHelpAbout()
{
    KAboutDialog *dia = new KAboutDialog( KAboutDialog::AbtProduct | KAboutDialog::AbtTitle | KAboutDialog::AbtImageOnly,
                                          kapp->caption(),
                                          KDialogBase::Ok, KDialogBase::Ok, this, 0, true );
    dia->setTitle( kapp->caption() );
    dia->setProduct( "Kivio", "TPR", "theKompany.com Team", "1999-2000" );
    dia->setImage( locate( "data", "koffice/pics/koffice-logo.png" ) );
    dia->exec();
    delete dia;
}
