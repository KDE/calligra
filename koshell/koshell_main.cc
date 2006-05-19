/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2000, 2001 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <kofficeversion.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <klocale.h>
#include <kuniqueapplication.h>
#include <kwin.h>

#include <KoGlobal.h>

#include "koshell_shell.h"

static const char* description = I18N_NOOP("KOffice Workspace");
static const char* version = KOFFICE_VERSION_STRING;

class KoShellApp : public KUniqueApplication {
  public:
    KoShellApp() : mMainWindow( 0 ) {}
    ~KoShellApp() {}

    int newInstance();

  private:
    KoShellWindow *mMainWindow;
};


int KoShellApp::newInstance()
{
  if ( QApplication::isSessionRestored() ) {
    // There can only be one main window
    if ( KMainWindow::canBeRestored( 1 ) ) {
      mMainWindow = new KoShellWindow();
//      setMainWidget( mMainWindow );
      mMainWindow->show();
      mMainWindow->restore( 1 );
    }
  } else {
    if ( !mMainWindow ) {
      mMainWindow = new KoShellWindow();
      mMainWindow->show();
//      setMainWidget( mMainWindow );
    }
  }
  // Handle startup notification and window activation
  // (The first time it will do nothing except note that it was called)
  return KUniqueApplication::newInstance();
}

extern "C" KOSHELL_EXPORT int kdemain( int argc, char **argv )
{
  KAboutData * aboutData = new KAboutData( "koshell", I18N_NOOP("KOffice Workspace"),
     version, description, KAboutData::License_GPL,
       "(c) 1998-2006, Torben Weis\n(c) 2002-2005, David Faure\n(c) 2005, Sven Lüppken");
  aboutData->addAuthor("Sven Lüppken", I18N_NOOP("Current Maintainer"), "sven@kde.org");
  aboutData->addAuthor("Torben Weis", 0, "weis@kde.org");
  aboutData->addAuthor("David Faure", 0, "faure@kde.org");
  KCmdLineArgs::init( argc, argv, aboutData );

  if ( !KoShellApp::start() ) {
    // Already running, brought to the foreground.
    return 0;
  }
  KoShellApp app;
  // Initialize stuff (like KoApplication does)
  KoGlobal::initialize();

  return app.exec();
}

