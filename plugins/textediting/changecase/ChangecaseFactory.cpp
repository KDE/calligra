/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ChangecaseFactory.h"
#include "Changecase.h"

#include <KLocalizedString>

ChangecaseFactory::ChangecaseFactory()
    : KoTextEditingFactory("changecase")
{
    setShowInMenu(true);
    setTitle(i18n("Change Case..."));
}

KoTextEditingPlugin *ChangecaseFactory::create() const
{
    return new Changecase();
}
