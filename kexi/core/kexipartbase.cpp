/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2014 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexipartbase.h"
#include <kexiutils/utils.h>

namespace KexiPart
{
//! @internal
class PartBase::Private
{
public:
    Private()
    : info(0)
    {
    }
    Info *info;
};
}

using namespace KexiPart;

PartBase::PartBase(QObject *parent,
                   const QVariantList& list)
    : QObject(parent)
    , d(new Private)
{
    Q_UNUSED(list);
}

PartBase::~PartBase()
{
    delete d;
}

void PartBase::setInfo(Info *info)
{
    d->info = info;
}

Info* PartBase::info() const
{
    return d->info;
}

KLocalizedString PartBase::i18nMessage(const QString& englishMessage, KexiWindow* window) const
{
    Q_UNUSED(window);
    if (QString(englishMessage).startsWith(':'))
        return KLocalizedString();
    return ki18n(englishMessage.toLatin1());
}

void PartBase::setupCustomPropertyPanelTabs(QTabWidget *)
{
}

