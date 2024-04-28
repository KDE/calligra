/*
 * SPDX-FileCopyrightText: 2015 Stefano Bonicatti <smjert@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KOHASHGENERATORPROVIDER_H
#define KOHASHGENERATORPROVIDER_H

#include <QHash>
#include <QMutex>

#include <pigment_export.h>

class KoHashGenerator;

class PIGMENTCMS_EXPORT KoHashGeneratorProvider
{
public:
    KoHashGeneratorProvider();
    ~KoHashGeneratorProvider();

    KoHashGenerator *getGenerator(const QString &algorithm);
    void setGenerator(const QString &algorithm, KoHashGenerator *generator);
    static KoHashGeneratorProvider *instance();

private:
    static KoHashGeneratorProvider *instance_var;
    QHash<QString, KoHashGenerator *> hashGenerators;
    QMutex mutex;
};

#endif
