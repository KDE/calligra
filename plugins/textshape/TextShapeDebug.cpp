/*
   SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "TextShapeDebug.h"

const QLoggingCategory &TEXTSHAPE_LOG()
{
    static const QLoggingCategory category("calligra.plugin.text.shape");
    return category;
}
