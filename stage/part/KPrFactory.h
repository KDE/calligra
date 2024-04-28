// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRFACTORY_H
#define KPRFACTORY_H

#include "stage_export.h"
#include <KPluginFactory>

class KAboutData;
class KoComponentData;

class STAGE_EXPORT KPrFactory : public KPluginFactory
{
    Q_OBJECT
public:
    explicit KPrFactory();
    ~KPrFactory() override;

    QObject *create(const char *iface, QWidget *parentWidget, QObject *parent, const QVariantList &args) override;
    static const KoComponentData &componentData();

    static KAboutData *aboutData();

private:
    static KoComponentData *s_instance;
    static KAboutData *s_aboutData;
};

#endif
