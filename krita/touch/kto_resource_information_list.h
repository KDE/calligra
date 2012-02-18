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

#ifndef _KTO_RESOURCE_INFORMATION_LIST_H_
#define _KTO_RESOURCE_INFORMATION_LIST_H_

#include <QDeclarativeImageProvider>
#include <QObject>

class KoAbstractResourceServerAdapter;
class KoResource;
class KtoResourceInformation;

class KtoResourceInformationList : public QObject, public QDeclarativeImageProvider
{
    Q_OBJECT
    Q_PROPERTY(QList<QObject*> resourcesInformation READ resourcesInformationAsQObject NOTIFY resourcesChanged)
public:
    KtoResourceInformationList(const QString& _url_name, KoAbstractResourceServerAdapter* resourceAdapter, QObject* parent = 0);
    ~KtoResourceInformationList();
    QList<KtoResourceInformation*> resourcesInformation() const;
    QList<KoResource*> resources() const;
    QString urlName() const;
    virtual QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize);
private:
    QList<QObject*> resourcesInformationAsQObject() const;
signals:
    void resourcesChanged();
private:
    QString m_url_name;
    QList<KtoResourceInformation*> m_resourcesInformation;
    QList<KoResource*> m_resources;
    KoAbstractResourceServerAdapter* m_resourceAdapter;
};

#endif
