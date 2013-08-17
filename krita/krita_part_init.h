/* This file is part of the KDE project
 * Copyright (C) 2013 Boudewijn Rempt <boud@valdyas.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef KRITAPARTINIT_H
#define KRITAPARTINIT_H

#include <kis_factory2.h>

class KritaPartInit : public KisFactory2
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KPluginFactory_iid FILE "kritapart.json")
    Q_INTERFACES(KPluginFactory)


public:
    explicit KritaPartInit(QObject *parent = 0) : KisFactory2(parent) {}
    virtual ~KirtaPartInit() {}

};

#endif // FLOWFACTORYINIT_H
