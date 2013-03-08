/* This file is part of the KDE project
 *
 * Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>
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


// Own
#include "KoOdfStyleManager.h"

// Qt
#include <QHash>
#include <QString>

// odf lib
#include "KoOdfStyle.h"


// ================================================================
//                 class KoOdfStyleManager


class KoOdfStyleManager::Private
{
public:
    Private() {};

    QHash<QString, KoOdfStyle*> styles;         // name, style
    QHash<QString, KoOdfStyle*> defaultStyles;  // family, style
};


// ----------------------------------------------------------------


KoOdfStyleManager::KoOdfStyleManager()
    : d(new KoOdfStyleManager::Private())
{
}

KoOdfStyleManager::~KoOdfStyleManager()
{
    delete d;
}


KoOdfStyle *KoOdfStyleManager::style(QString &name) const
{
    return d->styles.value(name, 0);
}

void KoOdfStyleManager::setStyle(QString &name, KoOdfStyle *style)
{
    d->styles.insert(name, style);
}


KoOdfStyle *KoOdfStyleManager::defaultStyle(QString &family) const
{
    return d->defaultStyles.value(family, 0);
}

void KoOdfStyleManager::setDefaultStyle(QString &name, KoOdfStyle *family)
{
    d->styles.insert(name, family);
}


void KoOdfStyleManager::clear()
{
    qDeleteAll(d->styles);
    qDeleteAll(d->defaultStyles);
}


