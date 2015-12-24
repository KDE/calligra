/*
   Copyright (c) 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
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
