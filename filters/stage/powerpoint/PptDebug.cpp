/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "PptDebug.h"

const QLoggingCategory &PPT_LOG()
{
    static const QLoggingCategory category("calligra.filter.ppt2odp");
    return category;
}
