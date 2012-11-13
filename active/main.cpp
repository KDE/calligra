/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
 * Copyright (C) 2012 Sujith Haridasan <sujith.haridasan@kdemail.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 sure
 * USA
 */

#include "src/MainWindow.h"
#include <KDE/KApplication>
#include <KDE/KAboutData>
#include <KDE/KCmdLineArgs>

int main(int argc, char *argv[])
{
    KAboutData aboutData("calligraactive",
                         0,
                         ki18n("Calligra Active"),
                         "1.0 Beta",
                         ki18n("Calligra application for tablets"),
                         KAboutData::License_GPL_V2,
                         ki18n("Copyright (c) 2011-2012"));
    aboutData.addAuthor(ki18n("Shantanu Tushar"),
                        ki18n("Maintainer and main developer"),
                        "shantanu@kde.org",
                        "http://www.shantanutushar.com");
    aboutData.addAuthor(ki18n("Sujith Haridasan"),
                        ki18n("Maintainer and main developer"),
                        "sujith.haridasan@kdemail.net",
                        "http://www.sujithh.info");
    KCmdLineArgs::init(argc, argv, &aboutData);
    KCmdLineOptions options;
    options.add("+[file]", ki18n("File to open"));
    KCmdLineArgs::addCmdLineOptions(options);


    KApplication app;
    MainWindow mw;

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if(args->count()) {
       mw.openFile(args->arg(0));
    }
    args->clear();

    mw.show();

    return app.exec();
}
