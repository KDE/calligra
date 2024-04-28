/*
 *  SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "TextDebug.h"

const QLoggingCategory &TEXT_LOG()
{
    static const QLoggingCategory category("calligra.lib.text");
    return category;
}
