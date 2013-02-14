/*
 *  Copyright (c) 2013 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "kis_application.h"

#include <KCmdLineArgs>
#include "KoGlobal.h"

#if KDE_IS_VERSION(4,6,0)
#include <krecentdirs.h>
#endif

#include <QFile>

#include <QDesktopServices>
#include <QProcessEnvironment>
#include <QDir>

#include <stdlib.h>

KisApplication::KisApplication()
    : KApplication()
{
    // Tell the iconloader about share/apps/calligra/icons
    KIconLoader::global()->addAppDir("calligra");

    // Initialize all Calligra directories etc.
    KoGlobal::initialize();

    m_starting = true;
#ifdef Q_WS_WIN
    setStyle("Plastique"); // the only style that works well on Windows with dark themes

#endif

}

void KisApplication::createCommandLineOptions()
{
    KCmdLineOptions options;
    options.add(CmdLineOptionName("print"), ki18n("Only print and exit"));
    options.add(CmdLineOptionName("template"), ki18n("Open a new document with a template"));
    options.add(CmdLineOptionName("dpi <dpiX,dpiY>"), ki18n("Override display DPI"));
    options.add(CmdLineOptionName("export-pdf"), ki18n("Only export to PDF and exit"));
    options.add(CmdLineOptionName("export-filename <filename>"), ki18n("Filename for export-pdf"));
    options.add(CmdLineOptionName("benchmark-loading"), ki18n("just load the file and then exit"));
    options.add(CmdLineOptionName("benchmark-loading-show-window"), ki18n("load the file, show the window and progressbar and then exit"));
    options.add(CmdLineOptionName("profile-filename <filename>"), ki18n("Filename to write profiling information into."));
    options.add(CmdLineOptionName("roundtrip-filename <filename>"), ki18n("Load a file and save it as an ODF file. Meant for debugging."));
    options.add(CmdLineOptionName("+[URL(s)]"), ki18n("File(s) to load"));

    KCmdLineArgs::init( argc, argv, &mAboutData );
    KCmdLineArgs::addCmdLineOptions( programOptions );
}


KisApplication::~KisApplication()
{
}

bool KisApplication::notify(QObject *receiver, QEvent *event)
{
    try {
        return QApplication::notify(receiver, event);
    } catch (std::exception &e) {
        qWarning("Error %s sending event %i to object %s",
                 e.what(), event->type(), qPrintable(receiver->objectName()));
    } catch (...) {
        qWarning("Error <unknown> sending event %i to object %s",
                 event->type(), qPrintable(receiver->objectName()));
    }
    return false;

}
