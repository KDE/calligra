/* This file is part of the KDE project
   Copyright (C) 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KOCOMPONENTDATA_H
#define KOCOMPONENTDATA_H

#include "komain_export.h"

#include <KSharedConfig>

#include <QExplicitlySharedDataPointer>

class QString;
class KAboutData;
class KoComponentDataPrivate;

/**
 * Data about a component.
 */
class KOMAIN_EXPORT KoComponentData
{
public:
    /**
     * Constructor.
     *
     * A copy of the aboutData object is made.
     *
     * @param aboutData data about this component
     *
     * @see KAboutData
     */
    explicit KoComponentData(const KAboutData &aboutData);

    /**
     * Copy constructor.
     *
     * It does not copy the data. The data is shared between the old and new objects.
     */
    KoComponentData(const KoComponentData&);

    /**
     * Destructor.
     */
    virtual ~KoComponentData();

    /**
     * Assignment operator.
     *
     * It does not copy the data. The data is shared between the old and new objects.
     */
    KoComponentData &operator=(const KoComponentData&);

    /**
     * Returns whether two objects reference the same data.
     */
    bool operator==(const KoComponentData&) const;

    /**
     * Returns whether two objects do not reference the same data.
     */
    bool operator!=(const KoComponentData &rhs) const { return !operator==(rhs); }

    /**
     * Returns the general config object (componentName"rc").
     * @return the KConfig object for the component.
     */
    const KSharedConfig::Ptr &config() const;

    /**
     * Returns the complete aboutdata of the component.
     *
     * @return The component name.
     */
    const KAboutData &aboutData() const;

    /**
     * Returns the name of the component.
     *
     * @return The component name.
     */
    QString componentName() const;

    /**
     * Returns the display name of the component.
     *
     * @return The component display name.
     */
    QString componentDisplayName() const;

private:
    QExplicitlySharedDataPointer<KoComponentDataPrivate> d;
};

#endif
