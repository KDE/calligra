/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOEVENTACTIONREGISTRY_H
#define KOEVENTACTIONREGISTRY_H

#include <QList>
#include <QSet>

#include "KoXmlReaderForward.h"
#include "flake_export.h"
class KoEventAction;
class KoEventActionFactoryBase;
class KoShapeLoadingContext;

/**
 * This singleton keeps a register of all available KoEventActionFactoryBase objects.
 *
 * It creates the event actions when loaded from odf.
 */
class FLAKE_EXPORT KoEventActionRegistry
{
public:
    class Singleton;
    KoEventActionRegistry();
    ~KoEventActionRegistry();

    /**
     * Return an instance of the KoEventActionRegistry
     */
    static KoEventActionRegistry *instance();

    /**
     * Create action events for the elements given
     */
    QSet<KoEventAction *> createEventActionsFromOdf(const KoXmlElement &element, KoShapeLoadingContext &context) const;

    /**
     * Add presentation event action.
     */
    void addPresentationEventAction(KoEventActionFactoryBase *factory);

    /**
     * Add script event action.
     */
    void addScriptEventAction(KoEventActionFactoryBase *factory);

    /**
     * Get presentation event actions.
     */
    QList<KoEventActionFactoryBase *> presentationEventActions();

    /**
     * Get script event actions.
     */
    QList<KoEventActionFactoryBase *> scriptEventActions();

private:
    KoEventActionRegistry(const KoEventActionRegistry &) = delete;
    KoEventActionRegistry operator=(const KoEventActionRegistry &) = delete;

    void init();

    class Private;
    Private *d;
};

#endif /* KOEVENTACTIONREGISTRY_H */
