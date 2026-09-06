/*
 * SPDX-FileCopyrightText: 2026 Calligra contributors
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once

#include "kundo2_export.h"

struct KUNDO2_EXPORT KUndo2CumulativeUndoData {
    int excludeFromMerge = 2;
    int mergeTimeout = 5000;
    int maxGroupSeparation = 1000;
    int maxGroupDuration = 5000;
};
