/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoChangeTrackerDisabledRAII.h"
#include <changetracker/KoChangeTracker.h>

KoChangeTrackerDisabledRAII::KoChangeTrackerDisabledRAII(KoChangeTracker *changeTracker)
    : m_changeTracker(changeTracker)
    , m_oldval(true)
{
    if (changeTracker) {
        m_oldval = changeTracker->recordChanges();
        changeTracker->setRecordChanges(false);
    }
}

KoChangeTrackerDisabledRAII::~KoChangeTrackerDisabledRAII()
{
    if (m_changeTracker) {
        m_changeTracker->setRecordChanges(m_oldval);
    }
}
