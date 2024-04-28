/*  This file is part of the KDE project

    SPDX-FileCopyrightText: 1999 Matthias Elter <elter@kde.org>
    SPDX-FileCopyrightText: 2003 Patrick Julien <freak@codepimps.org>
    SPDX-FileCopyrightText: 2005 Sven Langkamp <sven.langkamp@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef FILTERRESOURCESERVERPROVIDER_H
#define FILTERRESOURCESERVERPROVIDER_H

#include "KoResourceServer.h"

class KoResourceLoaderThread;
class FilterEffectResource;

/// Provides resource server for filter effect resources
class FilterResourceServerProvider : public QObject
{
    Q_OBJECT

public:
    ~FilterResourceServerProvider() override;

    static FilterResourceServerProvider *instance();

    KoResourceServer<FilterEffectResource> *filterEffectServer();

private:
    FilterResourceServerProvider();
    FilterResourceServerProvider(const FilterResourceServerProvider &);
    FilterResourceServerProvider operator=(const FilterResourceServerProvider &);

    static FilterResourceServerProvider *m_singleton;
    KoResourceServer<FilterEffectResource> *m_filterEffectServer;
    KoResourceLoaderThread *m_filterEffectThread;
};

#endif // FILTERRESOURCESERVERPROVIDER_H
