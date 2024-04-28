/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KPRPAGEEFFECTFACTORY_H
#define KPRPAGEEFFECTFACTORY_H

#include <QPair>

#include "KPrPageEffect.h"

#include "stage_export.h"

/**
 * Base class for a page effect factories
 */
class STAGE_EXPORT KPrPageEffectFactory
{
public:
    struct Properties {
        Properties(int duration, int subType)
            : duration(duration)
            , subType(subType)
        {
        }

        int duration;
        int subType;
    };

    /**
     * Constructor
     *
     * @param id The id of the page effect the factory is creating
     * @param name The name of the effect. This name is used in the UI
     * @param subTypes The possible subtypes of the page effect
     */
    KPrPageEffectFactory(const QString &id, const QString &name);

    virtual ~KPrPageEffectFactory();

    /**
     * Create a page effect
     *
     * @param properties The properties for creating a page effect
     */
    KPrPageEffect *createPageEffect(const Properties &properties) const;

    /**
     * Create a page effect
     *
     * This is used for loading a page effect from odf
     *
     * @param element The element containing the information for creating the page effect
     */
    KPrPageEffect *createPageEffect(const KoXmlElement &element) const;

    /**
     * Get the id of the page effect
     */
    QString id() const;

    /**
     * Get the name of the page effect
     */
    QString name() const;

    /**
     * Get the sub types of the page effect
     */
    QList<int> subTypes() const;

    /**
     * Get the tags the factory is responsible for
     *
     * This defines for which smil:type and smil:direction this factory is responsible.
     * If the bool is false the smil:direction is forward if it is true the smil:direction
     * is reverse.
     */
    QList<QPair<QString, bool>> tags() const;

    /**
     * Get the (i18n'ed) name of a given subtype.
     */
    virtual QString subTypeName(int subType) const = 0;

    /**
     * Get the subtypes sorted by name. This sorts by the i18n'ed name.
     */
    QMultiMap<QString, int> subTypesByName() const;

protected:
    /**
     * Add a strategy to the factory
     *
     * @param strategy The strategy to add
     */
    void addStrategy(KPrPageEffectStrategy *strategy);

private:
    struct Private;
    Private *const d;
};

#endif /* KPRPAGEEFFECTFACTORY_H */
