/*
 * SPDX-FileCopyrightText: 2015 Stefano Bonicatti <smjert@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KOHASHGENERATOR_H
#define KOHASHGENERATOR_H

#include <QByteArray>
#include <QString>

class KoHashGenerator
{
public:
    virtual QByteArray generateHash(const QString &filename) = 0;
    virtual QByteArray generateHash(const QByteArray &array) = 0;
    virtual ~KoHashGenerator() = default;
};
#endif
