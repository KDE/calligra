/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
