/*
 *  SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "TextLayoutDebug.h"

const QLoggingCategory &TEXT_LAYOUT_LOG()
{
    static const QLoggingCategory category("calligra.lib.textlayout");
    return category;
}
