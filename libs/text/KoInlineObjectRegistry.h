/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOINLINEOBJECTREGISTRY_H
#define KOINLINEOBJECTREGISTRY_H

#include "kotext_export.h"
#include <KoGenericRegistry.h>
#include <KoInlineObjectFactoryBase.h>
#include <KoXmlReaderForward.h>
#include <QList>

class KoCanvasBase;
class QAction;
class KoShapeLoadingContext;

/**
 * This singleton class keeps a register of all available InlineObject factories.
 * @see KoInlineObjectFactoryBase
 * @see KoInlineTextObjectManager
 * @see KoInlineObject
 * @see KoVariable
 */
class KOTEXT_EXPORT KoInlineObjectRegistry : public KoGenericRegistry<KoInlineObjectFactoryBase *>
{
public:
    KoInlineObjectRegistry();
    ~KoInlineObjectRegistry() override;

    /**
     * Return an instance of the KoInlineObjectRegistry
     * Creates an instance if that has never happened before and returns the singleton instance.
     */
    static KoInlineObjectRegistry *instance();

    /**
     * Create a list of actions that can be used to plug into a menu, for example.
     * This method will find all the InlineObjectFactories that are installed in the system and
     * find out which object they provide. If a factory provides a variable, then all its
     * templates will be added to the response.
     * Each of these actions, when executed, will insert the relevant variable in the current text-position.
     * The actions assume that the text tool is selected, if that's not the case then they will silently fail.
     * @param host the canvas for which these actions are created.  Note that the actions will get these
     *  actions as a parent (for memory management purposes) as well.
     * @see KoInlineTextObjectManager::createInsertVariableActions()
     */
    QList<QAction *> createInsertVariableActions(KoCanvasBase *host) const;

    /**
     * Use the element to find out which variable plugin can load it, and returns the loaded
     * variable. The element expected is one of 'text:subject', 'text:date' / etc.
     *
     * @returns the variable or 0 if no variable could be created
     */
    KoInlineObject *createFromOdf(const KoXmlElement &element, KoShapeLoadingContext &context) const;

private:
    KoInlineObjectRegistry(const KoInlineObjectRegistry &) = delete;
    KoInlineObjectRegistry operator=(const KoInlineObjectRegistry &) = delete;

    class Private;
    Private *const d;
};

#endif
