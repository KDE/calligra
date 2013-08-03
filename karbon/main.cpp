/* This file is part of the KDE project
   Copyright (C) 2001-2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2002,2004 David Faure <faure@kde.org>
   Copyright (C) 2003,2005 Laurent Montel <montel@kde.org>
   Copyright (C) 2003,2007 Rob Buis <buis@kde.org>
   Copyright (C) 2003 Stephan Binner <binner@kde.org>
   Copyright (C) 2004 Waldo Bastian <bastian@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>
   Copyright (C) 2006 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2006 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2007 Chusslove Illich <caslav.ilic@gmx.net>

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

#include "ui/splash/hi256-app-calligrakarbon.xpm"
#include <ksplashscreen.h>
#include <QHideEvent>

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <KoApplication.h>

#include <KarbonFactory.h>

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

extern "C" KDE_EXPORT int kdemain( int argc, char* argv[] )
{
    QScopedPointer<KAboutData> about(KarbonFactory::aboutData());
    KCmdLineArgs::init( argc, argv, about.data() );

    KCmdLineOptions options;
    options.add("+[file]", ki18n( "File to open" ));
    KCmdLineArgs::addCmdLineOptions( options );
    KoApplication::addCommonCommandLineOptions();
    KoApplication app(argc, argv, KarbonFactory::aboutData());

#ifdef MAINTANER_WANTED_SPLASH
    // After creating the QApplication then create the pixmap from an xpm: we cannot get the
    // location of our datadir before we've started our components,
    // so use an xpm.
    QSplashScreen *splashScreen = new KoSplashScreen(QPixmap(splash_screen_xpm));
    splashScreen->show();
    splashScreen->showMessage("<p style=\"color:black\">"
    "<b>Calligra Karbon is unmaintained!</b><br><br>"
    "The Calligra community welcomes someone to take over.<br><br>"
    "See community.kde.org/Calligra</p>");
#endif

    if( !app.start() )  // parses command line args, create initial docs and shells
        return 1;

    return app.exec();
}
