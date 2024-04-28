/*
 *  SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "Odf2Debug.h"

const QLoggingCategory &ODF2_LOG()
{
    static const QLoggingCategory category("calligra.filter.odf2");
    return category;
}
