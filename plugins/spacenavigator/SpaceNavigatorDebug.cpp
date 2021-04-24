/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SpaceNavigatorDebug.h"

const QLoggingCategory &SPACENAVIGATOR_LOG()
{
    static const QLoggingCategory category("calligra.plugin.spacenavigatordevice");
    return category;
}
