/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KODRAGODFSAVEHELPER_P_H
#define KODRAGODFSAVEHELPER_P_H

#include <KoShapeSavingContext.h>

class KoDragOdfSaveHelperPrivate
{
public:
    KoDragOdfSaveHelperPrivate()
        : context(nullptr)
    {
    }
    ~KoDragOdfSaveHelperPrivate()
    {
        delete context;
    }

    KoShapeSavingContext *context;
};

#endif
