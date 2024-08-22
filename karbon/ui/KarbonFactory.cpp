/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2001-2003 Lennart Kudling <kudling@kde.org>
 * SPDX-FileCopyrightText: 2002-2003 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2002 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
 * SPDX-FileCopyrightText: 2002, 2004-2005, 2007 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2002 Benoît Vautrin <benoit.vautrin@free.fr>
 * SPDX-FileCopyrightText: 2003 Lukáš Tinkl <lukas@kde.org>
 * SPDX-FileCopyrightText: 2004, 2006 Laurent Montel <montel@kde.org>
 * SPDX-FileCopyrightText: 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * SPDX-FileCopyrightText: 2005, 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2007 Matthias Kretz <kretz@kde.org>
 * SPDX-FileCopyrightText: 2007 Stephan Kulow <coolo@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KarbonFactory.h"

#include "KarbonAboutData.h"
#include "KarbonDocument.h"
#include "KarbonPart.h"

#include <KoComponentData.h>
#include <KoPluginLoader.h>

#include <KIconLoader>

KoComponentData *KarbonFactory::s_global = nullptr;

KarbonFactory::KarbonFactory()
    : KPluginFactory()
{
    (void)global();
}

KarbonFactory::~KarbonFactory() = default;

QObject *KarbonFactory::create(const char * /*iface*/, QWidget * /*parentWidget*/, QObject *parent, const QVariantList &args)
{
    Q_UNUSED(args);

    KarbonPart *part = new KarbonPart(parent);
    KarbonDocument *doc = new KarbonDocument(part);
    part->setDocument(doc);
    return part;
}

const KSharedConfig::Ptr &KarbonFactory::karbonConfig()
{
    return global().config();
}

const KoComponentData &KarbonFactory::global()
{
    if (!s_global) {
        KAboutData *aboutData = newKarbonAboutData();
        s_global = new KoComponentData(*aboutData);
        delete aboutData;

        // Add any application-specific resource directories here

        // Tell the iconloader about share/apps/calligra/icons
        KIconLoader::global()->addAppDir("calligra");

        // Load Karbon specific dockers.
        KoPluginLoader::load(QStringLiteral("karbon/dockers"));
        KoPluginLoader::load(QStringLiteral("calligra/pageapptools"));
    }
    return *s_global;
}
