/*  This file is part of the KDE project

    SPDX-FileCopyrightText: 1999 Matthias Elter <elter@kde.org>
    SPDX-FileCopyrightText: 2003 Patrick Julien <freak@codepimps.org>
    SPDX-FileCopyrightText: 2005 Sven Langkamp <sven.langkamp@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KORESOURCESERVERPROVIDER_H
#define KORESOURCESERVERPROVIDER_H

#include <kowidgets_export.h>

#include <QThread>

#include <WidgetsDebug.h>

#include "KoAbstractGradient.h"
#include "KoColorSet.h"
#include "KoPattern.h"
#include "KoResourceServer.h"

/**
 * KoResourceLoaderThread allows threaded loading of the resources of a resource server
 */
class KOWIDGETS_EXPORT KoResourceLoaderThread : public QThread
{
    Q_OBJECT
public:
    /**
     * Constructs a KoResourceLoaderThread for a server
     * @param server the server the resources will be loaded for
     */
    explicit KoResourceLoaderThread(KoResourceServerBase *server);
    ~KoResourceLoaderThread() override = default;
public Q_SLOTS:
    /**
     * Checks whether the thread has finished loading and waits
     * until it is finished if necessary
     */
    void barrier();

protected:
    /**
     * Overridden from QThread
     */
    void run() override;

private:
    KoResourceServerBase *m_server;
    QStringList m_fileNames;
};

/**
 * Provides default resource servers for gradients, patterns and palettes
 */
class KOWIDGETS_EXPORT KoResourceServerProvider : public QObject
{
    Q_OBJECT

public:
    KoResourceServerProvider();
    ~KoResourceServerProvider() override;

    static KoResourceServerProvider *instance();

    KoResourceServer<KoPattern> *patternServer(bool block = true);
    KoResourceServer<KoAbstractGradient> *gradientServer(bool block = true);
    KoResourceServer<KoColorSet> *paletteServer(bool block = true);

private:
    KoResourceServerProvider(const KoResourceServerProvider &);
    KoResourceServerProvider operator=(const KoResourceServerProvider &);

private:
    struct Private;
    Private *const d;
};

#endif // KORESOURCESERVERPROVIDER_H
