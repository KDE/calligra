/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTEDITINGFACTORY_H
#define KOTEXTEDITINGFACTORY_H

#include "kotext_export.h"

class KoTextEditingPlugin;
class QString;

/**
 * A factory for text editing plugins. There should be one for each plugin type to
 * allow the creation of the text-editing-class from that plugin.
 * @see KoTextEditingRegistry
 */
class KOTEXT_EXPORT KoTextEditingFactory
{
public:
    /**
     * Create the new factory
     * @param id a string that will be used internally for referencing the variable-type.
     */
    explicit KoTextEditingFactory(const QString &id);
    virtual ~KoTextEditingFactory();

    /**
     * Create a new instance of an inline object.
     */
    virtual KoTextEditingPlugin *create() const = 0;

    /**
     * return the id for the plugin this factory creates.
     * @return the id for the plugin this factory creates.
     */
    QString id() const;

    /**
     * return if the plugin this factory creates has to be shown in the context menu.
     * @see KoTextEditingPlugin::checkSection()
     */
    bool showInMenu() const;

    /// If showInMenu() returns true; the returned text is used in the context menu.
    QString title() const;

protected:
    /**
     * Set if the plugin this factory creates has to be shown in the context menu.
     * @see KoTextEditingPlugin::checkSection()
     */
    void setShowInMenu(bool show);
    /// set the title used in the context menu
    void setTitle(const QString &title);

private:
    class Private;
    Private *const d;
};

#endif
