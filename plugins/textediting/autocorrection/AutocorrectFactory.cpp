/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "AutocorrectFactory.h"
#include "Autocorrect.h"

#include <KLocalizedString>

AutocorrectFactory::AutocorrectFactory()
    : KoTextEditingFactory("autocorrection")
{
    setShowInMenu(true);
    setTitle(i18n("Autocorrect"));
}

KoTextEditingPlugin *AutocorrectFactory::create() const
{
    return new Autocorrect();
}
