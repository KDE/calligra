/* This file is part of the KDE project
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2010-2011 Jarosław Staniek <staniek@kde.org>
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

#include "KoAbstractApplicationWindow.h"

class KoAbstractApplicationWindow::Private
{
public:
    Private() {}
    KoAbstractApplicationController *controller;
};

KoAbstractApplicationWindow::KoAbstractApplicationWindow(
    KoAbstractApplicationController *controller)
    : d(new Private)
{
    d->controller = controller;
}

KoAbstractApplicationWindow::~KoAbstractApplicationWindow()
{
    delete d;
}

KoAbstractApplicationController* KoAbstractApplicationWindow::controller() const
{
    return d->controller;
}
