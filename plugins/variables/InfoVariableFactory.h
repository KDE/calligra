/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Pierre Ducroquet <pinaraf@gmail.com>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KO_INFO_VARIABLE_FACTORY
#define KO_INFO_VARIABLE_FACTORY

#include <KoInlineObjectFactoryBase.h>

class InfoVariableFactory : public KoInlineObjectFactoryBase
{
public:
    InfoVariableFactory();

    KoInlineObject *createInlineObject(const KoProperties *properties = nullptr) const override;
};

#endif
