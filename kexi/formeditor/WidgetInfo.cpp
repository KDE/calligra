/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2009 Jarosław Staniek <staniek@kde.org>

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

#include "WidgetInfo.h"
#include "widgetfactory.h"

#include <kdebug.h>
#include <koproperty/Property.h>
#include <koproperty/Set.h>

namespace KFormDesigner {
class WidgetInfo::Private
{
public:
    Private (WidgetFactory *f)
        : inheritedClass(0)
        , overriddenAlternateNames(0)
        , factory(f)
        , propertiesWithDisabledAutoSync(0)
        , customTypesForProperty(0)
    {
    }

    ~Private() {
        delete overriddenAlternateNames;
        delete propertiesWithDisabledAutoSync;
        delete customTypesForProperty;
    }

    QString pixmap;
    QByteArray className;
    QString name;
    QString prefixName;
    QString desc;
    QString include;
    QList<QByteArray> alternateNames;
    QList<QByteArray> *overriddenAlternateNames;
    QList<QByteArray> autoSaveProperties;
    QByteArray saveName;
    QPointer<WidgetFactory> factory;
    QHash<QByteArray, tristate> *propertiesWithDisabledAutoSync;
    QHash<QByteArray, int> *customTypesForProperty;
    QByteArray parentFactoryName;
    QByteArray inheritedClassName; //!< Used for inheriting widgets between factories
    WidgetInfo* inheritedClass;

};
}

//--------------------------------------

using namespace KFormDesigner;

WidgetInfo::WidgetInfo(WidgetFactory *f)
 : d(new Private(f) )
{
}

/*2.0
WidgetInfo::WidgetInfo(WidgetFactory *f, const char* parentFactoryName,
                       const QString& inheritedClassName)
        : m_parentFactoryName(QByteArray("kformdesigner_") + parentFactoryName)
        , m_inheritedClassName(inheritedClassName)
        , m_inheritedClass(0)
        , m_overriddenAlternateNames(0)
        , m_factory(f)
        , m_propertiesWithDisabledAutoSync(0)
        , m_customTypesForProperty(0)
{
}*/

WidgetInfo::~WidgetInfo()
{
    delete d;
}

QString WidgetInfo::pixmap() const
{
    return d->pixmap;
}

void WidgetInfo::setPixmap(const QString &p)
{
    d->pixmap = p;
}

QByteArray WidgetInfo::className() const
{
    return d->className;
}

void WidgetInfo::setClassName(const QByteArray &className)
{
    d->className = className;
}

QByteArray WidgetInfo::inheritedClassName() const
{
    return d->inheritedClassName;
}

void WidgetInfo::setInheritedClassName(const QByteArray& inheritedClassName)
{
    d->inheritedClassName = inheritedClassName;
    if (d->className.isEmpty())
        d->className = inheritedClassName; // the default
}

void WidgetInfo::setInheritedClass(WidgetInfo *inheritedClass)
{
    d->inheritedClass = inheritedClass;
}

WidgetInfo* WidgetInfo::inheritedClass() const
{
    return d->inheritedClass;
}

QString WidgetInfo::namePrefix() const
{
    return d->prefixName;
}

void WidgetInfo::setNamePrefix(const QString &n)
{
    d->prefixName = n;
}

QString WidgetInfo::name() const
{
    return d->name;
}

void WidgetInfo::setName(const QString &n)
{
    d->name = n;
}

QString WidgetInfo::description() const
{
    return d->desc;
}


void WidgetInfo::setDescription(const QString &desc)
{
    d->desc = desc;
}

QString WidgetInfo::includeFileName() const
{
    return d->include;
}

void WidgetInfo::setIncludeFileName(const QString &name)
{
    d->include = name;
}

QList<QByteArray> WidgetInfo::alternateClassNames() const
{
    return d->alternateNames;
}

QByteArray WidgetInfo::savingName() const
{
    return d->saveName;
}

WidgetFactory* WidgetInfo::factory() const
{
    return d->factory;
}

void WidgetInfo::setSavingName(const QByteArray& saveName)
{
    d->saveName = saveName;
}

QByteArray WidgetInfo::parentFactoryName() const
{
    return d->parentFactoryName;
}

void WidgetInfo::setParentFactoryName(const QByteArray& parentFactoryName)
{
    d->parentFactoryName = QByteArray("kformdesigner_") + parentFactoryName;
}

void WidgetInfo::addAlternateClassName(const QByteArray& alternateName, bool override)
{
    d->alternateNames += alternateName;
    if (override) {
        if (!d->overriddenAlternateNames)
            d->overriddenAlternateNames = new QList<QByteArray>;
        d->overriddenAlternateNames->append(alternateName);
    } else {
        if (d->overriddenAlternateNames)
            d->overriddenAlternateNames->removeAll(alternateName);
    }
}

bool WidgetInfo::isOverriddenClassName(const QByteArray& alternateName) const
{
    return d->overriddenAlternateNames && d->overriddenAlternateNames->contains(alternateName);
}

void WidgetInfo::setAutoSyncForProperty(const char *propertyName, tristate flag)
{
    if (!d->propertiesWithDisabledAutoSync) {
        if (~flag)
            return;
        d->propertiesWithDisabledAutoSync = new QHash<QByteArray, tristate>;
    }

    if (~flag) {
        d->propertiesWithDisabledAutoSync->remove(propertyName);
    } else {
        d->propertiesWithDisabledAutoSync->insert(propertyName, flag);
    }
}

tristate WidgetInfo::autoSyncForProperty(const char *propertyName) const
{
    if (!d->propertiesWithDisabledAutoSync)
        return cancelled;
    tristate flag = d->propertiesWithDisabledAutoSync->value(propertyName);
    return flag;
}

void WidgetInfo::setAutoSaveProperties(const QList<QByteArray>& properties)
{
    d->autoSaveProperties = properties;
}

QList<QByteArray> WidgetInfo::autoSaveProperties() const
{
    if (!d->inheritedClass)
        return d->autoSaveProperties;
    return d->inheritedClass->autoSaveProperties() + d->autoSaveProperties;
}

void WidgetInfo::setCustomTypeForProperty(const char *propertyName, int type)
{
    if (!propertyName || type == (int)KoProperty::Auto)
        return;
    if (!d->customTypesForProperty) {
        d->customTypesForProperty = new QHash<QByteArray, int>();
    }
    d->customTypesForProperty->remove(propertyName);
    d->customTypesForProperty->insert(propertyName, type);
}

int WidgetInfo::customTypeForProperty(const char *propertyName) const
{
    if (!d->customTypesForProperty || !d->customTypesForProperty->contains(propertyName))
        return KoProperty::Auto;
    return d->customTypesForProperty->value(propertyName);
}

QVariant WidgetInfo::internalProperty(const QByteArray& property) const
{
    return d->factory->internalProperty(d->className, property);
}

void WidgetInfo::setInternalProperty(const QByteArray& property, const QVariant& value)
{
    InternalPropertyHandlerInterface *iface = static_cast<InternalPropertyHandlerInterface*>(d->factory);
    iface->setInternalProperty(d->className, property, value);
}
