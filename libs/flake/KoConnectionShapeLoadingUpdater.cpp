/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoConnectionShapeLoadingUpdater.h"

#include "KoConnectionShape.h"

KoConnectionShapeLoadingUpdater::KoConnectionShapeLoadingUpdater(KoConnectionShape *connectionShape, ConnectionPosition position)
    : m_connectionShape(connectionShape)
    , m_position(position)
{
}

KoConnectionShapeLoadingUpdater::~KoConnectionShapeLoadingUpdater() = default;

void KoConnectionShapeLoadingUpdater::update(KoShape *shape)
{
    if (m_position == First) {
        m_connectionShape->connectFirst(shape, m_connectionShape->firstConnectionId());
    } else {
        m_connectionShape->connectSecond(shape, m_connectionShape->secondConnectionId());
    }
    m_connectionShape->finishLoadingConnection();
}
