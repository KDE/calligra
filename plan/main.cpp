/* This file is part of the KDE project
   Copyright (C) 2001 Thomas zander <zander@kde.org>

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

#include "kptaboutdata.h"

#include "about/hi256-app-calligraplan.xpm"
#include <ksplashscreen.h>
#include <QHideEvent>

#include <kdemacros.h>
#include <KoApplication.h>
#include <kcmdlineargs.h>

#include "kptmaindocument.h"

#ifdef MAINTANER_WANTED_SPLASH
class KoSplashScreen : public KSplashScreen
{
public:
    explicit KoSplashScreen(const QPixmap& pixmap) : KSplashScreen(pixmap) {}

    void hideEvent(QHideEvent *event)
    {
        event->accept();
        deleteLater();
    }
};
#endif


extern "C" KDE_EXPORT int kdemain( int argc, char **argv ) {
    QScopedPointer<KAboutData> about(KPlato::newAboutData());

    KCmdLineArgs::init( argc, argv, about.data());
    KCmdLineOptions options;
    options.add("+[file]", ki18n("File to open"));
    KCmdLineArgs::addCmdLineOptions( options );

    KoApplication::addCommonCommandLineOptions();
    KoApplication app(argc, argv, PLAN_MIME_TYPE, about.data());

#ifdef MAINTANER_WANTED_SPLASH
    // After creating the QApplication then create the pixmap from an xpm: we cannot get the
    // location of our datadir before we've started our components,
    // so use an xpm.
    QSplashScreen *splashScreen = new KoSplashScreen(QPixmap(splash_screen_xpm));
    splashScreen->show();
    splashScreen->showMessage("<p style=\"color:black\">"
    "<b>Calligra Plan is unmaintained!</b><br><br>"
    "The Calligra community welcomes someone to take over.<br><br>"
    "See community.kde.org/Calligra</p>");
#endif

    // This is disabled for now so the crude test below will run
    if (!app.start())
	return 1;

    app.exec();

    delete (aboutData);

    return 0;
}
