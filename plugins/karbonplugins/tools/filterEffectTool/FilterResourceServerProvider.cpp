/*  This file is part of the KDE project

    SPDX-FileCopyrightText: 1999 Matthias Elter <elter@kde.org>
    SPDX-FileCopyrightText: 2003 Patrick Julien <freak@codepimps.org>
    SPDX-FileCopyrightText: 2005 Sven Langkamp <sven.langkamp@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "FilterResourceServerProvider.h"
#include "FilterEffectResource.h"

#include <QDir>
#include <QFileInfo>

#include "KoSegmentGradient.h"
#include "KoStopGradient.h"
#include <KoResourcePaths.h>
#include <KoResourceServerProvider.h>

FilterResourceServerProvider *FilterResourceServerProvider::m_singleton = nullptr;

FilterResourceServerProvider::FilterResourceServerProvider()
{
    KoResourcePaths::addResourceType("ko_effects", "data", "karbon/effects/");

    m_filterEffectServer = new KoResourceServerSimpleConstruction<FilterEffectResource>("ko_effects", "*.svg");
    if (!QFileInfo::exists(m_filterEffectServer->saveLocation())) {
        QDir().mkpath(m_filterEffectServer->saveLocation());
    }
    m_filterEffectThread = new KoResourceLoaderThread(m_filterEffectServer);
    m_filterEffectThread->start();
}

FilterResourceServerProvider::~FilterResourceServerProvider()
{
    delete m_filterEffectThread;
    delete m_filterEffectServer;
}

FilterResourceServerProvider *FilterResourceServerProvider::instance()
{
    if (FilterResourceServerProvider::m_singleton == nullptr) {
        FilterResourceServerProvider::m_singleton = new FilterResourceServerProvider();
    }
    return FilterResourceServerProvider::m_singleton;
}

KoResourceServer<FilterEffectResource> *FilterResourceServerProvider::filterEffectServer()
{
    m_filterEffectThread->barrier();
    return m_filterEffectServer;
}
