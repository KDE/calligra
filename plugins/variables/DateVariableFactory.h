/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KO_DATE_VARIABLE_FACTORY
#define KO_DATE_VARIABLE_FACTORY

#include <KoInlineObjectFactoryBase.h>

class DateVariableFactory : public KoInlineObjectFactoryBase
{
public:
    DateVariableFactory();

    KoInlineObject *createInlineObject(const KoProperties *properties = nullptr) const override;
};

#endif
