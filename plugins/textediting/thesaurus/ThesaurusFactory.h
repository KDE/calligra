/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef THESAURUS_FACTORY
#define THESAURUS_FACTORY

#include <KoTextEditingFactory.h>

class KoTextEditingPlugin;

class ThesaurusFactory : public KoTextEditingFactory
{
public:
    explicit ThesaurusFactory();
    ~ThesaurusFactory() override = default;

    KoTextEditingPlugin *create() const override;
};

#endif
