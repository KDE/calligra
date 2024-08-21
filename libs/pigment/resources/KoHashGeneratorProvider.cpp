/*
 * SPDX-FileCopyrightText: 2015 Stefano Bonicatti <smjert@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#include "KoHashGeneratorProvider.h"

#include <QGlobalStatic>
#include <QMutexLocker>

#include "KoMD5Generator.h"

KoHashGeneratorProvider *KoHashGeneratorProvider::instance_var = nullptr;
Q_GLOBAL_STATIC(KoHashGeneratorProvider, s_instance);

KoHashGeneratorProvider::KoHashGeneratorProvider()
{
    // Initialize default generators
    hashGenerators.insert("MD5", new KoMD5Generator());
}

KoHashGeneratorProvider::~KoHashGeneratorProvider()
{
    qDeleteAll(hashGenerators);
}

KoHashGenerator *KoHashGeneratorProvider::getGenerator(const QString &algorithm)
{
    QMutexLocker locker(&mutex);
    return hashGenerators.value(algorithm);
}

void KoHashGeneratorProvider::setGenerator(const QString &algorithm, KoHashGenerator *generator)
{
    if (hashGenerators.contains(algorithm)) {
        delete hashGenerators.take(algorithm);
        hashGenerators[algorithm] = generator;
    } else
        hashGenerators.insert(algorithm, generator);
}

KoHashGeneratorProvider *KoHashGeneratorProvider::instance()
{
    return s_instance;
}
