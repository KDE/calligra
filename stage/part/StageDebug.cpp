/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "StageDebug.h"

const QLoggingCategory &STAGE_LOG()
{
    static const QLoggingCategory category("calligra.stage");
    return category;
}

const QLoggingCategory &STAGEANIMATION_LOG()
{
    static const QLoggingCategory category("calligra.stage.animation");
    return category;
}

const QLoggingCategory &STAGEPAGEEFFECT_LOG()
{
    static const QLoggingCategory category("calligra.stage.pageeffect");
    return category;
}
