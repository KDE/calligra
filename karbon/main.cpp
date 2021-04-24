/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001-2002 Lennart Kudling <kudling@kde.org>
   SPDX-FileCopyrightText: 2002, 2004 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2003, 2005 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2003, 2007 Rob Buis <buis@kde.org>
   SPDX-FileCopyrightText: 2003 Stephan Binner <binner@kde.org>
   SPDX-FileCopyrightText: 2004 Waldo Bastian <bastian@kde.org>
   SPDX-FileCopyrightText: 2005 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2006 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2007 Chusslove Illich <caslav.ilic@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ui/splash/hi256-app-calligrakarbon.xpm"
#include <QSplashScreen>
#include <QHideEvent>
#include <QLoggingCategory>

#include <KoApplication.h>
#include <Calligra2Migration.h>

#include <KarbonFactory.h>
#include <KarbonDocument.h>
#include <ui/KarbonAboutData.h>

#ifdef MAINTANER_WANTED_SPLASH
class KoSplashScreen : public QSplashScreen
{
public:
    explicit KoSplashScreen(const QPixmap& pixmap) : QSplashScreen(pixmap) {}

    void hideEvent(QHideEvent *event) override
    {
        event->accept();
        deleteLater();
    }
};
#endif

extern "C" Q_DECL_EXPORT int kdemain( int argc, char* argv[] )
{
    /**
     * Disable debug output by default, only log warnings.
     * Debug logs can be controlled by the environment variable QT_LOGGING_RULES.
     *
     * For example, to get full debug output, run the following:
     * QT_LOGGING_RULES="calligra.*=true" karbon
     *
     * See: http://doc.qt.io/qt-5/qloggingcategory.html
     */
    QLoggingCategory::setFilterRules("calligra.*.debug=false\n"
                                     "calligra.*.warning=true");

    KoApplication app(KARBON_MIME_TYPE, QStringLiteral("calligrakarbon"), newKarbonAboutData, argc, argv);

    // Migrate data from kde4 to kf5 locations
    Calligra2Migration m("karbon");
    m.setConfigFiles(QStringList() << QStringLiteral("karbonrc"));
    m.setUiFiles(QStringList() << QStringLiteral("karbon.rc")<< QStringLiteral("karbon_readonly.rc"));
    m.migrate();

    if (!app.start()) {  // parses command line args, create initial docs and mainwindows
        return 1;
    }

#ifdef MAINTANER_WANTED_SPLASH
    // After creating the KoApplication then create the pixmap from an xpm: we cannot get the
    // location of our datadir before we've started our components,
    // so use an xpm.
    QSplashScreen *splashScreen = new KoSplashScreen(QPixmap(splash_screen_xpm));
    splashScreen->show();
    splashScreen->showMessage("<p style=\"color:black\">"
    "<b>Calligra Karbon is unmaintained!</b><br><br>"
    "The Calligra community welcomes someone to take over.<br><br>"
    "See community.kde.org/Calligra</p>");
#endif

    return app.exec();
}
