/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Sebastian Sauer <mail@dipe.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KO_USER_VARIABLE_FACTORY
#define KO_USER_VARIABLE_FACTORY

#include <KoInlineObjectFactoryBase.h>

class UserVariableFactory : public KoInlineObjectFactoryBase
{
public:
    UserVariableFactory();

    KoInlineObject *createInlineObject(const KoProperties *properties = nullptr) const override;
};

#endif
