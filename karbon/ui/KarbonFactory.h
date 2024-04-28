/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2001-2002 Lennart Kudling <kudling@kde.org>
 * SPDX-FileCopyrightText: 2002-2003 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2005 Laurent Montel <montel@kde.org>
 * SPDX-FileCopyrightText: 2005 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Inge Wallin <inge@lysator.liu.se>
 * SPDX-FileCopyrightText: 2006 Tim Beaulen <tbscope@gmail.com>
 * SPDX-FileCopyrightText: 2007 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2007 Matthias Kretz <kretz@kde.org>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef __KARBON_FACTORY_H__
#define __KARBON_FACTORY_H__

#include <KPluginFactory>
#include <KSharedConfig>

#include <karbonui_export.h>

class KoComponentData;

class KARBONUI_EXPORT KarbonFactory : public KPluginFactory
{
    Q_OBJECT

public:
    explicit KarbonFactory();
    ~KarbonFactory() override;

    QObject *create(const char *iface, QWidget *parentWidget, QObject *parent, const QVariantList &args) override;

    static const KSharedConfig::Ptr &karbonConfig();
    static const KoComponentData &global();

private:
    static KoComponentData *s_global;
};

#endif
