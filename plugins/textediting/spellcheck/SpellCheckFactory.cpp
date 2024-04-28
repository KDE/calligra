/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SpellCheckFactory.h"
#include "SpellCheck.h"

#include <KLocalizedString>

SpellCheckFactory::SpellCheckFactory()
    : KoTextEditingFactory("spellcheck")
{
    setShowInMenu(true);
    setTitle(i18n("Check Spelling"));
}

KoTextEditingPlugin *SpellCheckFactory::create() const
{
    return new SpellCheck();
}
