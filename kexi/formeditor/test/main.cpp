/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#include "kfd_mainwindow.h"

#include <kapplication.h>
#include <kiconloader.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static const char *description =
    I18N_NOOP("KFormDesigner");

static const char *version = "0.3";

int main(int argc, char **argv)
{
    KAboutData about("kformdesigner", 0, ki18n("KFormDesigner"), version, ki18n(description),
                     KAboutData::License_LGPL, ki18n("(C) 2003-2005 Kexi Team"));
    about.addCredit( ki18n("Lucijan Busch"), ki18n("Original author"), 0, "lucijan@kde.org" );
    about.addAuthor( ki18n("Cedric Pasteur"), KLocalizedString(), "cedric.pasteur@free.fr");
    about.addCredit( ki18n("Jarosław Staniek"), ki18n("Win32 version, some icons, many fixes, ideas and bug reports"), "js@iidea.pl");
    about.addCredit( ki18n("Kristof Borrey "), ki18n("Icons"), 0, "kristof.borrey@skynet.be" );
    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("+[URL]", ki18n( "Document to open" ));
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

   KGlobal::iconLoader()->addAppDir("kexi");

    KFDMainWindow *v = new KFDMainWindow();
    if (!v->centralWidget()) { //KFD part could be not found
      delete v;
      return 1;
    }
    app.setMainWidget(v);
    v->show();



    // see if we are starting with session management
  if (app.isSessionRestored())
  {
    RESTORE(KFDMainWindow);
  }
  else
  {
  // no session.. just start up normally
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  if (args->count() >= 1)
  {
            for (int i = 0; i < args->count(); i++)
    /** @todo report loading errors here */
    v->loadUIFile(args->url(i));
        }
        args->clear();
    }

    return app.exec();
}
