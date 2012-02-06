/*
 * Copyright (C) 2012 Cyrille Berger <cberger@cberger.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 sure
 * USA
 */

#include <KApplication>
#include "kto_main_window.h"
#include "ui/kis_aboutdata.h"
#include <KCmdLineArgs>
#include <qdeclarative.h>
#include "kto_canvas.h"
#include <KoPluginLoader.h>

int main(int argc, char *argv[])
{
    KAboutData * aboutData = newKritaAboutData();
    
    KCmdLineArgs::init(argc, argv, aboutData);

    KCmdLineOptions options;
    options.add("+[file(s)]", ki18n("File(s) or URL(s) to open"));
    KCmdLineArgs::addCmdLineOptions(options);
    
    KApplication app;

    // Load dockers
    KoPluginLoader::PluginsConfig config;
    config.blacklist = "QmlPluginsDisabled";
    config.group = "krita";
    KoPluginLoader::instance()->load(QString::fromLatin1("Krita/Qml"),
                                        QString::fromLatin1("[X-Krita-Version] == 5"));
    
    qmlRegisterType<KtoCanvas> ("KritaTouch", 1, 0, "Canvas");
    
    KtoMainWindow window;
    if (app.argc() > 1)
        window.openFile(app.arguments().at(1));
    window.show();

    return app.exec();
}
