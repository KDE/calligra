/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2012       Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>

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

#include <KoApplication.h>
#include <kcmdlineargs.h>
#include "CAuAboutData.h"
#include "KWDocument.h"
extern "C" KDE_EXPORT int kdemain(int argc, char **argv)
{
    QScopedPointer<KAboutData> aboutData(newAuthorAboutData());

#if HAVE_X11
    // the "raster" graphicssystem is way faster then the "native" graphicssystem on x11 with Calligra Author
    qApp->setGraphicsSystem( QLatin1String("raster") );
#endif

    KCmdLineArgs::init(argc, argv, aboutData.data());

    KCmdLineOptions options;
    options.add("+[file]", ki18n("File to open"));
    KCmdLineArgs::addCmdLineOptions(options);

    KoApplication::addCommonCommandLineOptions();
    KoApplication app(KCmdLineArgs::qtArgc(), KCmdLineArgs::qtArgv(), WORDS_MIME_TYPE, aboutData.data());

    if (!app.start())
        return 1;

    return app.exec();
}
