/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CHANGECASE_FACTORY
#define CHANGECASE_FACTORY

#include <KoTextEditingFactory.h>

class KoTextEditingPlugin;

class ChangecaseFactory : public KoTextEditingFactory
{
public:
    explicit ChangecaseFactory();
    ~ChangecaseFactory() override = default;

    KoTextEditingPlugin *create() const override;
};

#endif
