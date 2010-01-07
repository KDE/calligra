/*
 * This file is part of Maemo 5 Office UI for KOffice
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Manikandaprasad N C <manikandaprasad.chandrasekar@nokia.com>
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
 * 02110-1301 USA
 *
 */

#include "MainWindow.h"
#include "Splash.h"
#include "DBusAdaptor.h"
#include "HildonApplication.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(FreOffice);

    // parse and set the value for graphicssystem
    int graphicsSystemIndex = -1;
    for (int i = 1; i < argc; ++i) {
        QString arg(argv[i]);
        if (arg.startsWith("-graphicssystem=")) {
            graphicsSystemIndex = i;
            QApplication::setGraphicsSystem(arg.mid(16));
        }
    }

    HildonApplication a(argc, argv);
    QStringList arguments = a.arguments();
    // remove graphicssystem attribute from the attribute list
    if (graphicsSystemIndex != -1) {
        arguments.removeAt(graphicsSystemIndex);
    }
    bool loadScrollAndQuit = false;
    if (arguments.contains("-loadScrollAndQuit")) {
        loadScrollAndQuit = true;
        arguments.removeAll("-loadScrollAndQuit");
    }

    Splash *s = new Splash();
    if (arguments.size() > 1)
        s->show();

    a.processEvents();
    MainWindow w(s);

    DBusAdaptor adaptor(&a);
    QObject::connect(&a, SIGNAL(openDocument(const QString &)),
                     &w, SLOT(openDocument(const QString &)));
    QObject::connect(&a, SIGNAL(showApplicationMenu()),
                     &w, SLOT(showApplicationMenu()));

    if (arguments.size() > 1) {
        w.openDocument(arguments[1]);
    } else {
        QTimer::singleShot(5, &w, SLOT(checkDBusActivation()));
    }
    if (loadScrollAndQuit) {
        QTimer::singleShot(10, &w, SLOT(loadScrollAndQuit()));
    }
    return a.exec();
}
