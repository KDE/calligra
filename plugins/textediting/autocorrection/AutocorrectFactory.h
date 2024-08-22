/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef AUTOCORRECT_FACTORY
#define AUTOCORRECT_FACTORY

#include <KoTextEditingFactory.h>

class KoTextEditingPlugin;

class AutocorrectFactory : public KoTextEditingFactory
{
public:
    explicit AutocorrectFactory();
    ~AutocorrectFactory() override = default;

    KoTextEditingPlugin *create() const override;
};

#endif
