/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOEVENTACTIONFACTORY_H
#define KOEVENTACTIONFACTORY_H

#include <QString>

#include "flake_export.h"

class KoEventAction;
class QWidget;

/**
 * Factory for creating KoEventActions
 *
 * This is the base class for event action plugins. Each plugin that ships a KoEventAction should
 * also ship a factory.
 */
class FLAKE_EXPORT KoEventActionFactoryBase
{
public:
    /**
     * Factory to create events
     *
     * @param id The id of the event action
     * @param action Only presentation event actions need to set the action. It is not used
     *               for script event actions.
     */
    explicit KoEventActionFactoryBase(const QString &id, const QString &action = QString());
    virtual ~KoEventActionFactoryBase();

    /**
     * Create the event action.
     */
    virtual KoEventAction *createEventAction() = 0;

    /**
     * Create the widget to configure the action
     */
    virtual QWidget *createOptionWidget() = 0;

    /**
     * The action is used to differentiate presentation effects.
     */
    QString action() const;

    /**
     * return the id for the variable this factory creates.
     * @return the id for the variable this factory creates.
     */
    QString id() const;

private:
    class Private;
    Private *const d;
};

#endif /* KOEVENTACTIONFACTORY_H */
