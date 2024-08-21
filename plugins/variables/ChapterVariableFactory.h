/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Sebastian Sauer <mail@dipe.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KO_CHAPTER_VARIABLE_FACTORY
#define KO_CHAPTER_VARIABLE_FACTORY

#include <KoInlineObjectFactoryBase.h>

class ChapterVariableFactory : public KoInlineObjectFactoryBase
{
public:
    ChapterVariableFactory();

    KoInlineObject *createInlineObject(const KoProperties *properties = nullptr) const override;
};

#endif
