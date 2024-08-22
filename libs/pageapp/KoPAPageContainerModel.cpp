/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoPAPageContainerModel.h"

#include "KoPAPageBase.h"
#include <PageAppDebug.h>

KoPAPageContainerModel::KoPAPageContainerModel() = default;

KoPAPageContainerModel::~KoPAPageContainerModel() = default;

void KoPAPageContainerModel::childChanged(KoShape *child, KoShape::ChangeType type)
{
    Q_UNUSED(type);
    // debugPageApp << "page changed" << child->parent();
    if (KoPAPageBase *page = dynamic_cast<KoPAPageBase *>(child->parent())) {
        page->pageUpdated();
    }
}
