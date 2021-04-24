/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SPELLCHECK_FACTORY
#define SPELLCHECK_FACTORY

#include <KoTextEditingFactory.h>

class KoTextEditingPlugin;

class SpellCheckFactory : public KoTextEditingFactory
{
public:
    explicit SpellCheckFactory();

    KoTextEditingPlugin *create() const override;
};

#endif
