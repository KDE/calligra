/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef PROPERTYCONTAINER_H
#define PROPERTYCONTAINER_H

#include <QObject>
#include <QVariant>

/**
 * The only purpose of this class is to expose the dynamic property
 * system of Qt to QML, so we can set and get properties on a generic
 * object. It is a little bit of a hack, but QML deliberately does
 * not have access to this (according to the developers).
 */
class PropertyContainer : public QObject
{
    Q_OBJECT
public:
    explicit PropertyContainer(QString name, QObject *parent = nullptr);
    ~PropertyContainer() override;

    // As QObject already as setProperty and property() functions, we must
    // name ours differently
    Q_INVOKABLE void writeProperty(QString name, QVariant value);
    Q_INVOKABLE QVariant readProperty(QString name);

    Q_INVOKABLE QString name();

private:
    QString m_name;
};

#endif // PROPERTYCONTAINER_H
