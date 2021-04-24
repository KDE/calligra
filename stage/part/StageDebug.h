/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef STAGE_DEBUG_H
#define STAGE_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

#include <stage_export.h>

extern const STAGE_EXPORT QLoggingCategory &STAGE_LOG();

#define debugStage qCDebug(STAGE_LOG)
#define warnStage qCWarning(STAGE_LOG)
#define errorStage qCCritical(STAGE_LOG)

extern const STAGE_EXPORT QLoggingCategory &STAGEANIMATION_LOG();

#define debugStageAnimation qCDebug(STAGEANIMATION_LOG)
#define warnStageAnimation qCWarning(STAGEANIMATION_LOG)
#define errorStageAnimation qCCritical(STAGEANIMATION_LOG)

extern const STAGE_EXPORT QLoggingCategory &STAGEPAGEEFFECT_LOG();

#define debugStagePageEffect qCDebug(STAGEPAGEEFFECT_LOG)
#define warnStagePageEffect qCWarning(STAGEPAGEEFFECT_LOG)
#define errorStagePageEffect qCCritical(STAGEPAGEEFFECT_LOG)

#endif
