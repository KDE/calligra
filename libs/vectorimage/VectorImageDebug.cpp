/*
 *  SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "VectorImageDebug.h"
const QLoggingCategory &VECTOR_IMAGE_LOG()
{
    static const QLoggingCategory category("calligra.lib.vectorimage");
    return category;
}
