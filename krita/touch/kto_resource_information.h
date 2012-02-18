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

#ifndef _KTO_RESOURCE_INFORMATION_H_
#define _KTO_RESOURCE_INFORMATION_H_

#include <QObject>
#include <QMetaType>

class KtoResourceInformationList;
class KoResource;
class KtoResourceInformation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString imageUrl READ imageUrl NOTIFY imageUrlChanged)
public:
    KtoResourceInformation(int _index, KtoResourceInformationList* parent = 0);
    virtual ~KtoResourceInformation();
    QString name() const;
    QString imageUrl() const;
signals:
    void nameChanged();
    void imageUrlChanged();
private:
    int m_index;
    KtoResourceInformationList* m_parent;
};

Q_DECLARE_METATYPE(QList<KtoResourceInformation*>)

#endif
