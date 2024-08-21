/*
* This file is part of the KDE project
*
* SPDX-FileCopyrightText: 2009 Nokia Corporation and /or its subsidiary(-ies).
*
* Contact: Amit Aggarwal <amitcs06@gmail.com>
*
SPDX-License-Identifier: LGPL-2.1-or-later
*
*/

#ifndef PRESENTATION_VARIABLE_FACTORY_H
#define PRESENTATION_VARIABLE_FACTORY_H

#include <KoInlineObjectFactoryBase.h>

class PresentationVariableFactory : public KoInlineObjectFactoryBase
{
public:
    PresentationVariableFactory();

    /// reimplemented
    KoInlineObject *createInlineObject(const KoProperties *properties = nullptr) const override;
};

#endif
