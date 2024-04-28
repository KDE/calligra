/*
 *  SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "StoreDebug.h"

const QLoggingCategory &STORE_LOG()
{
    static const QLoggingCategory category("calligra.lib.store");
    return category;
}
