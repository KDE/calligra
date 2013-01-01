/* This file is part of the KDE project
   Copyright (C) 2012 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KexiTester.h"

#include <KDebug>

#include <QMap>
#include <QWidget>

class KexiTester::Private
{
public:
    Private() {}
    QMap<QString, QObject*> objects;
};

KexiTester::KexiTester(QObject *parent)
    : QObject(parent), d(new Private)
{
}

KexiTester::~KexiTester()
{
    delete d;
}

bool KexiTester::addObject(QObject *object, const QString &name)
{
    if (!object) {
        kWarning() << "No object provided";
        return false;
    }
    QString realName = name;
    if (realName.isEmpty()) {
        realName = object->objectName();
    }
    if (realName.isEmpty()) {
        kWarning() << "No name for object provided, won't add";
        return false;
    }
    d->objects.insert(realName, object);
    return true;
}

QObject *KexiTester::object(const QString &name) const
{
    return d->objects.value(name);
}

QWidget *KexiTester::widget(const QString &name) const
{
    QObject *o = object(name);
    return qobject_cast<QWidget*>(o);
}

#include "KexiTester.moc"
