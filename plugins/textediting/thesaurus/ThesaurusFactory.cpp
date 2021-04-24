/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ThesaurusFactory.h"
#include "Thesaurus.h"

ThesaurusFactory::ThesaurusFactory()
    : KoTextEditingFactory("thesaurustool")
{
    setShowInMenu(false);
    setTitle("Thesaurus");
}

KoTextEditingPlugin *ThesaurusFactory::create() const
{
    return new Thesaurus();
}
