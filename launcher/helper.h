// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <qqmlregistration.h>

class Helper : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit Helper(QObject *parent = nullptr);

    Q_INVOKABLE void execute(const QString &appId);
};
