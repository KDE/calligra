/*
 * This file is part of Maemo 5 Office UI for Calligra
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
#ifndef Q_OS_ANDROID
#include "DBusAdaptor.h"
#endif
#include "HildonApplication.h"
#include <KoAbstractApplicationController.h>
#include <QTimer>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(CalligraMobile);

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

    // check if we are running in meego-handset, and if so set 'meegotouch' as the widget style
    if (qgetenv("X_DESKTOP_SESSION") == "X-MEEGO-HS") {
        QApplication::setStyle("meegotouch");
    }

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
    s->move(QPoint(311,122));
    if (arguments.size() > 1)
        s->show();

    a.processEvents();
    MainWindow w(s);

#ifndef Q_OS_ANDROID
    DBusAdaptor adaptor(&a);
#endif
    QObject::connect(&a, SIGNAL(openDocument(const QString &)),
                     w.controller(), SLOT(openDocument(const QString &)));
    QObject::connect(&a, SIGNAL(showApplicationMenu()),
                     w.controller(), SLOT(showApplicationMenu()));

    if (arguments.size() > 1) {
        KoAbstractApplicationOpenDocumentArguments openArgs;
        openArgs.documentsToOpen.append(arguments[1]);
        if (arguments.size() > 2) {
            openArgs.openAsTemplates = arguments[2].compare("false", Qt::CaseInsensitive);
        }
        w.controller()->openDocuments(openArgs);
    } else {
        QTimer::singleShot(5, w.controller(), SLOT(checkDBusActivation()));
    }
    if (loadScrollAndQuit) {
        QTimer::singleShot(10, w.controller(), SLOT(loadScrollAndQuit()));
    }
    return a.exec();
}
