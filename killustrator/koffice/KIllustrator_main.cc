/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by  
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "KIllustrator_main.h"
#include "KIllustrator_main.moc"
#include "KIllustrator.h"
#include "KIllustrator_shell.h"

#include <koScanParts.h>
#include <factory_impl.h>

bool withGUI = true;

FACTORY(KIllustratorDocument, KIllustrator::Factory_skel, KIllustratorFactory)

typedef AutoLoader<KIllustratorFactory> KIllustratorAutoLoader;

KIllustratorOPApp::KIllustratorOPApp (int argc, char** argv) :
    OPApplication (argc, argv, "killustrator") 
{
    getLocale()->insertCatalogue("koffice");
}

KIllustratorOPApp::~KIllustratorOPApp () {
}

void KIllustratorOPApp::start () {
  koScanParts ();

  if (withGUI) {
    KIllustratorShell* m_pShell;

    m_pShell = new KIllustratorShell;
    m_pShell->enableMenuBar ();
    m_pShell->PartShell_impl::enableStatusBar ();
    m_pShell->enableToolBars ();

    m_pShell->show ();
  }
}

int main (int argc, char** argv) {
  for (int i = 1; i < argc; i++) {
    if (::strcmp (argv[i], "-s") == 0 || 
	::strcmp (argv[i], "--server") == 0)
      withGUI = false;
  }

  KIllustratorAutoLoader loader ("IDL:KIllustrator/Factory:1.0");
  KIllustratorOPApp app (argc, argv);
  app.exec ();

  return 0;
}
