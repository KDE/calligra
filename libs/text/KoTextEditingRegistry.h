/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTEDITINGREGISTRY_H
#define KOTEXTEDITINGREGISTRY_H

#include <KoGenericRegistry.h>
#include <KoTextEditingFactory.h>

/**
 * This singleton class keeps a register of all available text editing plugins.
 * The text editing plugins are all about handling user input while (s)he
 * is editing the text. A plugin can do near everything with the typed text,
 * including altering it and adding markup. The plugin gives events when a
 * word and when a paragraph has been finished. Which is ideal for autocorrection
 * and autoreplacement of text.
 * @see KoTextEditingFactory
 * @see KoTextEditingPlugin
 */
class KOTEXT_EXPORT KoTextEditingRegistry : public KoGenericRegistry<KoTextEditingFactory *>
{
public:
    /**
     * Return an instance of the KoTextEditingRegistry
     * Creates an instance if that has never happened before and returns the singleton instance.
     */
    KoTextEditingRegistry() = default;
    ~KoTextEditingRegistry() override;
    static KoTextEditingRegistry *instance();

private:
    void init();
};

#endif
