/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __KoChangeTrackerDisabledRAII_h__
#define __KoChangeTrackerDisabledRAII_h__

class KoChangeTracker;

/**
 * @short Disable a change tracker and automatically reset it when
 *        this object is destroyed.
 *
 * Resource Acquisition Is Initialization pattern to temporarily
 * disable the ChangeTracker. Useful for cases where high level
 * activities like applying a semantic stylesheet are performed where
 * you might like to add a higher level action to the change tracker
 * than just text substitution.
 *
 * @author Ben Martin <ben.martin@kogmbh.com>
 * @see KoChangeTracker
 *
 */
class KoChangeTrackerDisabledRAII
{
public:
    explicit KoChangeTrackerDisabledRAII(KoChangeTracker *changeTracker);
    ~KoChangeTrackerDisabledRAII();

private:
    KoChangeTracker *m_changeTracker;
    bool m_oldval;
};

#endif
