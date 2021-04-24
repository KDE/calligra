// This file is part of KSpread
// SPDX-FileCopyrightText: 2016 Friedrich W. H. Kossebau <kossebau@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "ScriptingDebug.h"

const QLoggingCategory &SHEETSSCRIPTING_LOG()
{
    static const QLoggingCategory category("calligra.sheets.scripting");
    return category;
}
