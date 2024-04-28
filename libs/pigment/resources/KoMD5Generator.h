/*
 * SPDX-FileCopyrightText: 2015 Stefano Bonicatti <smjert@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KOMD5GENERATOR_H
#define KOMD5GENERATOR_H

#include "KoHashGenerator.h"

#include <pigment_export.h>

class PIGMENTCMS_EXPORT KoMD5Generator : public KoHashGenerator
{
public:
    KoMD5Generator();
    ~KoMD5Generator() override;
    QByteArray generateHash(const QString &filename) override;
    QByteArray generateHash(const QByteArray &array) override;
};

#endif
