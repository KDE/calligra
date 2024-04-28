/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPREVENTACTIONDATA_H
#define KPREVENTACTIONDATA_H

#include "stage_export.h"

class KPrSoundCollection;
class KoShape;
class KoEventAction;
class KPrSoundCollection;

class STAGE_EXPORT KPrEventActionData
{
public:
    KPrEventActionData(KoShape *shape, KoEventAction *eventAction, KPrSoundCollection *soundCollection);
    ~KPrEventActionData();

    KPrSoundCollection *soundCollection() const;

    /**
     * Get the shape
     *
     * @return the shape or 0 if there is none.
     */
    KoShape *shape() const;

    /**
     * Get the event action
     *
     * @return the event action or 0 if there is none
     */
    KoEventAction *eventAction() const;

private:
    class Private;
    Private *const d;
};

#endif /* KPREVENTACTIONDATA_H */
