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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <sys/param.h>
#endif

#ifdef __FreeBSD__
#include <floatingpoint.h>
#endif

#include <kapp.h>
#include "version.h"
#if NEWKDE
#include <kio/job.h>
#include <kstddirs.h>
#endif
#include "KIllustrator.h"
#include "PStateManager.h"
#include "StartupScreen.h"
#include "filter_utils.h"

#ifndef KDEMAXPATHLEN
#define KDEMAXPATHLEN 4095
#endif

#include <unistd.h>

int main (int argc, char** argv) {
#ifdef __FreeBSD__
  fpsetmask (fpgetmask() & ~(FP_X_DZ|FP_X_INV));
#endif
  int option_index = 0;
  bool show_gui = true;

  KApplication* app = new KApplication (argc, argv, APP_NAME);

  QObject::connect (app, SIGNAL(saveYourself ()),
		    PStateManager::instance (), SLOT(saveDefaultSettings ()));

  if (argc != 1) {
    opterr = 0;
    while (1) {
      int c = getopt (argc, argv, "hve::");
      if (c == -1)
	break;
      switch (c) {
      case 'h':
	cout << "KIllustrator Release " << APP_VERSION << '\n'
	     << "Options:\n" 
	     << " -h             Print this message\n"
	     << " -v             Print version number\n"
	     << " -e fmt files   Convert KIllustrator files into "
	     << "output format fmt\n"
	     << " -e             List supported output formats"
	     << endl;
	show_gui = false;
	break;
      case 'v':
	cout << APP_VERSION << endl;
	show_gui = false;
	break;
      case 'e':
	if (optind == argc)
	  list_export_filters ();
	else
	  convert_to_format (argv[optind], &argv[optind + 1], 
			     argc - optind - 1);
	show_gui = false;
      default:
	break;
      }
    }
  }

  if (!show_gui)
    exit (0);

  if (PStateManager::instance ()->showSplashScreen ())
#ifdef NEWKDE
    new StartupScreen( KGlobal::dirs()->findResource( "appdata",
                "pics/killustrator-intro.gif" ), 5);
#else
    new StartupScreen (kapp->kde_datadir() + 
		       "/killustrator/pics/killustrator-intro.gif", 5);
#endif
  if (app->isRestored ()) {
    int n = 1;
    while (KTMainWindow::canBeRestored (n)) {
      KTMainWindow* toplevel = new KIllustrator ();
      toplevel->restore (n++);
    }
  }
  else {
    if (argc > 1) {
      for (int i = 1; i < argc; i++) {
	QString arg = argv[i];
	if (arg.find (":/") == -1) {
	  if (arg.left (1) != "/") {
	    char buf[KDEMAXPATHLEN];
	    getcwd (buf, KDEMAXPATHLEN);
	    arg.sprintf ("file:%s/%s", buf, argv[i]);
	  }
	  else
	    arg.sprintf ("file:%s", argv[i]);
	}
	KTMainWindow* toplevel = new KIllustrator ((const char *) arg);
	toplevel->show ();
      }
    }
    else {
      KTMainWindow* toplevel = new KIllustrator ();	
      toplevel->show ();
    }
  }
  int retval = app->exec ();

#ifdef __FreeBSD__
  fpresetsticky (FP_X_DZ|FP_X_INV);
  fpsetmask (FP_X_DZ|FP_X_INV);
#endif

  return retval;
}
