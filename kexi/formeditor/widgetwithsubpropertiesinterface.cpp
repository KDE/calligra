/* This file is part of the KDE project
   Copyright (C) 2006 Jarosław Staniek <staniek@kde.org>

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

#include "widgetwithsubpropertiesinterface.h"

#include <QMetaObject>

#include <kexiutils/utils.h>
#include <kdebug.h>

using namespace KFormDesigner;

class WidgetWithSubpropertiesInterface::Private
{
public:
    Private()
    {

    }

    ~Private()
    {

    }

    QPointer<QWidget> subwidget;
    QSet<QByteArray> subproperies;
};

WidgetWithSubpropertiesInterface::WidgetWithSubpropertiesInterface() : d(new Private())
{
}

WidgetWithSubpropertiesInterface::~WidgetWithSubpropertiesInterface()
{
    delete d;
}

void WidgetWithSubpropertiesInterface::setSubwidget(QWidget *widget)
{
    d->subwidget = widget;
    d->subproperies.clear();
    QSet<QByteArray> addedSubproperies;
    if (d->subwidget) {
        //remember properties in the subwidget that are not present in the parent
        for (const QMetaObject *metaObject = d->subwidget->metaObject(); metaObject;
                metaObject = metaObject->superClass()) {
            QList<QMetaProperty> properties(
                KexiUtils::propertiesForMetaObjectWithInherited(metaObject));
            foreach(const QMetaProperty &property, properties) {
                if (dynamic_cast<QObject*>(this)
                    && -1 != dynamic_cast<QObject*>(this)->metaObject()->indexOfProperty(property.name())
                    && !addedSubproperies.contains(property.name()))
                {
                    d->subproperies.insert(property.name());
                    addedSubproperies.insert(property.name());
                    kDebug() << "added subwidget's property that is not present in the parent: "
                        << property.name();
                }
            }
        }
    }
}

QWidget* WidgetWithSubpropertiesInterface::subwidget() const
{
    return d->subwidget;
}

QSet<QByteArray> WidgetWithSubpropertiesInterface::subproperies() const
{
    return d->subproperies;
}

QMetaProperty WidgetWithSubpropertiesInterface::findMetaSubproperty(const char * name) const
{
    if (!d->subwidget || d->subproperies.contains(name))
        return QMetaProperty();
    return KexiUtils::findPropertyWithSuperclasses(d->subwidget, name);
}

QVariant WidgetWithSubpropertiesInterface::subproperty(const char * name, bool &ok) const
{
    if (!d->subwidget || d->subproperies.contains(name)) {
        ok = false;
        return QVariant();
    }
    ok = true;
    return d->subwidget->property(name);
}

bool WidgetWithSubpropertiesInterface::setSubproperty(const char * name, const QVariant & value)
{
    if (!d->subwidget || d->subproperies.contains(name))
        return false;
    return d->subwidget->setProperty(name, value);
}
