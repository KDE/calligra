/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2013  Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "SharingManager.h"
#include <kis_view2.h>
#include <kis_part2.h>
#include <kparts/plugin.h>
#include <QDeclarativeEngine>
#include <QNetworkAccessManager>
#include <QNetworkCookieJar>

class SharingManager::Private {
public:
    Private()
        : view(0)
    {};
    KisView2* view;
};

SharingManager::SharingManager(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
}

SharingManager::~SharingManager()
{
    delete d;
}

QObject* SharingManager::sharingHandler(QString name)
{
    if(!d->view)
        return 0;
    foreach(KXMLGUIClient* child, d->view->childClients()) {
        KParts::Plugin* plugin = dynamic_cast<KParts::Plugin*>(child);
        if(plugin && plugin->metaObject()->className() == name) {
            return plugin;
        }
    }
    return 0;
}

void SharingManager::setView(QObject* newView)
{
    if(d->view) {
        foreach(KXMLGUIClient* child, d->view->childClients()) {
            KParts::Plugin* plugin = dynamic_cast<KParts::Plugin*>(child);
            if(plugin) {
                plugin->disconnect(this);
            }
        }
    }
    d->view = qobject_cast<KisView2*>(newView);
    if(d->view) {
        foreach(KXMLGUIClient* child, d->view->childClients()) {
            KParts::Plugin* plugin = dynamic_cast<KParts::Plugin*>(child);
            if(plugin) {
                if(plugin->metaObject()->indexOfSignal("sharingSuccessful(QString,QString)") > -1) {
                    connect(plugin, SIGNAL(sharingSuccessful(QString,QString)), SIGNAL(sharingSuccessful(QString,QString)));
                }
            }
        }
    }
    emit viewChanged();
}

QObject* SharingManager::view() const
{
    return d->view;
}

void SharingManager::clearCookies(QObject* qmlEngine)
{
    QDeclarativeEngine* engine = qobject_cast<QDeclarativeEngine*>(qmlEngine);
    engine->networkAccessManager()->setCookieJar(new QNetworkCookieJar());
}

#include "SharingManager.moc"
