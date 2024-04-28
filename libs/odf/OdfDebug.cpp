/*
 *  SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "OdfDebug.h"

const QLoggingCategory &ODF_LOG()
{
    static const QLoggingCategory category("calligra.lib.odf");
    return category;
}
