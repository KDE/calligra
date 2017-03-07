/*
 *  Copyright (c) 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation; either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef ODFREADER_DEBUG_H_
#define ODFREADER_DEBUG_H_

#include <QDebug>
#include <QLoggingCategory>

#include "koodfreader_export.h"

extern const KOODFREADER_EXPORT QLoggingCategory &ODFREADER_LOG();

#define debugOdfReader qCDebug(ODFREADER_LOG)
#define warnOdfReader qCWarning(ODFREADER_LOG)
#define errorOdfReader qCCritical(ODFREADER_LOG)

#endif
