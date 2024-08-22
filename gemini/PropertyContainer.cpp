/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "PropertyContainer.h"

PropertyContainer::PropertyContainer(QString name, QObject *parent)
    : QObject(parent)
    , m_name(name)
{
}

PropertyContainer::~PropertyContainer() = default;

void PropertyContainer::writeProperty(QString name, QVariant value)
{
    setProperty(name.toLatin1(), value);
}

QVariant PropertyContainer::readProperty(QString name)
{
    return property(name.toLatin1());
}

QString PropertyContainer::name()
{
    return m_name;
}
